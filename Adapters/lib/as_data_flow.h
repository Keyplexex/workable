#pragma once

#include "concepts.h" 

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

template<typename Container>
auto CreateDataFlow(const Container& container) {
    return MakeAsDataFlow(container);
}


class AsVector
{};