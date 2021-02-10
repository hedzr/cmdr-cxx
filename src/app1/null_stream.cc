//
// Created by Hedzr Yeh on 2021/1/14.
//

#include <cmdr11/cmdr_defs.hh>
#include <ostream>
#include <streambuf>
#include <tuple>


template<class cT, class traits = std::char_traits<cT>>
class basic_nullbuf : public std::basic_streambuf<cT, traits> {
    typename traits::int_type overflow(typename traits::int_type c) {
        return traits::not_eof(c); // indicate success
    }
};

template<class cT, class traits = std::char_traits<cT>>
class basic_nullstream : public std::basic_ostream<cT, traits> {
public:
    basic_nullstream()
        : std::basic_ios<cT, traits>(&m_sbuf)
        , std::basic_ostream<cT, traits>(&m_sbuf) {
        init(&m_sbuf);
    }

private:
    static void init(basic_nullbuf<cT, traits> *sbuf) {
        UNUSED(sbuf);
    }

private:
    basic_nullbuf<cT, traits> m_sbuf;
};

typedef basic_nullstream<char> nullstream;
typedef basic_nullstream<wchar_t> wnullstream;

// --------------

template<class cT, class traits = std::char_traits<cT>>
class basic_encoder_buf : public std::basic_streambuf<cT, traits> {
    typename traits::int_type overflow(typename traits::int_type c) {
        return traits::not_eof(c); // indicate success
    }
};

template<class cT, class traits = std::char_traits<cT>>
class basic_encoder : public std::basic_ostream<cT, traits> {
public:
    basic_encoder()
        : std::basic_ios<cT, traits>(&m_sbuf)
        , std::basic_ostream<cT, traits>(&m_sbuf) {
        init(&m_sbuf);
    }

private:
    static void init(basic_nullbuf<cT, traits> *sbuf) {
        UNUSED(sbuf);
    }

private:
    basic_nullbuf<cT, traits> m_sbuf;
};

typedef basic_encoder<char> encoder;
typedef basic_encoder<wchar_t> wencoder;

void test_encoder() {
    encoder os;
    std::tuple<int, std::string> mc;
    os << 666 << std::get<1>(mc);
}

// --------------

class MyClass {
    int a;

    friend std::ostream &operator<<(std::ostream &, MyClass const &);

public:
    MyClass() = default;
    ~MyClass() = default;
};

inline std::ostream &operator<<(std::ostream &out, MyClass const &b) {
    std::cout << "call format function!!" << '\n';
    out << b.a;
    return out;
}

void test_null_stream() {
    nullstream os;
    MyClass mc{};
    os << 666 << mc;
}
