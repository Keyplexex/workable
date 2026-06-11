#pragma once

#include <iterator>
#include <type_traits>

template<typename Iterator, typename Predicate>
class FilterIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::remove_cv_t<std::remove_reference_t<decltype(*std::declval<Iterator>())>>;
    using difference_type = std::ptrdiff_t;
    using pointer = std::add_pointer_t<value_type>;
    using reference = decltype(*std::declval<Iterator>());

    FilterIterator(Iterator current, Iterator end, Predicate predicate)
        : current_(current), end_(end), predicate_(std::move(predicate))
    {
        AdvanceToNextValid();
    }

    bool operator==(const FilterIterator& other) const {
        return current_ == other.current_;
    }

    bool operator!=(const FilterIterator& other) const {
        return !(*this == other);
    }

    FilterIterator& operator++() {
        ++current_;
        AdvanceToNextValid();
        return *this;
    }

    FilterIterator operator++(int) {
        FilterIterator temp = *this;
        ++(*this);
        return temp;
    }

    reference operator*() const {
        return *current_;
    }

    pointer operator->() const {
        return &(*current_);
    }

private:
    void AdvanceToNextValid() {
        while (current_ != end_ && !predicate_(*current_)) {
            ++current_;
        }
    }

    Iterator current_;
    Iterator end_;
    Predicate predicate_;
};

template<typename Container, typename Predicate>
class Filter_view {
public:
    using const_iterator = FilterIterator<typename Container::const_iterator, Predicate>;

    Filter_view(Container container, Predicate predicate)
        : container_(std::move(container)), predicate_(std::move(predicate)),
          begin_(container_.begin(), container_.end(), predicate_),
          end_(container_.end(), container_.end(), predicate_) {}

    const_iterator begin() const {
        return begin_;
    }

    const_iterator end() const {
        return end_;
    }

private:
    Container container_;
    Predicate predicate_;
    const_iterator begin_;
    const_iterator end_;
};

template<typename Func>
class Filter {
public:
    explicit Filter(Func func) : func_(std::move(func)) {}

    template<typename Collection>
    auto operator()(Collection collection) const {
        return Filter_view<Collection, Func>(std::move(collection), func_);
    }

    const Func& GetFunc() const {
        return func_;
    }
private:
    Func func_;
};
