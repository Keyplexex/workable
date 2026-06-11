#pragma once

#include <iterator>
#include <utility>
template<typename Iter, typename Transformation>
class TransformIterator {
public:
    using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
    using value_type = decltype(std::declval<Transformation>()(*std::declval<Iter>()));
    using difference_type = typename std::iterator_traits<Iter>::difference_type;
    using pointer = void;
    using reference = value_type;

    TransformIterator(Iter iterator, Transformation transform)
        : iterator_(iterator), transform_(std::move(transform)) {}

    bool operator==(const TransformIterator& other) const {
        return iterator_ == other.iterator_;
    }

    bool operator!=(const TransformIterator& other) const {
        return !(*this == other);
    }

    TransformIterator& operator++() {
        ++iterator_;
        return *this;
    }

    auto operator*() const {
        return transform_(*iterator_);
    }

private:
    Iter iterator_;
    Transformation transform_;
};

template<typename Collection, typename Transformation>
class Transform_view {
public:
    using const_iterator = TransformIterator<typename Collection::const_iterator, Transformation>;

    Transform_view(Collection collection, Transformation transform)
        : collection_(std::move(collection)), transform_(std::move(transform)) {}

    const_iterator begin() const {
        return const_iterator(collection_.begin(), transform_);
    }

    const_iterator end() const {
        return const_iterator(collection_.end(), transform_);
    }

private:
    Collection collection_; 
    Transformation transform_;
};

template<typename Func>
class Transform {
public:
    explicit Transform(Func func) : func_(std::move(func)) {}

    template<typename Collection>
    auto operator()(Collection collection) const {
        return Transform_view<Collection, Func>(std::move(collection), func_);
    }

    const Func& GetFunc() const {
        return func_;
    }
private:
    Func func_;
};
