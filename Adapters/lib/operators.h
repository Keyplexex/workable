#pragma once

#include "transform.h"
#include "filter.h"
#include "drop_nullopt.h"
#include "split.h"
#include "write.h"
#include "agregate_by_key.h"
#include "split_expected.h"
#include "open_files.h"
#include "join.h"

template <typename View, typename Stream>
auto operator|(const View& view, const Out<Stream>& out) {
    auto writer = out.GetWrite();
    for (const auto& val : view) {
        writer(val); 
    }
    return out;
}


template <ForwardContainer Container, Functor<Container> Func>
Transform_view<Container, Func> operator|(const Container& range, const Transform<Func>& func) {
    return { range, func.GetFunc() };
}

template <ForwardContainer Container, FunctorFilter<Container> Func>
Filter_view<Container, Func> operator|(const Container& range, const Filter<Func>& filter) {
    return { range, filter.GetFunc() };
}

template <ForwardContainerAndNullOpt Container>
DropNullopt_view<Container> operator|(const Container& range, const DropNullopt&) {
    return { range };
}

template <ForwardContainerAndIStream Container>
Split_view<Container> operator|(const Container& range, const Split& split) {
    return { range, split.GetDelimiters()};
}

template <ForwardContainer Container, typename Stream, typename Delimeter>
Container operator|(const Container& range, const Write<Stream, Delimeter>& write) {
    auto& stream   = write.GetStream();
    Delimeter del  = write.GetDelimeter();
    for (auto i : range)
        stream << i << del;

    return range;
}

template <ForwardContainer Container, typename Stream>
Container operator|(const Container& range, const Out<Stream>& out) {
    return range | out.GetWrite();
}

template <ForwardContainer Container>
decltype(auto) operator|(const Container& range, const AsVector&) {
    std::vector<std::remove_cvref_t<decltype(*range.begin())>> res;
    for (const auto& i : range)
        res.push_back(i);

    return res;
}

template <typename Container, typename T, typename Aggregator, typename KeyFunc>
requires AggregateByKeyRequirements<Container, T, Aggregator, KeyFunc>
AggregateByKey_view<Container, T, Aggregator, KeyFunc> operator|(const Container& range, const AggregateByKey<T, Aggregator, KeyFunc>& aggregate_by_key) {
    using KeyType   = decltype(std::declval<KeyFunc>()(std::declval<typename Container::const_iterator::value_type>()));
    using ValueType = typename std::remove_reference_t<T>;
    auto mp = std::make_unique<std::unordered_map<KeyType, ValueType>>();
    auto vec = std::make_unique<std::vector<KeyType>>();
    for (const auto& i : range) {
        auto temp = aggregate_by_key.GetKeyFunc()(i);
        if (mp->find(temp) == mp->end()) {
            vec->push_back(temp);
            (*mp)[temp] = aggregate_by_key.GetStartValue();
        }
        aggregate_by_key.GetAggregator()(i, (*mp)[temp]);
    }
    return { std::move(mp), std::move(vec) };
}

template <ForwardContainerAndExpected Container>
decltype(auto) operator|(const Container& range, const SplitExpected& ) {
    return std::make_pair( SplitExpected_view<Container, true>(range), SplitExpected_view<Container, false>(range));
}

template <typename Container>
OpenFiles_view<Container> operator|(const Container& range, const OpenFiles& ) {
    return { range };
}

template <typename LeftContainer, typename RightContainer, typename LeftFunc, typename RightFunc>
requires JoinRequirements<LeftContainer, RightContainer, LeftFunc, RightFunc>
Join_view<LeftContainer, RightContainer, LeftFunc, RightFunc> operator|(const LeftContainer& left_range, const Join<RightContainer, LeftFunc, RightFunc>& join) {
    using KeyRightFunc = decltype(std::declval<RightFunc>()(std::declval<typename RightContainer::const_iterator::value_type>()));
    using ValRightFunc = typename RightContainer::const_iterator::value_type;

    auto right_mp = std::make_unique<std::unordered_map<KeyRightFunc, ValRightFunc>>();
    const RightFunc& right_func = join.GetRightFunc();
    for (const auto& i : join.GetRightContainer()) {
        (*right_mp)[right_func(i)] = i;
    }
    return { left_range, std::move(right_mp), join.GetLeftFunc() };
}