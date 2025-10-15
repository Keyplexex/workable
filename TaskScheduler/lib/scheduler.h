#pragma once
#include <memory>
#include <vector>
#include <queue>
#include <type_traits>

#include "func_tuple.h" 

class TTaskScheduler;

template <typename T>
struct Future {
    size_t id;
};

template <typename T>
struct unwrap {
    using type = T;
};

template <typename U>
struct unwrap<Future<U>> {
    using type = U;
};

template <typename V, typename S>
V transformArg(const V& v, S*) { return v; }

template <typename U, typename S>
U transformArg(const Future<U>& f, S* s) {
    return s->template getResult<U>(f.id);
}

template <typename T>
struct remove_cv { using type = T; };

template <typename T> struct remove_cv<const T> { using type = T; };
template <typename T> struct remove_cv<volatile T> { using type = T; };
template <typename T> struct remove_cv<const volatile T> { using type = T; };

template <typename T>
using remove_cv_t = typename remove_cv<T>::type;

template <typename T>
struct decay {
    using type = remove_cv_t<remove_reference_t<T>>;
};

template <typename T>
using decay_t = typename decay<T>::type;

template <typename T>
remove_reference_t<T>&& declval() noexcept;

template <size_t... Is>
struct index_sequence {};

template <size_t N, size_t... Is>
struct make_index_sequence_impl : make_index_sequence_impl<N - 1, N - 1, Is...> {};

template <size_t... Is>
struct make_index_sequence_impl<0, Is...> {
    using type = index_sequence<Is...>;
};

template <size_t N>
using make_index_sequence = typename make_index_sequence_impl<N>::type;

namespace my {
    template <typename T>
    remove_reference_t<T>&& move(T&& v) noexcept {
        return static_cast<remove_reference_t<T>&&>(v);
    }
} 

template <typename Func, typename Tuple, size_t... Is>
auto apply_impl(Func&& func, Tuple& tup, TTaskScheduler* sched, index_sequence<Is...>) {
    return callFunction(forward<Func>(func),
                        transformArg(TupleElement<Is, Tuple>::get(tup), sched)...);
}

template <typename Func, typename... Ts>
auto apply(Func&& func, MyTuple<Ts...>& tup, TTaskScheduler* sched) {
    return apply_impl(forward<Func>(func),
                      tup,
                      sched,
                      make_index_sequence<sizeof...(Ts)>{});
}

struct BaseTask {
    virtual ~BaseTask() = default;
    virtual void execute() = 0;
};

template <typename R>
struct BaseTaskResult {
    virtual ~BaseTaskResult() = default;
    virtual R get() = 0;
};

template <>
struct BaseTaskResult<void> {
    virtual ~BaseTaskResult() = default;
    virtual void get() = 0;
};


template <typename R, typename Bound>
struct Task : BaseTask, BaseTaskResult<R> {
    Bound bound;
    R result;
    bool have_result = false;

    Task(Bound&& bound_) : bound(forward<Bound>(bound_)) {}

    void execute() override {
        if (!have_result) {
            if constexpr (std::is_move_assignable_v<R>) {
                result = bound();
            } else {
                result.~R();
                new (&result) R(bound());
            }
            have_result = true;
        }
    }

    R get() override { 
        if constexpr (std::is_move_constructible_v<R>) {
            return std::move(result);
        } else {
            return result;
        }
    }
};

template <typename Bound>
struct Task<void, Bound> : BaseTask, BaseTaskResult<void> {
    Bound bound;
    bool executed = false;

    Task(Bound&& bound_) : bound(forward<Bound>(bound_)) {}

    void execute() override {
        if (!executed) {
            bound();
            executed = true;
        }
    }

    void get() override {}
};

using TaskPtr = std::unique_ptr<BaseTask>;

template <typename R, typename C, typename... ArgsF, typename Obj, typename... CallArgs>
auto my_invoke(R (C::*pmf)(ArgsF...), Obj&& obj, CallArgs&&... args)
    -> decltype((forward<Obj>(obj).*pmf)(forward<CallArgs>(args)...)) {
    return (forward<Obj>(obj).*pmf)(forward<CallArgs>(args)...);
}

template <typename R, typename C, typename... ArgsF, typename Obj, typename... CallArgs>
auto my_invoke(R (C::*pmf)(ArgsF...) const, Obj&& obj, CallArgs&&... args)
    -> decltype((forward<Obj>(obj).*pmf)(forward<CallArgs>(args)...)) {
    return (forward<Obj>(obj).*pmf)(forward<CallArgs>(args)...);
}

template <typename R, typename C, typename... ArgsF, typename Ptr, typename... CallArgs>
auto my_invoke(R (C::*pmf)(ArgsF...), Ptr&& ptr, CallArgs&&... args)
    -> decltype((forward<Ptr>(ptr)->*pmf)(forward<CallArgs>(args)...)) {
    return (forward<Ptr>(ptr)->*pmf)(forward<CallArgs>(args)...);
}

template <typename R, typename C, typename... ArgsF, typename Ptr, typename... CallArgs>
auto my_invoke(R (C::*pmf)(ArgsF...) const, Ptr&& ptr, CallArgs&&... args)
    -> decltype((forward<Ptr>(ptr)->*pmf)(forward<CallArgs>(args)...)) {
    return (forward<Ptr>(ptr)->*pmf)(forward<CallArgs>(args)...);
}

template <typename F, typename... CallArgs>
auto my_invoke(F&& f, CallArgs&&... args)
    -> decltype(forward<F>(f)(forward<CallArgs>(args)...)) {
    return forward<F>(f)(forward<CallArgs>(args)...);
}

template <typename F, typename... Args>
using invoke_result_t = decltype(my_invoke(declval<F>(), declval<Args>()...));


class TTaskScheduler {
   private:
    std::vector<TaskPtr> tasks_;
    std::vector<std::vector<size_t>> deps_;

    void extractDepsRecursive(std::vector<size_t>&) {}

    template <typename U, typename... Rest>
    void extractDepsRecursive(std::vector<size_t>& vec, const Future<U>& fut, Rest&&... rest) {
        vec.push_back(fut.id);
        extractDepsRecursive(vec, forward<Rest>(rest)...);
    }

    template <typename V, typename... Rest>
    void extractDepsRecursive(std::vector<size_t>& vec, const V&, Rest&&... rest) {
        extractDepsRecursive(vec, forward<Rest>(rest)...);
    }

    template <typename... Rest>
    std::vector<size_t> extractDeps(Rest&&... rest) {
        std::vector<size_t> vec;
        extractDepsRecursive(vec, forward<Rest>(rest)...);
        return vec;
    }

   public:
    template <typename F, typename... Args>
    size_t add(F&& func, Args&&... args) {
        size_t id = tasks_.size();

        using R = invoke_result_t<F, typename unwrap<decay_t<Args>>::type...>;

        auto tup = make_my_tuple(std::forward<Args>(args)...); 

        auto bound = [this, func = std::forward<F>(func), tup = my::move(tup)]() mutable {
            return apply(func, tup, this);
        };

        if constexpr (std::is_same_v<R, void>) {
            tasks_.push_back(std::make_unique<Task<void, decltype(bound)>>(std::move(bound)));
        } else {
            tasks_.push_back(std::make_unique<Task<R, decltype(bound)>>(std::move(bound)));
        }

        auto dependencies = extractDeps(args...);
        deps_.push_back(std::move(dependencies));

        return id;
    }

    template <typename T>
    Future<T> getFutureResult(size_t id) {
        return Future<T>{id};
    }

    template <typename R>
    R getResult(size_t id) {
        tasks_[id]->execute();
        if constexpr (std::is_same_v<R, void>) {
            auto* res = dynamic_cast<BaseTaskResult<void>*>(tasks_[id].get());
            res->get();
        } else {
            auto* res = dynamic_cast<BaseTaskResult<R>*>(tasks_[id].get());
            return res->get();
        }
    }

    bool hasCycle() const {
        size_t n = deps_.size();
        std::vector<int> inDeg(n);
        std::vector<std::vector<size_t>> adj(n);
        for (size_t u = 0; u < n; ++u) {
            for (auto v : deps_[u]) {
                if (v >= n) {
                    return true;
                }
                adj[v].push_back(u);
                ++inDeg[u];
            }
        }
        std::queue<size_t> q;
        for (size_t i = 0; i < n; ++i)
            if (inDeg[i] == 0) q.push(i);
        size_t cnt = 0;
        while (!q.empty()) {
            size_t u = q.front();
            q.pop();
            ++cnt;
            for (auto v : adj[u]) {
                if (--inDeg[v] == 0)
                    q.push(v);
            }
        }
        return cnt != n;
    }

    void executeAll() {
        if (hasCycle()) {
            throw "Cycle detected in task dependencies";
        }
        for (const auto& task : tasks_) {
            task->execute();
        }
    }
};