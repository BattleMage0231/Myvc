#pragma once

#include <optional>
#include <variant>
#include "synced.h"
#include "commit.h"
#include "branch.h"

namespace myvc {

class Head : public Synced {
public:
    class Provider {
    public:
        virtual Head getHead() const = 0;
        virtual void updateHead(const Head &) = 0;
        virtual Commit getCommit(Hash) const = 0;
        virtual Branch getBranch(const std::string &) const = 0;
        virtual ~Provider() {};
    };

private:
    std::variant<std::string, Hash> state;

public:
    explicit Head(std::variant<std::string, Hash> state = {}, std::unique_ptr<Provider> prov = {});
    explicit Head(std::unique_ptr<Provider>);
    Head(const Head &);
    Head &operator=(const Head &);

    std::vector<char> serialize() const override;
    void deserialize(std::vector<char>) override;
    void reload() override;
    void store() override;

    Commit getCommit() const;
    Commit operator*() const;
    std::optional<Branch> getBranch() const;
    void setState(std::variant<std::string, Hash>);
    void setProvider(std::unique_ptr<Provider>);
};

}
