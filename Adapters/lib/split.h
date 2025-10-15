#pragma once

#include <string>
#include <algorithm>
#include <istream>
#include <iterator>
#include "concepts.h"

template <typename Iterator>
class Split_iterator {
public:
    using value_type = std::string;

    Split_iterator(Iterator current, Iterator end, const std::string& delimiters)
        : current_(current), end_(end), delimiters_(delimiters), index_(0) {
        if (current_ != end) {
            const_cast<std::remove_cvref_t<decltype(*current_)>&>(*current_).seekg(0);
            std::istreambuf_iterator<char> begin((*current_).rdbuf()), end;
            buffer_.assign(begin, end);
            NextToken();
        }
    }

    bool operator==(const Split_iterator& other) const {
        return current_ == other.current_;
    }

    Split_iterator& operator++() {
        NextToken();
        return *this;
    }

    std::string operator*() const {
        return token_;
    }

private:
    void NextToken() {
        if (index_ >= buffer_.size()) {
            ++current_;
            if (current_ == end_) {
                return;
            } else {
                index_ = 0;
                std::istreambuf_iterator<char> begin((*current_).rdbuf()), end;
                buffer_.assign(begin, end);
            }
        }

        token_.clear();
        while (index_ < buffer_.size()) {
            if (std::find(delimiters_.begin(), delimiters_.end(), buffer_[index_]) != delimiters_.end()) {
                ++index_;
                break;
            }
            token_ += buffer_[index_];
            ++index_;
        }
    }

    Iterator current_;
    Iterator end_;
    std::string delimiters_;
    std::string token_;
    std::string buffer_;
    size_t index_;
};

template <typename Container>
class Split_view {
public:
    using const_iterator = Split_iterator<typename Container::const_iterator>;

    Split_view(const Container& range, const std::string& delimiters)
        : begin_iterator_(range.begin(), range.end(), delimiters),
          end_iterator_(range.end(), range.end(), delimiters) {}

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

class Split {
public:
    Split(const std::string& delimiters) : delimiters_(delimiters) {}

    std::string GetDelimiters() const {
        return delimiters_;
    }
private:
    std::string delimiters_;
};