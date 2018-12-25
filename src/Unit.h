#pragma once

#include <string>
#include <memory>

struct Term;
struct NonTerm;

struct Unit {
    std::string name;

    Unit() = default;
    explicit Unit(std::string const &);
    Term* as_term();
    NonTerm* as_non_term();

    virtual bool is_term();
    virtual bool is_non_term();

    virtual ~Unit() = default;
};

using unit_t = std::shared_ptr<Unit>;
