#pragma once

#include <type_traits>
#include <expected>
#include <optional>
#include <istream>

template<typename T>
concept ForwardContainer = requires(T t) {
    t.begin();
    t.end();
    ++t.begin();
    *t.begin();
    t.begin() == t.end();
    { t.begin() } -> std::same_as<decltype(t.end())>;
};

template <typename F, typename T>
concept Functor = requires(T t, F f){
    f(*t.begin());
};

template <typename F, typename T>
concept FunctorFilter = requires(T t, F f){
    { f(*t.begin()) } -> std::same_as<bool>;
} && Functor<F, T>;

template <typename T>
struct is_optional : std::false_type
{};

template <typename T>
struct is_optional<std::optional<T>> : std::true_type
{};

template <typename T>
constexpr bool is_optional_v = is_optional<T>::value;

template<typename T>
concept ForwardContainerAndNullOpt = requires(T t) {
    requires is_optional_v<typename T::const_iterator::value_type>;
} && ForwardContainer<T>;

template<typename T>
concept ForwardContainerAndIStream = requires {
    std::derived_from<typename T::const_iterator::value_type, std::istream>;
} && ForwardContainer<T>;

template <typename T>
struct is_expected : std::false_type
{};

template <typename T, typename E>
struct is_expected<std::expected<T, E>> : std::true_type
{};

template<typename T>
concept ForwardContainerAndExpected = requires(T t) {
    requires is_expected<typename T::const_iterator::value_type>::value;
} && ForwardContainer<T>;

template <typename LeftContainer, typename RightContainer, typename LeftFunc, typename RightFunc>
concept JoinRequirements =
ForwardContainer<LeftContainer> &&
ForwardContainer<RightContainer> &&
requires(const LeftContainer& left_range, const LeftFunc& left_func) {
    { left_func(*left_range.begin()) };
} &&
requires(const RightContainer& right_range, const RightFunc& right_func) {
    { right_func(*right_range.begin()) };
} &&
requires(const LeftContainer& left_range, const LeftFunc& left_func, const RightContainer& right_range, const RightFunc& right_func) {
    { left_func(*left_range.begin()) == right_func(*right_range.begin()) } -> std::convertible_to<bool>;
} &&
requires(const RightContainer& right_range, const RightFunc& right_func) {
    { std::hash<decltype(right_func(*right_range.begin()))>{}(right_func(*right_range.begin())) }  -> std::convertible_to<std::size_t>;
};


template<typename Container, typename T, typename Aggregator, typename KeyFunc>
concept AggregateByKeyRequirements =
ForwardContainer<Container> &&
requires(KeyFunc key_func, const Container::const_iterator::value_type& range_obj) {
    key_func(range_obj);
} &&
requires(Aggregator aggregator, const Container::const_iterator::value_type& range_obj, T& t) {
    { aggregator(range_obj, t) } -> std::same_as<void>;
} &&
requires (decltype(std::declval<KeyFunc>()(std::declval<typename Container::const_iterator::value_type>())) key) {
    { std::hash<decltype(key)>{}(key) } -> std::convertible_to<std::size_t>;
};