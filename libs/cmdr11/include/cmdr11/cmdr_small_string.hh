//
// Created by Hedzr Yeh on 2021/1/16.
//

#ifndef CMDR_CXX11_CMDR_SHORT_STRING_HH
#define CMDR_CXX11_CMDR_SHORT_STRING_HH

#include <cstring> // memcpy
#include <iosfwd>  // ostream operator <<
#include <string>

// NOTE: SSO has been paused and the codes in this header is not final.

namespace cmdr::string {

    // not yet
    template<class size_type = std::size_t>
    class sso_string {
    public:
        // all 83 member functions
    private:
        typedef struct heap {
            std::unique_ptr<char[]> _data; // 8b
            size_type _size;               // 4b/8b
            size_type _capacity;           // 4b/8b
        } heap;
        union {
            struct heap _heap;
            std::array<char, sizeof(_heap)> _sso{};
        };
    };


    // based on : https://codereview.stackexchange.com/q/148757
    template<std::size_t Size, class T = char,
             std::size_t HUGE_BUFFER_SIZE = 65536>
    class small_string_preallocated {
    public:
        using size_type = std::size_t;
        using value_type = T;

        constexpr small_string_preallocated() noexcept
            : _buffer{} { set_size(0u); }

        template<size_type N>
        constexpr explicit small_string_preallocated(const T (&str)[N]) noexcept
            : _buffer{} {
            auto size = N >= Size ? Size - 1 : N - 1;
            std::memcpy(_buffer, str, size);
            set_size(size);
        }

        template<class Traits, class Allocator>
        small_string_preallocated(
                const std::basic_string<T, Traits, Allocator> &str) noexcept
            : _buffer{} {
            auto size = str.size() >= Size ? Size : str.size();
            std::memcpy(_buffer, str.c_str(), size);
            set_size(size);
        }

        template<size_type N>
        constexpr small_string_preallocated(
                const small_string_preallocated<N> &rhs) noexcept
            : _buffer{} {
            auto size = N >= Size ? Size : N;
            std::memcpy(_buffer, rhs.c_str(), size);
            set_size(size);
        }

        template<size_type N>
        constexpr small_string_preallocated &operator=(const T (&str)[N]) noexcept {
            auto size = N >= Size ? Size - 1 : N - 1;
            std::memcpy(_buffer, str, size);
            set_size(size);
            return *this;
        }

        template<class Traits, class Allocator>
        small_string_preallocated &
        operator=(const std::basic_string<T, Traits, Allocator> &str) noexcept {
            auto size = str.size() >= Size ? Size : str.size();
            std::memcpy(_buffer, str.c_str(), size);
            set_size(size);
            return *this;
        }

        template<size_type N>
        constexpr small_string_preallocated &
        operator=(const small_string_preallocated<N> &rhs) noexcept {
            if ((void *) this != (void *) &rhs) {
                auto size = N >= Size ? Size : N;
                std::memcpy(_buffer, rhs.c_str(), size);
                set_size(size);
            }
            return *this;
        }

        template<size_type N>
        small_string_preallocated(small_string_preallocated<N> &&rhs) {
            auto size = N >= Size ? Size : N;
            std::memcpy(_buffer, rhs.c_str(), size);
            set_size(size);
        }

        template<size_type N>
        small_string_preallocated &operator=(small_string_preallocated<N> &&rhs) {
            if ((void *) this != (void *) &rhs) {
                auto size = N >= Size ? Size : N;
                std::memcpy(_buffer, rhs.c_str(), size);
                set_size(size);
            }
            return *this;
        }

        inline void append(value_type value) noexcept {
            if (size() < capacity()) {
                _buffer[size()] = value;
                set_size(size() + 1);
            }
        }

        inline const value_type *c_str() const noexcept { return _buffer; }

        inline std::string to_string() const {
            return {reinterpret_cast<const char *>(_buffer, size())};
        }

        constexpr size_type size() const noexcept {
            return capacity() - _buffer[capacity()];
        }

        constexpr size_type capacity() const noexcept { return Size - 1; }

        template<size_type N>
        constexpr bool
        compare(const small_string_preallocated<N> &rhs) const noexcept {
            return compare(_buffer, rhs.c_str());
        }

        inline const value_type *begin() const { return _buffer; }

        inline const value_type *end() const { return _buffer + size(); }

        template<std::size_t SizeB>
        small_string_preallocated<Size, T> &
        operator+=(const small_string_preallocated<SizeB, T> &rhs) noexcept {
            for (const auto c : rhs) {
                append(c);
            }
            return *this;
        }

        template<std::size_t N>
        small_string_preallocated<Size, T> &operator+=(const T (&str)[N]) noexcept {
            for (const value_type *p = str; *p; ++p) {
                append(*p);
            }
            return *this;
        }

        template<class Traits, class Allocator>
        small_string_preallocated<Size, T> &
        operator+=(const std::basic_string<T, Traits, Allocator> &rhs) noexcept {
            for (const auto c : rhs) {
                append(c);
            }
            return *this;
        }

    private:
        inline void set_size(size_type size) noexcept {
            _buffer[capacity()] = Size - 1 - size;
        }

        constexpr bool compare(const value_type *lhs,
                               const value_type *rhs) const noexcept {
            return (*lhs && *rhs) ? (*lhs == *rhs && compare(lhs + 1, rhs + 1))
                                  : (!*lhs && !*rhs);
        }

        value_type _buffer[Size];
    };

    template<std::size_t SizeA, std::size_t SizeB, class T>
    small_string_preallocated<SizeA, T>
    operator+(small_string_preallocated<SizeA, T> a,
              const small_string_preallocated<SizeB, T> &b) noexcept {
        a += b;
        return a;
    }

    template<std::size_t Size, class CharT, class Traits, class Allocator>
    small_string_preallocated<Size, CharT>
    operator+(small_string_preallocated<Size, CharT> ss,
              const std::basic_string<CharT, Traits, Allocator> &str) noexcept {
        ss += str;
        return ss;
    }

    template<std::size_t Size, class T, std::size_t N>
    small_string_preallocated<Size, T>
    operator+(small_string_preallocated<Size, T> ss, const T (&str)[N]) noexcept {
        ss += str;
        return ss;
    }

    template<std::size_t Size, class T, std::size_t N>
    small_string_preallocated<Size, T>
    operator+(const T (&str)[N], small_string_preallocated<Size, T> ss) noexcept {
        ss += str;
        return ss;
    }

    template<std::size_t SizeA, std::size_t SizeB, class T>
    constexpr bool
    operator==(const small_string_preallocated<SizeA, T> &a,
               const small_string_preallocated<SizeB, T> &b) noexcept {
        return a.compare(b);
    }

    template<std::size_t Size, class CharT, class Traits, class Allocator>
    bool operator==(
            const small_string_preallocated<Size, CharT> &lhs,
            const std::basic_string<CharT, Traits, Allocator> &rhs) noexcept {
        return std::strncmp(lhs.c_str(), rhs.c_str(), Size) == 0;
    }

    template<class CharT, class Traits, class Allocator, std::size_t Size>
    bool operator==(const std::basic_string<CharT, Traits, Allocator> &lhs,
                    const small_string_preallocated<Size, CharT> &rhs) noexcept {
        return rhs == lhs;
    }

    template<std::size_t Size, class T>
    std::ostream &operator<<(std::ostream &os,
                             const small_string_preallocated<Size, T> &s) {
        return os << s.c_str();
    }

} // namespace cmdr::string

#endif // CMDR_CXX11_CMDR_SHORT_STRING_HH
