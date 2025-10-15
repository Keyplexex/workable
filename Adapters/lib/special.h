#pragma once

#include <iterator>
#include <memory>
#include <vector>
#include <type_traits>
#include <optional>
#include <utility>
#include <iostream>

template<typename K, typename V>
struct KV {

    K key;
    V value;
    
    friend std::ostream& operator<<(std::ostream& os, const KV& kv) {
        os << "{" << kv.key << ": ";
        if (kv.value) os << *kv.value; else os << "null";
        return os << "}";
    }

    bool operator==(const KV& other) const = default;
};

template<typename Container>
class AsDataFlow;

template<typename Container>
auto MakeAsDataFlow(const Container& container);

template<typename Container>
class AsDataFlow {
public:
    using const_iterator = typename Container::const_iterator;
    using value_type = typename Container::value_type;
    using container_type = Container;

    explicit AsDataFlow(const Container& data)
        : data_(std::make_shared<Container>(data)),
          start_(data_->begin()),
          finish_(data_->end()) {}

    explicit AsDataFlow(Container&& data)
        : data_(std::make_shared<Container>(std::move(data))),
          start_(data_->begin()),
          finish_(data_->end()) {}

    const_iterator begin() const { return start_; }
    const_iterator end() const { return finish_; }

    const Container& get() const { return *data_; }

private:
    std::shared_ptr<const Container> data_;
    const_iterator start_;
    const_iterator finish_;
};

template<typename Container>
auto MakeAsDataFlow(const Container& c) {
    return AsDataFlow<Container>(c);
}

template<typename F>
class TransformOp {
public:
    explicit TransformOp(F func) : func_(func) {}
    
    template<typename Container>
    auto operator()(const Container& container) const {
        using InputType = typename Container::value_type;
        using OutputType = decltype(func_(std::declval<InputType>()));
        
        std::vector<OutputType> result;
        result.reserve(std::distance(container.begin(), container.end()));
        for (const auto& item : container) {
            result.push_back(func_(item));
        }
        return MakeAsDataFlow(result);
    }
    
private:
    F func_;
};

template<typename F>
class FilterOp {
public:
    explicit FilterOp(F predicate) : predicate_(predicate) {}
    
    template<typename Container>
    auto operator()(const Container& container) const {
        using ValueType = typename Container::value_type;
        
        std::vector<ValueType> result;
        for (const auto& item : container) {
            if (predicate_(item)) {
                result.push_back(item);
            }
        }
        return MakeAsDataFlow(result);
    }
    
private:
    F predicate_;
};

template<typename LeftContainer, typename RightContainer>
class MergeCustomView {
public:
    using value_type = KV<int, std::optional<int>>;

    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        Iterator(std::shared_ptr<LeftContainer> left_data,
                std::shared_ptr<RightContainer> right_data,
                typename LeftContainer::const_iterator left_it,
                typename RightContainer::const_iterator right_it)
            : left_data_(left_data), right_data_(right_data),
              left_it_(left_it), right_it_(right_it) {}

        value_type operator*() const {
            const bool has_left = left_it_ != left_data_->end();
            const bool has_right = right_it_ != right_data_->end();

            if (has_left && has_right) {
                return {*left_it_ + *right_it_, *left_it_ * *right_it_};
            }
            else if (has_left) {
                return {*left_it_, std::nullopt};
            }
            else if (has_right) {
                return {*right_it_, std::nullopt};
            }
            return {0, std::nullopt};
        }

        Iterator& operator++() {
            if (left_it_ != left_data_->end()) ++left_it_;
            if (right_it_ != right_data_->end()) ++right_it_;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return left_it_ != other.left_it_ || right_it_ != other.right_it_;
        }

    private:
        std::shared_ptr<LeftContainer> left_data_;
        std::shared_ptr<RightContainer> right_data_;
        typename LeftContainer::const_iterator left_it_;
        typename RightContainer::const_iterator right_it_;
    };

    MergeCustomView(const LeftContainer& left, const RightContainer& right)
        : left_data_(std::make_shared<LeftContainer>(left)),
          right_data_(std::make_shared<RightContainer>(right)) {}

    Iterator begin() const {
        return Iterator(left_data_, right_data_, 
                       left_data_->begin(), right_data_->begin());
    }

    Iterator end() const {
        return Iterator(left_data_, right_data_,
                       left_data_->end(), right_data_->end());
    }

private:
    std::shared_ptr<LeftContainer> left_data_;
    std::shared_ptr<RightContainer> right_data_;
};

template<typename RightContainer>
class MergeCustomOp {
public:
    explicit MergeCustomOp(const RightContainer& right) : right_(right) {}
    
    template<typename LeftContainer>
    auto operator()(const LeftContainer& left) const {
        return MergeCustomView<LeftContainer, RightContainer>(left, right_);
    }
    
private:
    const RightContainer& right_;
};

class OutOp {
public:
    explicit OutOp(std::ostream& os = std::cout) : os_(os) {}
    
    template<typename Container>
    void operator()(const Container& container) const {
        for (const auto& item : container) {
            os_ << item << " ";
        }
        os_ << std::endl;
    }
    
private:
    std::ostream& os_;
};

template<typename Container>
auto CreateDataFlow(const Container& container) {
    return MakeAsDataFlow(container);
}

template<typename F>
auto Transform(F func) {
    return TransformOp<F>(func);
}

template<typename F>
auto Filter(F predicate) {
    return FilterOp<F>(predicate);
}

template<typename RightContainer>
auto MergeCustom(const RightContainer& right) {
    return MergeCustomOp<RightContainer>(right);
}

inline auto Out(std::ostream& os = std::cout) {
    return OutOp(os);
}

template<typename Left, typename Right>
auto operator|(Left&& left, Right&& right) {
    return right(std::forward<Left>(left));
}