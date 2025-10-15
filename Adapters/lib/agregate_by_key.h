#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include "concepts.h"

template <typename Container, typename T, typename Aggregator, typename KeyFunc>
class AggregateByKey_iterator {
    using KeyType = decltype(std::declval<KeyFunc>()(std::declval<typename Container::const_iterator::value_type>()));
    using ValueType = std::remove_reference_t<T>;

public:
    using value_type = std::pair<KeyType, ValueType>;

    AggregateByKey_iterator(std::shared_ptr<std::unordered_map<KeyType, ValueType>> mp,
                            std::shared_ptr<std::vector<KeyType>> vec,
                            typename std::vector<KeyType>::const_iterator it) : mp_(mp), vec_(vec), it_(it) {}

    bool operator==(const AggregateByKey_iterator& other) const {
        return it_ == other.it_;
    }

    AggregateByKey_iterator& operator++() {
        ++it_;
        return *this;
    }

    auto operator*() const {
        return std::make_pair(*it_, mp_->at(*it_));
    }

private:
    std::shared_ptr<std::unordered_map<KeyType, ValueType>> mp_;
    std::shared_ptr<std::vector<KeyType>> vec_;
    typename std::vector<KeyType>::const_iterator it_;
};

template <typename Container, typename T, typename Aggregator, typename KeyFunc>
class AggregateByKey_view {
    using KeyType = decltype(std::declval<KeyFunc>()(std::declval<typename Container::const_iterator::value_type>()));
    using ValueType = std::remove_reference_t<T>;

public:
    using const_iterator = AggregateByKey_iterator<Container, T, Aggregator, KeyFunc>;

    AggregateByKey_view(std::shared_ptr<std::unordered_map<KeyType, ValueType>> mp,
                        std::shared_ptr<std::vector<KeyType>> vec)
        : mp_(mp), vec_(vec), begin_(mp, vec, vec->begin()), end_(mp, vec, vec->end()) {}

    const_iterator begin() const {
        return begin_;
    }

    const_iterator end() const {
        return end_;
    }

private:
    std::shared_ptr<std::unordered_map<KeyType, ValueType>> mp_;
    std::shared_ptr<std::vector<KeyType>> vec_;
    const_iterator begin_;
    const_iterator end_;
};

template <typename T, typename Aggregator, typename KeyFunc>
class AggregateByKey {
public:
    AggregateByKey(const T& start_value, const Aggregator& aggregator, const KeyFunc& key_func)
        : start_value_(start_value), aggregator_(aggregator), key_func_(key_func) {}

    T GetStartValue() const {
        return start_value_;
    }

    const Aggregator& GetAggregator() const {
        return aggregator_;
    }

    const KeyFunc& GetKeyFunc() const {
        return key_func_;
    }

private:
    T start_value_;
    Aggregator aggregator_;
    KeyFunc key_func_;
};