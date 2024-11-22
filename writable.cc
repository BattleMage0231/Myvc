export module writable;

export import <filesystem>;
import <iostream>;
import <fstream>;
import <stdexcept>;

namespace myvc {

namespace fs = std::filesystem;
using std::ostream, std::ofstream, std::logic_error;

export class Writable {
    virtual void write(ostream &) const noexcept = 0;

    virtual fs::path getPath(const fs::path &) const = 0;

public:
    virtual ~Writable() {}

    void writeWithBase(const fs::path &base) const {
        const fs::path &writePath = getPath(base);
        // todo check the path and error handling
        fs::create_directories(writePath.parent_path());
        ofstream out {writePath};
        out << *this;
    }

    friend ostream &operator<<(ostream &out, const Writable &w) noexcept {
        w.write(out);
        return out;
    }
};

}
