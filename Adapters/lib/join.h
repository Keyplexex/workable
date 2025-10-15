
#pragma once

#include <optional>
#include <unordered_map>
#include <memory>
#include "concepts.h" 

template <typename Key, typename Value>
struct KV {
    using key_type   = Key;
    using value_type = Value;

    Key key;
    Value value;

    friend std::ostream& operator<<(std::ostream& os, const KV& kv) {
        os << "{" << kv.key << ": ";
        if (kv.value) os << *kv.value; else os << "null";
        return os << "}";
    }    

    bool operator==(const KV& other) const = default;
};

template <typename>
struct is_kv : std::false_type {};

template <typename T, typename U>
struct is_kv<KV<T, U>> : std::true_type {};

template <typename T>
constexpr bool is_kv_v = is_kv<T>::value;

template <typename Base, typename Joined>
struct JoinResult {
    Base base;
    std::optional<Joined> joined;

    bool operator==(const JoinResult& other) const = default;
};

template <typename Iterator, typename LeftContainer, typename RightContainer, typename LeftFunc, typename RightFunc, bool IsKV>
class Join_iterator {
    using KeyRightFunc   = decltype(std::declval<RightFunc>()(std::declval<typename RightContainer::const_iterator::value_type>()));
    using ValRightFunc   = typename RightContainer::const_iterator::value_type;
    using LeftValueType  = typename LeftContainer::const_iterator::value_type;
    using RightValueType = typename RightContainer::const_iterator::value_type;

public:
    using value_type = JoinResult<LeftValueType, RightValueType>;

    Join_iterator(Iterator it, std::unique_ptr<std::unordered_map<KeyRightFunc, ValRightFunc>> mp, const LeftFunc& left_func)
    : it_(it)
    , right_mp_(std::move(mp))
    , left_func_(left_func)
    {}

    bool operator==(const Join_iterator& other) const {
        return it_ == other.it_;
    }

    Join_iterator operator++() {
        ++it_;
        return *this;
    }

    auto operator*() const {
        value_type res;
        res.base = *it_;
        if (right_mp_->find(left_func_(*it_)) == right_mp_->end()) {
            res.joined = std::nullopt;
        } else {
            res.joined = (*right_mp_)[left_func_(*it_)];
        }
        return res;
    }
private:
    Iterator it_;
    std::shared_ptr<std::unordered_map<KeyRightFunc, ValRightFunc>> right_mp_;
    const LeftFunc& left_func_;
};

template <typename Iterator, typename LeftContainer, typename RightContainer, typename LeftFunc, typename RightFunc>
class Join_iterator<Iterator, LeftContainer, RightContainer, LeftFunc, RightFunc, true> {
    using KeyRightFunc   = decltype(std::declval<RightFunc>()(std::declval<typename RightContainer::const_iterator::value_type>()));
    using ValRightFunc   = typename RightContainer::const_iterator::value_type;
    using LeftValueType  = typename LeftContainer::const_iterator::value_type;
    using RightValueType = typename RightContainer::const_iterator::value_type;

public:
    using value_type = JoinResult<typename LeftValueType::value_type, typename RightValueType::value_type>;

    Join_iterator(Iterator it, std::unique_ptr<std::unordered_map<KeyRightFunc, ValRightFunc>> mp, const LeftFunc& left_func)
    : it_(it)
    , right_mp_(std::move(mp))
    , left_func_(left_func)
    {}

    bool operator==(const Join_iterator& other) const {
        return it_ == other.it_;
    }

    Join_iterator operator++() {
        ++it_;
        return *this;
    }

    auto operator*() const {
        value_type res;
        res.base = (*it_).value;
        if (right_mp_->find(left_func_(*it_)) == right_mp_->end()) {
            res.joined = std::nullopt;
        } else {
            res.joined = (*right_mp_)[left_func_(*it_)].value;
        }
        return res;
    }
private:
    Iterator it_;
    std::shared_ptr<std::unordered_map<KeyRightFunc, ValRightFunc>> right_mp_;
    const LeftFunc& left_func_;
};


template <typename LeftContainer, typename RightContainer, typename LeftFunc, typename RightFunc>
class Join_view {
    using KeyRightFunc = decltype(std::declval<RightFunc>()(std::declval<typename RightContainer::const_iterator::value_type>()));
    using ValRightFunc = typename RightContainer::const_iterator::value_type;
public:
    using const_iterator = Join_iterator<typename LeftContainer::const_iterator, LeftContainer, RightContainer, LeftFunc, RightFunc, is_kv_v<typename LeftContainer::const_iterator::value_type>>;

    Join_view(const LeftContainer& left_range, std::unordered_map<KeyRightFunc, ValRightFunc> mp, const LeftFunc& left_func_)
    : begin_(left_range.begin(), std::move(mp), left_func_)
    , end_(left_range.end(), nullptr, left_func_)
    {}

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

static auto left_function = [](const auto& kv) { return kv.key; };
static auto right_function = [](const auto& kv) { return kv.key; };

template <typename RightContainer,
          typename LeftFunc = decltype(left_function),
          typename RightFunc = decltype(right_function)>
class Join {
public:
    Join(const RightContainer& right_range)
        : right_range_(right_range)
        , left_func_(left_function)
        , right_func_(right_function)
    {}

    Join(const RightContainer& right_range, const LeftFunc& left_func, const RightFunc& right_func)
        : right_range_(right_range)
        , left_func_(left_func)
        , right_func_(right_func)
    {}

    RightContainer GetRightContainer() const {
        return right_range_;
    }

    const LeftFunc& GetLeftFunc() const {
        return left_func_;
    }

    const RightFunc& GetRightFunc() const {
        return right_func_;
    }
private:
    RightContainer right_range_;
    const LeftFunc& left_func_;
    const RightFunc& right_func_;
};
