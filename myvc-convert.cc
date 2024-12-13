#include <iostream>
#include <memory>
#include <set>
#include <git2.h>
#include "repository.h"

using namespace myvc;

struct Comparator {
    bool operator()(const git_oid &a, const git_oid &b) const {
        return git_oid_cmp(&a, &b) < 0;
    }
};

std::map<git_oid, Hash, Comparator> cached;

Hash create_blob(git_repository *repo, git_blob *blob, Repository &myvcRepo) {
    const char *content = (char *) git_blob_rawcontent(blob);
    size_t size = git_blob_rawsize(blob);
    std::vector<char> data {content, content + size};
    Blob b {std::move(data)};
    myvcRepo.createBlob(b);
    return b.hash();
}

Hash create_tree(git_repository *repo, git_tree *tree, Repository &myvcRepo) {
    std::map<std::string, Tree::Node> nodes;
    size_t entryCnt = git_tree_entrycount(tree);
    for(size_t i = 0; i < entryCnt; ++i) {
        const git_tree_entry *entry = git_tree_entry_byindex(tree, i);
        const char *name = git_tree_entry_name(entry);
        git_object_t type = git_tree_entry_type(entry);
        const git_oid *oid = git_tree_entry_id(entry);
        bool isBlob = (type == GIT_OBJECT_BLOB);
        Hash h;
        if(type == GIT_OBJECT_TREE) {
            git_tree *subtree;
            git_tree_lookup(&subtree, repo, oid);
            h = create_tree(repo, subtree, myvcRepo);
            git_tree_free(subtree);
        } else if(type == GIT_OBJECT_BLOB) {
            git_blob *blob;
            git_blob_lookup(&blob, repo, oid);
            h = create_blob(repo, blob, myvcRepo);
            git_blob_free(blob);
        }
        nodes.insert_or_assign(name, Tree::Node {h, isBlob});
    }
    Tree t {std::move(nodes)};
    myvcRepo.createTree(t);
    return t.hash();
}

Hash create_commit(git_repository *repo, git_commit *commit, Repository &myvcRepo) {
    git_oid oid;
    git_oid_cpy(&oid, git_commit_id(commit));
    if(cached.find(oid) != cached.end()) return cached.at(oid);
    std::string msg = git_commit_message_raw(commit);
    time_t time = static_cast<time_t>(git_commit_time(commit));
    git_tree *tree;
    git_commit_tree(&tree, commit);
    Hash treeHash = create_tree(repo, tree, myvcRepo);
    git_tree_free(tree);
    std::set<Hash> parents;
    size_t parent_cnt = git_commit_parentcount(commit);
    for(size_t i = 0; i < parent_cnt; ++i) {
        git_commit *parent_commit;
        git_commit_parent(&parent_commit, commit, i);
        parents.insert(create_commit(repo, parent_commit, myvcRepo));
        git_commit_free(parent_commit);
    }
    Commit c {std::move(parents), std::move(treeHash), time, std::move(msg)};
    myvcRepo.createCommit(c);
    cached.insert_or_assign(oid, c.hash());
    return c.hash();
}

void create_objects(git_repository *repo, Repository &myvcRepo) {
    git_revwalk *walker;
    git_revwalk_new(&walker, repo);
    git_revwalk_push_glob(walker, "refs/heads/*");
    git_revwalk_push_head(walker);
    git_oid oid;
    while(git_revwalk_next(&oid, walker) == 0) {
        git_commit *commit;
        git_commit_lookup(&commit, repo, &oid);
        create_commit(repo, commit, myvcRepo);
        git_commit_free(commit);
    }
    git_revwalk_free(walker);
}

void create_branches(git_repository *repo, Repository &myvcRepo) {
    git_branch_iterator *it;
    git_branch_iterator_new(&it, repo, GIT_BRANCH_ALL);
    git_reference *ref;
    git_branch_t type;
    while(git_branch_next(&ref, &type, it) == 0) {
        const char *name;
        git_branch_name(&name, ref);
        if(type == GIT_BRANCH_LOCAL) {
            git_commit *commit;
            git_reference_peel((git_object **) &commit, ref, GIT_OBJECT_COMMIT);
            Hash h = create_commit(repo, commit, myvcRepo);
            myvcRepo.createBranch(name, std::move(h));
            git_commit_free(commit);
        }
        git_reference_free(ref);
    }
    git_branch_iterator_free(it);
}

void create_head_and_index(git_repository *repo, Repository &myvcRepo) {
    git_reference *head;
    git_repository_head(&head, repo);
    git_commit *commit;
    Head &h = myvcRepo.getHead();
    if(git_reference_type(head) == GIT_REF_COMMIT) {
        git_reference_peel((git_object **) &commit, head, GIT_OBJECT_COMMIT);
        h.setCommit(create_commit(repo, commit, myvcRepo));
        git_commit_free(commit);
    } else {
        const char *branch = git_reference_name(head);
        h.setBranch(branch);
    }
    myvcRepo.getIndex().setTree(h.getCommit().getTree().hash());
}

int main(int argc, char *argv[]) {
    const char *repoPath = argv[1];
    git_repository *repo;
    git_libgit2_init();
    git_repository_open(&repo, repoPath);
    Repository myvcRepo {"."};
    create_objects(repo, myvcRepo);
    create_branches(repo, myvcRepo);
    create_head_and_index(repo, myvcRepo);
    git_repository_free(repo);
    git_libgit2_shutdown();
    return 0;
}
