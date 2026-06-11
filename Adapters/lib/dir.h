#pragma once

#include <filesystem>

namespace fs = std::filesystem;

class Dir_iterator {
public:
    using value_type = fs::path;

    Dir_iterator() = default;

    Dir_iterator(const fs::path& path, bool recursive) : is_recursive_(recursive) {
        if (is_recursive_) {
            recursive_it_ = fs::recursive_directory_iterator(path);
        } else {
            flat_it_ = fs::directory_iterator(path);
        }
        advance_to_file();
    }

    bool operator==(const Dir_iterator& other) const {
        return is_recursive_ == other.is_recursive_ &&
               (is_recursive_ ? recursive_it_ == other.recursive_it_
                              : flat_it_ == other.flat_it_);
    }

    Dir_iterator& operator++() {
        if (is_recursive_) {
            ++recursive_it_;
        } else {
            ++flat_it_;
        }
        advance_to_file();
        return *this;
    }

    fs::path operator*() const {
        return is_recursive_ ? *recursive_it_ : *flat_it_;
    }

private:
    void advance_to_file() {
        if (is_recursive_) {
            while (recursive_it_ != fs::recursive_directory_iterator() &&
                   fs::is_directory(*recursive_it_)) {
                ++recursive_it_;
            }
        } else {
            while (flat_it_ != fs::directory_iterator() &&
                   fs::is_directory(*flat_it_)) {
                ++flat_it_;
            }
        }
    }

    bool is_recursive_ = false;
    fs::directory_iterator flat_it_;
    fs::recursive_directory_iterator recursive_it_;
};

class Dir {
public:
    using const_iterator = Dir_iterator;

    Dir(const fs::path& path, bool recursive) : path_(path), recursive_(recursive), begin_(path, recursive), end_() {}

    const_iterator begin() const {
        return begin_;
    }

    const_iterator end() const {
        return end_;
    }

private:
    fs::path path_;
    bool recursive_;
    const_iterator begin_;
    const_iterator end_;
};