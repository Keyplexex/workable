#pragma once

#include <expected>
#include "concepts.h"

template <typename Iterator, bool WithError>
class SplitExpected_iterator {
public:
    using value_type = std::conditional_t<WithError, typename Iterator::value_type::error_type, typename Iterator::value_type::value_type>;

    SplitExpected_iterator(Iterator it, Iterator end) : it_(it), end_(end) {
        Next();
    }

    bool operator==(const SplitExpected_iterator& other) const {
        return it_ == other.it_;
    }

    SplitExpected_iterator operator++() {
        ++it_;
        Next();
        return *this;
    }

    auto operator*() const {
        if constexpr (WithError) {
            return (*it_).error();
        } else {
            return *it_;
        }
    }
private:
    void Next() {
        if (WithError) {
            while (it_ != end_ && *it_)
                ++it_;
        } else {
            while (it_ != end_ && !(*it_))
                ++it_;
        }
    }
    Iterator it_;
    Iterator end_;
};

template <typename Container, bool WithError>
class SplitExpected_view {
public:
    using const_iterator = SplitExpected_iterator<typename Container::const_iterator, WithError>;

    SplitExpected_view(const Container& range) : begin_(range.begin(), range.end()), end_(range.end(), range.end()) {}

    const_iterator begin() const {
        return begin_;
    }

    const_iterator end() const {
        return end_;
    }
private:
    const_iterator begin_;
    const_iterator end_;
};

class SplitExpected
{};