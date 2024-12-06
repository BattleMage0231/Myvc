#pragma once

#include <optional>
#include <variant>
#include <memory>
#include <functional>
#include "serialize.h"
#include "commit.h"
#include "branch.h"

namespace myvc {

class Head : public Serializable {
public:
    class Provider {
    public:
        virtual std::optional<Commit> getCommit(const Hash &) const = 0;
        virtual std::optional<const std::reference_wrapper<Branch>> getBranch(const std::string &) const = 0;
        virtual ~Provider() {};
    };

private:
    std::variant<std::monostate, std::string, Hash> state;
    std::weak_ptr<Provider> prov;

public:
    explicit Head(std::variant<std::monostate, std::string, Hash> state = {}, std::weak_ptr<Provider> prov = {});

    void write(std::ostream &) const override;
    void read(std::istream &) override;

    bool hasState() const;
    std::variant<const std::reference_wrapper<Branch>, Commit> get() const;
    Commit getCommit() const;
    void setBranch(std::string);
    void setCommit(Hash);
    void setProvider(std::weak_ptr<Provider>);
};

}
