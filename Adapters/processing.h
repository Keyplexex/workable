#pragma once

#include <iostream> 
#include <filesystem>
#include <fstream>
#include <optional>
#include <type_traits>
#include <memory>
#include <string>
#include <utility>
#include <type_traits>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <functional>
#include <expected>

namespace fs = std::filesystem;

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


class Dir_iterator {
public:
    using value_type = fs::path;

    Dir_iterator() = default;

    Dir_iterator(const fs::path& path, bool recursive) : is_recursive_(recursive) {
        if (is_recursive_) {
            recursive_it_ = fs::recursive_directory_iterator(path);
        } else {
            flat_it_ = fs::directory_iterator(path);
        }
        advance_to_file();
    }

    bool operator==(const Dir_iterator& other) const {
        return is_recursive_ == other.is_recursive_ &&
               (is_recursive_ ? recursive_it_ == other.recursive_it_
                              : flat_it_ == other.flat_it_);
    }

    Dir_iterator& operator++() {
        if (is_recursive_) {
            ++recursive_it_;
        } else {
            ++flat_it_;
        }
        advance_to_file();
        return *this;
    }

    fs::path operator*() const {
        return is_recursive_ ? *recursive_it_ : *flat_it_;
    }

private:
    void advance_to_file() {
        if (is_recursive_) {
            while (recursive_it_ != fs::recursive_directory_iterator() &&
                   fs::is_directory(*recursive_it_)) {
                ++recursive_it_;
            }
        } else {
            while (flat_it_ != fs::directory_iterator() &&
                   fs::is_directory(*flat_it_)) {
                ++flat_it_;
            }
        }
    }

    bool is_recursive_ = false;
    fs::directory_iterator flat_it_;
    fs::recursive_directory_iterator recursive_it_;
};

class Dir {
public:
    using const_iterator = Dir_iterator;

    Dir(const fs::path& path, bool recursive) : path_(path), recursive_(recursive), begin_(path, recursive), end_() {}

    const_iterator begin() const {
        return begin_;
    }

    const_iterator end() const {
        return end_;
    }

private:
    fs::path path_;
    bool recursive_;
    const_iterator begin_;
    const_iterator end_;
};

template <typename Iterator>
class OpenFiles_iterator {
public:
    using value_type = std::ifstream&;
    using iterator_category = std::input_iterator_tag; //это для меня

    OpenFiles_iterator() = default;

    OpenFiles_iterator(Iterator current, Iterator end) : current_(current), end_(end) {
        NextFile();
    }

    bool operator==(const OpenFiles_iterator& other) const {
        return current_ == other.current_;
    }

    OpenFiles_iterator& operator++() {
        ++current_;
        NextFile();
        return *this;
    }

    std::ifstream& operator*() const {
        return *current_stream_;
    }

private:
    void NextFile() {
        while (current_ != end_) {
            auto stream = std::make_shared<std::ifstream>(*current_);
            if (stream->is_open()) {
                current_stream_ = std::move(stream);
                return;
            }
            ++current_;
        }
        current_stream_.reset();
    }

    Iterator current_;
    Iterator end_;
    std::shared_ptr<std::ifstream> current_stream_;
};

template <typename Container>
class OpenFiles_view {
public:
    using const_iterator = OpenFiles_iterator<typename Container::const_iterator>;

    OpenFiles_view(const Container& range) : begin_(range.begin(), range.end()), end_(range.end(), range.end()) {}

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

class OpenFiles {};

template <typename Iterator>
class Split_iterator {
public:
    using value_type = std::string;

    Split_iterator(Iterator current, Iterator end, const std::string& delimiters)
        : current_(current), end_(end), delimiters_(delimiters), index_(0) {
        if (current_ != end) {
            const_cast<std::remove_cvref_t<decltype(*current_)>&>(*current_).seekg(0);
            std::istreambuf_iterator<char> begin((*current_).rdbuf()), end;
            buffer_.assign(begin, end);
            NextToken();
        }
    }

    bool operator==(const Split_iterator& other) const {
        return current_ == other.current_;
    }

    Split_iterator& operator++() {
        NextToken();
        return *this;
    }

    std::string operator*() const {
        return token_;
    }

private:
    void NextToken() {
        if (index_ >= buffer_.size()) {
            ++current_;
            if (current_ == end_) {
                return;
            } else {
                index_ = 0;
                std::istreambuf_iterator<char> begin((*current_).rdbuf()), end;
                buffer_.assign(begin, end);
            }
        }

        token_.clear();
        while (index_ < buffer_.size()) {
            if (std::find(delimiters_.begin(), delimiters_.end(), buffer_[index_]) != delimiters_.end()) {
                ++index_;
                break;
            }
            token_ += buffer_[index_];
            ++index_;
        }
    }

    Iterator current_;
    Iterator end_;
    std::string delimiters_;
    std::string token_;
    std::string buffer_;
    size_t index_;
};

template <typename Container>
class Split_view {
public:
    using const_iterator = Split_iterator<typename Container::const_iterator>;

    Split_view(const Container& range, const std::string& delimiters) 
        : begin_iterator_(range.begin(), range.end(), delimiters),
          end_iterator_(range.end(), range.end(), delimiters) {}

    const_iterator begin() const {
        return begin_iterator_;
    }

    const_iterator end() const {
        return end_iterator_;
    }

private:
    const_iterator begin_iterator_;
    const_iterator end_iterator_;
};

class Split {
public:
	Split(const std::string& delimiters) : delimiters_(delimiters) {}

	std::string GetDelimiters() const {
		return delimiters_;
	}
private:
	std::string delimiters_;
};

template<typename Container>
class AsDataFlow {
public:
    using const_iterator = Container::const_iterator;
    using value_type     = Container::value_type;

    AsDataFlow(const Container& data) : start_(data.begin()), finish_(data.end()) {}

    const_iterator begin() const {
        return start_;
    }

    const_iterator end() const {
        return finish_;
    }

private:
    const_iterator start_;
    const_iterator finish_;
};

template <typename Stream, typename Delimeter>
class Write {
public:
	Write(Stream& stream, const Delimeter& delimeter) : stream_(stream), delimeter_(delimeter) {}

	Stream& GetStream() const {
		return stream_;
	}

	Delimeter GetDelimeter() const {
		return delimeter_;
	}
private:
	Stream& stream_;
	Delimeter delimeter_;
}; 

template<typename Stream>
class Out {
public:
	Out(Stream& stream)
	: write_(stream, ' ')
	{}

	Write<Stream, char> GetWrite() const {
		return write_;
	}
private:
	Write<Stream, char> write_;
};

template<typename Iter, typename Transformation>
class TransformIterator {
public:
    using value_type = decltype(std::declval<Transformation>()(*std::declval<Iter>()));

    TransformIterator(Iter iterator, const Transformation& transform)
        : iterator_(iterator), transform_(transform) {}

    bool operator==(const TransformIterator& other) const {
        return iterator_ == other.iterator_;
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
    const Transformation& transform_;
};

template<typename Collection, typename Transformation>
class Transform_view {
public:
    using const_iterator = TransformIterator<typename Collection::const_iterator, Transformation>;

    Transform_view(const Collection& collection, const Transformation& transform)
        : start_(collection.begin(), transform), end_(collection.end(), transform) {}

    const_iterator begin() const {
        return start_;
    }

    const_iterator end() const {
        return end_;
    }

private:
    const_iterator start_;
    const_iterator end_;
};

template<typename Func>
class Transform {
public:
	Transform(const Func& func) : func_(func) {}

	const Func& GetFunc() const {
		return func_;
	}
private:
	const Func& func_;
};

template<typename Iterator, typename Predicate>
class FilterIterator {
public:
    using value_type = typename std::iterator_traits<Iterator>::value_type;

    FilterIterator(Iterator current, Iterator end, const Predicate& predicate)
        : current_(current), end_(end), predicate_(predicate) 
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

    value_type operator*() const {
        return *current_;
    }

private:
    void AdvanceToNextValid() {
        while (current_ != end_ && !predicate_(*current_)) {
            ++current_;
        }
    }

    Iterator current_;
    Iterator end_;
    const Predicate& predicate_;
};

template<typename Container, typename Predicate>
class Filter_view {
public:
    using const_iterator = FilterIterator<typename Container::const_iterator, Predicate>;

    Filter_view(const Container& container, const Predicate& predicate)
        : begin_(container.begin(), container.end(), predicate),
          end_(container.end(), container.end(), predicate) {}

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

template<typename Func>
class Filter {
public:
    Filter(const Func& func) : func_(func) {}

    const Func& GetFunc() const {
        return func_;
    }
private:
    const Func& func_;
};

template <typename Iterator>
class DropNullopt_iterator {
public:
    using value_type = typename Iterator::value_type;

    DropNullopt_iterator(Iterator current, Iterator last) : current_(current), last_(last) {
        MoveToNextValid();
    }

    bool operator==(const DropNullopt_iterator& other) const {
        return current_ == other.current_;
    }

    DropNullopt_iterator& operator++() {
        ++current_;
        MoveToNextValid();
        return *this;
    }

    auto operator*() const {
        return *current_;
    }

private:
    void MoveToNextValid() {
        while (current_ != last_ && *current_ == std::nullopt) {
            ++current_;
        }
    }

    Iterator current_;
    Iterator last_;
};

template<typename Container>
class DropNullopt_view {
public:
    using const_iterator = DropNullopt_iterator<typename Container::const_iterator>;

    DropNullopt_view(const Container& container)
        : begin_iterator_(container.begin(), container.end()),
          end_iterator_(container.end(), container.end()) {}

    const_iterator begin() const {
        return begin_iterator_;
    }

    const_iterator end() const {
        return end_iterator_;
    }

private:
    const_iterator begin_iterator_;
    const_iterator end_iterator_;
};
class DropNullopt 
{};

class AsVector 
{};

template <typename Container, typename T, typename Aggregator, typename KeyFunc>
class AggregateByKey_iterator {
    using KeyType = decltype(std::declval<KeyFunc>()(std::declval<typename Container::const_iterator::value_type>()));
    using ValueType = std::remove_reference_t<T>;

public:
    using value_type = std::pair<KeyType, ValueType>;

    AggregateByKey_iterator(std::shared_ptr<std::unordered_map<KeyType, ValueType>> mp,
                            std::shared_ptr<std::vector<KeyType>> vec,
                            typename std::vector<KeyType>::const_iterator it) : mp_(mp), vec_(vec), it_(it) {}

    bool operator==(const AggregateByKey_iterator& other) const {
        return it_ == other.it_;
    }

    AggregateByKey_iterator& operator++() {
        ++it_;
        return *this;
    }

    auto operator*() const {
        return std::make_pair(*it_, mp_->at(*it_));
    }

private:
    std::shared_ptr<std::unordered_map<KeyType, ValueType>> mp_;
    std::shared_ptr<std::vector<KeyType>> vec_;
    typename std::vector<KeyType>::const_iterator it_;
};

template <typename Container, typename T, typename Aggregator, typename KeyFunc>
class AggregateByKey_view {
    using KeyType = decltype(std::declval<KeyFunc>()(std::declval<typename Container::const_iterator::value_type>()));
    using ValueType = std::remove_reference_t<T>;

public:
    using const_iterator = AggregateByKey_iterator<Container, T, Aggregator, KeyFunc>;

    AggregateByKey_view(std::shared_ptr<std::unordered_map<KeyType, ValueType>> mp,
                        std::shared_ptr<std::vector<KeyType>> vec)
        : mp_(mp), vec_(vec), begin_(mp, vec, vec->begin()), end_(mp, vec, vec->end()) {}

    const_iterator begin() const {
        return begin_;
    }

    const_iterator end() const {
        return end_;
    }

private:
    std::shared_ptr<std::unordered_map<KeyType, ValueType>> mp_;
    std::shared_ptr<std::vector<KeyType>> vec_;
    const_iterator begin_;
    const_iterator end_;
};

template <typename T, typename Aggregator, typename KeyFunc>
class AggregateByKey {
public:
    AggregateByKey(const T& start_value, const Aggregator& aggregator, const KeyFunc& key_func)
        : start_value_(start_value), aggregator_(aggregator), key_func_(key_func) {}

    T GetStartValue() const {
        return start_value_;
    }

    const Aggregator& GetAggregator() const {
        return aggregator_;
    }

    const KeyFunc& GetKeyFunc() const {
        return key_func_;
    }

private:
    T start_value_;
    Aggregator aggregator_; 
    KeyFunc key_func_; 
};

template <typename Iterator, bool WithError>
class SplitExpected_iterator {
public:
	using value_type = std::conditional_t<WithError, typename Iterator::value_type::error_type, typename Iterator::value_type::value_type>;

	SplitExpected_iterator(Iterator it, Iterator end) : it_(it), end_(end) {
		Next();
	}

	bool operator==(const SplitExpected_iterator& other) const {
		return it_ == other.it_;
	}

	SplitExpected_iterator operator++() {
		++it_;
		Next();
		return *this;
	}

	auto operator*() const {
		if constexpr (WithError) {
			return (*it_).error();
		} else {
			return *it_;
		}
	}
private:
	void Next() {
		if (WithError) {
			while (it_ != end_ && *it_)
				++it_;
		} else {
			while (it_ != end_ && !(*it_))
				++it_;
		}
	}
	Iterator it_; 
	Iterator end_; 
};

template <typename Container, bool WithError>
class SplitExpected_view {
public:
	using const_iterator = SplitExpected_iterator<typename Container::const_iterator, WithError>;

	SplitExpected_view(const Container& range) : begin_(range.begin(), range.end()), end_(range.end(), range.end()) {}

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

class SplitExpected 
{};

template <typename Key, typename Value>
struct KV {
	using key_type   = Key;
	using value_type = Value;

	Key key;
	Value value;

	bool operator==(const KV& other) const = default;
};

template <typename Base, typename Joined>
struct JoinResult {
	Base base;
	std::optional<Joined> joined;

	bool operator==(const JoinResult& other) const = default;
};

template <typename>
struct is_kv : std::false_type {};

template <typename T, typename U>
struct is_kv<KV<T, U>> : std::true_type {};

template <typename T>
constexpr bool is_kv_v = is_kv<T>::value;

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

	Join_view(const LeftContainer& left_range, std::unique_ptr<std::unordered_map<KeyRightFunc, ValRightFunc>> mp, const LeftFunc& left_func_)
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