#pragma once

#include <iterator>
#include <optional>
#include "headers.h"

template<typename LeftView, typename RightView>
class MergeCustomView;

template<typename RightView>
class MergeCustomOp {
public:
    explicit MergeCustomOp(RightView right) : right_(right) {}

    template<typename LeftView>
    auto operator()(LeftView left) const {
        return MergeCustomView<LeftView, RightView>(left, right_);
    }

private:
    RightView right_;
};

template<typename LeftView, typename RightView>
class MergeCustomView {
public:
    using value_type = KV<int, std::optional<int>>;

    class Iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        using LeftIter = decltype(std::declval<LeftView>().begin());
        using RightIter = decltype(std::declval<RightView>().begin());

        Iterator(LeftIter left_it, LeftIter left_end,
                 RightIter right_it, RightIter right_end)
            : left_it_(left_it), left_end_(left_end),
              right_it_(right_it), right_end_(right_end) {}

        value_type operator*() const {
            bool has_left = left_it_ != left_end_;
            bool has_right = right_it_ != right_end_;

            if (has_left && has_right) {
                int left_val = *left_it_;
                int right_val = *right_it_;
                return {left_val + right_val, left_val * right_val};
            }
            else if (has_left) {
                int left_val = *left_it_;
                return {left_val, std::nullopt};
            }
            else if (has_right) {
                int right_val = *right_it_;
                return {right_val, std::nullopt};
            }
            return {0, std::nullopt};
        }

        Iterator& operator++() {
            if (left_it_ != left_end_) ++left_it_;
            if (right_it_ != right_end_) ++right_it_;
            return *this;
        }

        bool operator!=(const Iterator& other) const {
            return left_it_ != other.left_it_ || right_it_ != other.right_it_;
        }

    private:
        LeftIter left_it_;
        LeftIter left_end_;
        RightIter right_it_;
        RightIter right_end_;
    };

    MergeCustomView(LeftView left, RightView right)
        : left_view_(left), right_view_(right) {}

    Iterator begin() const {
        return Iterator(left_view_.begin(), left_view_.end(),
                        right_view_.begin(), right_view_.end());
    }

    Iterator end() const {
        return Iterator(left_view_.end(), left_view_.end(),
                        right_view_.end(), right_view_.end());
    }

private:
    LeftView left_view_;
    RightView right_view_;
};

template<typename LeftView, typename RightView>
auto operator|(LeftView left, MergeCustomOp<RightView> op) {
    return op(left);
}

template<typename RightView>
auto MergeCustom(RightView right) {
    return MergeCustomOp<RightView>(right);
}
