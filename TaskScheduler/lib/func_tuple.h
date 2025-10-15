#pragma once
#include <utility>
#include <type_traits>

template<typename T>
struct remove_reference { using type = T; };

template<typename T>
struct remove_reference<T&> { using type = T; };

template<typename T>
struct remove_reference<T&&> { using type = T; };

template<typename T>
using remove_reference_t = typename remove_reference<T>::type;

template<typename T>
remove_reference_t<T>&& declval() noexcept;

template<typename T>
constexpr T&& forward(remove_reference_t<T>& t) noexcept {
    return static_cast<T&&>(t);
}

template<typename T>
constexpr T&& forward(remove_reference_t<T>&& t) noexcept {
    return static_cast<T&&>(t);
}

template <typename F, typename... Args>
auto callFunction(F&& func, Args&&... args) 
    -> decltype(forward<F>(func)(forward<Args>(args)...)) {
    return forward<F>(func)(forward<Args>(args)...);
}

template <typename R, typename C, typename... Args>
auto callFunction(R (C::*pmf)(Args...), C& obj, Args&&... args)
    -> decltype((obj.*pmf)(forward<Args>(args)...)) {
    return (obj.*pmf)(forward<Args>(args)...);
}

template <typename R, typename C, typename... Args>
auto callFunction(R (C::*pmf)(Args...) const, const C& obj, Args&&... args)
    -> decltype((obj.*pmf)(forward<Args>(args)...)) {
    return (obj.*pmf)(forward<Args>(args)...);
}

template <typename R, typename C, typename... Args>
auto callFunction(R (C::*pmf)(Args...), C* obj, Args&&... args)
    -> decltype((obj->*pmf)(forward<Args>(args)...)) {
    return (obj->*pmf)(forward<Args>(args)...);
}

template <typename R, typename C, typename... Args>
auto callFunction(R (C::*pmf)(Args...) const, const C* obj, Args&&... args)
    -> decltype((obj->*pmf)(forward<Args>(args)...)) {
    return (obj->*pmf)(forward<Args>(args)...);
}


template <typename T>
struct MyTupleElement {
    using StoredType = std::conditional_t<std::is_lvalue_reference_v<T>, T, T>;
    StoredType value;

    template <typename U>
    MyTupleElement(U&& v) : value(std::forward<U>(v)) {}
};


template <typename... Ts>
struct MyTuple; 


template <>
struct MyTuple<> {};


template <typename H, typename... Ts>
struct MyTuple<H, Ts...> {
    MyTupleElement<H> head;
    MyTuple<Ts...> tail;

    template <typename UH, typename... UTs>
    MyTuple(UH&& h, UTs&&... ts)
        : head(std::forward<UH>(h)), tail(std::forward<UTs>(ts)...) {}
};


template <size_t I, typename Tuple>
struct TupleElement;

template <typename H, typename... Ts>
struct TupleElement<0, MyTuple<H, Ts...>> {
    static auto& get(MyTuple<H, Ts...>& t) { return t.head.value; }
    static const auto& get(const MyTuple<H, Ts...>& t) { return t.head.value; }
};

template <size_t I, typename H, typename... Ts>
struct TupleElement<I, MyTuple<H, Ts...>> {
    static auto& get(MyTuple<H, Ts...>& t) {
        return TupleElement<I - 1, MyTuple<Ts...>>::get(t.tail);
    }
    static const auto& get(const MyTuple<H, Ts...>& t) {
        return TupleElement<I - 1, MyTuple<Ts...>>::get(t.tail);
    }
};

template <typename... Args>
MyTuple<Args...> make_my_tuple(Args&&... args) {
    return MyTuple<Args...>(std::forward<Args>(args)...);
}
