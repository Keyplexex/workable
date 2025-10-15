#pragma once

#include <optional>
#include "concepts.h" 

template <typename Iterator>
class DropNullopt_iterator {
public:
    using value_type = typename Iterator::value_type;

    DropNullopt_iterator(Iterator current, Iterator last) : current_(current), last_(last) {
        MoveToNextValid();
    }

    bool operator==(const DropNullopt_iterator& other) const {
        return current_ == other.current_;
    }

    DropNullopt_iterator& operator++() {
        ++current_;
        MoveToNextValid();
        return *this;
    }

    auto operator*() const {
        return *current_;
    }

private:
    void MoveToNextValid() {
        while (current_ != last_ && *current_ == std::nullopt) {
            ++current_;
        }
    }

    Iterator current_;
    Iterator last_;
};

template<typename Container>
class DropNullopt_view {
public:
    using const_iterator = DropNullopt_iterator<typename Container::const_iterator>;

    DropNullopt_view(const Container& container)
        : begin_iterator_(container.begin(), container.end()),
          end_iterator_(container.end(), container.end()) {}

    const_iterator begin() const {
        return begin_iterator_;
    }

    const_iterator end() const {
        return end_iterator_;
    }

private:
    const_iterator begin_iterator_;
    const_iterator end_iterator_;
};
class DropNullopt
{};