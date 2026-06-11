#pragma once

#include <fstream>
#include <memory>
#include "concepts.h" 

template <typename Iterator>
class OpenFiles_iterator {
public:
    using value_type = std::ifstream&;
    using iterator_category = std::input_iterator_tag; //это для меня

    OpenFiles_iterator() = default;

    OpenFiles_iterator(Iterator current, Iterator end) : current_(current), end_(end) {
        NextFile();
    }

    bool operator==(const OpenFiles_iterator& other) const {
        return current_ == other.current_;
    }

    OpenFiles_iterator& operator++() {
        ++current_;
        NextFile();
        return *this;
    }

    std::ifstream& operator*() const {
        return *current_stream_;
    }

private:
    void NextFile() {
        while (current_ != end_) {
            auto stream = std::make_shared<std::ifstream>(*current_);
            if (stream->is_open()) {
                current_stream_ = std::move(stream);
                return;
            }
            ++current_;
        }
        current_stream_.reset();
    }

    Iterator current_;
    Iterator end_;
    std::shared_ptr<std::ifstream> current_stream_;
};

template <typename Container>
class OpenFiles_view {
public:
    using const_iterator = OpenFiles_iterator<typename Container::const_iterator>;

    OpenFiles_view(const Container& range) : begin_(range.begin(), range.end()), end_(range.end(), range.end()) {}

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

class OpenFiles {};