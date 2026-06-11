#pragma once


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

    template<typename T>
    void operator()(const T& value) const {
        stream_ << value << delimeter_;
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