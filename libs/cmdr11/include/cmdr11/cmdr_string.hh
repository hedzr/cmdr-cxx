/**
 * cmdr -- C++17 Command Line Arguments Parser
 *
 * @file
 * @brief Utilities for string operations.
 *
 * @ , copyright © 2016 - 2021 Hedzr Yeh.
 *
 * This file is part of cmdr (cmdr-c17 for C++ version).
 *
 * cmdr is free software: you can redistribute it and/or modify
 * it under the terms of the MIT License..
 *
 * cmdr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the MIT License
 * along with cmdr.  If not, see <https://github.com/hedzr/cmdr-c17/blob/master/LICENSE>.
 */

#ifndef CMDR_CXX11_CMDR_STRING_HH
#define CMDR_CXX11_CMDR_STRING_HH

#include <cassert>

#include <type_traits>

#include <chrono>

#include <iomanip>
#include <sstream>

#include <any>
#include <array>
#include <string>
#include <variant>
#include <vector>

#include <regex>
#include <utility>

#include "cmdr_defs.hh"


namespace cmdr::string {

    inline string_array tokenize(const std::string &s) {
        string_array tokens;
        std::string token;

        for (const auto &c : s) {
            if (!isspace(c))
                token += c;
            else {
                if (token.length())
                    tokens.push_back(token);
                token.clear();
            }
        }

        if (token.length())
            tokens.push_back(token);

        return tokens;
    }

    inline std::vector<std::string> tokenize(const std::string &s, char c) {
        auto end = s.cend();
        auto start = end;

        string_array v;
        for (auto it = s.cbegin(); it != end; ++it) {
            if (*it != c) {
                if (start == end)
                    start = it;
                continue;
            }
            if (start != end) {
                v.emplace_back(start, it);
                start = end;
            }
        }
        if (start != end)
            v.emplace_back(start, end);
        return v;
    }

    // string_array out;
    // split("", ",", std::back_inserter(out));
    // split("Hello, world!", "", std::back_inserter(out));
    // split("abxycdxyxydefxya", "xyz", std::back_inserter(out));
    // split("abxycdxy!!xydefxya", "xy", std::back_inserter(out));
    // ...
    template<class Iter>
    inline Iter split(const std::string &s, const std::string &delim, Iter out) {
        if (delim.empty()) {
            *out++ = s;
            return out;
        }
        size_t a = 0, b = s.find(delim);
        for (; b != std::string::npos;
             a = b + delim.length(), b = s.find(delim, a)) {
            *out++ = std::move(s.substr(a, b - a));
        }
        *out++ = std::move(s.substr(a, s.length() - a));
        return out;
    }

    inline string_array split(const std::string &s, char delim = '\n') {
        std::stringstream ss(s);
        std::string item;
        std::vector<std::string> elems;
        while (std::getline(ss, item, delim)) {
            elems.push_back(std::move(item));
        }
        return elems;
    }

    // cut_by: R"([\s,]+)"
    // cut_by: R"(\r?\n)"
    inline string_array reg_split(const std::string &s, const char *cut_by = R"(\r?\n)") {
        std::regex regex{cut_by};
        std::sregex_token_iterator it{s.begin(), s.end(), regex, -1};
        string_array words{it, {}};
        return words;
    }


    inline std::string join(std::vector<char const *> const &array, char delim = ',', char before = '\0', char after = '\0') {
        std::stringstream ss;
        int i = 0;
        if (before != '\0')
            ss << before;
        for (auto const &it : array) {
            if (i++ > 0)
                ss << delim;
            ss << it;
        }
        if (after != '\0')
            ss << after;
        return ss.str();
    }

    inline std::string join(std::vector<char const *> const &array, const_chars delim = ",", const_chars before = nullptr, const_chars after = nullptr) {
        std::stringstream ss;
        int i = 0;
        if (before)
            ss << before;
        for (auto const &it : array) {
            if (i++ > 0)
                ss << delim;
            ss << it;
        }
        if (after)
            ss << after;
        return ss.str();
    }

    inline std::string join(string_array const &array, char delim = ',', char before = '\0', char after = '\0') {
        std::stringstream ss;
        int i = 0;
        if (before != '\0')
            ss << before;
        for (auto const &it : array) {
            if (i++ > 0)
                ss << delim;
            ss << it;
        }
        if (after != '\0')
            ss << after;
        return ss.str();
    }

    inline std::string join(string_array const &array, const_chars delim = ",", const_chars before = nullptr, const_chars after = nullptr) {
        std::stringstream ss;
        int i = 0;
        if (before)
            ss << before;
        for (auto const &it : array) {
            if (i++ > 0)
                ss << delim;
            ss << it;
        }
        if (after)
            ss << after;
        return ss.str();
    }


    inline bool replace(std::string &str, const std::string &from, const std::string &to) {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }

    inline void replace_all(std::string &str, const std::string &from, const std::string &to) {
        if (from.empty())
            return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }

    inline std::string reg_replace(const std::string &s,
                                   const std::string &reg_find,
                                   const std::string &replaced_by,
                                   std::regex_constants::match_flag_type flags =
                                           std::regex_constants::match_any) {
        std::string r = std::regex_replace(s, std::regex(reg_find), replaced_by, flags);
        return r;
    }


    template<typename T>
    inline std::basic_string<T>
    pad_left(const std::basic_string<T> &str, typename std::basic_string<T>::size_type n = 2, T c = ' ') {
        std::stringstream ss;
        auto vec = reg_split(str);
        if (vec.size() <= 1) {
            ss << std::setfill(c) << std::setw(n + str.length()) << str;
        } else {
            for (auto l : vec) {
                ss << std::setfill(c) << std::setw(n + l.length()) << l << '\n';
            }
        }
        return ss.str();
    }

    template<typename T>
    inline void pad(std::basic_string<T> &s,
                    typename std::basic_string<T>::size_type n, T c) {
        if (n > s.length())
            s.append(n - s.length(), c);
    }

    inline void to_upper(std::string &input) {
        std::locale locale;
        auto to_upper = [&locale](char ch) { return std::use_facet<std::ctype<char>>(locale).toupper(ch); };
        std::transform(input.begin(), input.end(), input.begin(), to_upper);

        // std::for_each(std::begin(input), std::end(input), [](char &c) {
        //     c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        // });
    }

    inline void to_lower(std::string &input) {
        std::locale locale;
        auto to_lower = [&locale](char ch) { return std::use_facet<std::ctype<char>>(locale).tolower(ch); };
        std::transform(input.begin(), input.end(), input.begin(), to_lower);

        // std::for_each(std::begin(input), std::end(input), [](char &c) {
        //     c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        // });
    }

    template<class ch = char>
    inline void to_upper_t(std::basic_string<ch, std::char_traits<ch>, std::allocator<ch>> &input) {
        auto &f = std::use_facet<std::ctype<ch>>(std::locale());
        f.toupper(input.data(), input.data() + input.size());

        // auto &f = std::use_facet<std::ctype<ch> >(std::locale());
        // f.toupper(const_cast<ch *>(input.data()), input.data() + input.size());
    }

    template<class ch = char>
    inline void to_lower_t(std::basic_string<ch, std::char_traits<ch>, std::allocator<ch>> &input) {
        auto &f = std::use_facet<std::ctype<ch>>(std::locale());
        f.tolower(input.data(), input.data() + input.size());

        // auto &f = std::use_facet<std::ctype<ch> >(std::locale());
        // f.toupper(const_cast<ch *>(input.data()), input.data() + input.size());
    }

    inline void to_upper_trans(std::string &str) {
        // std::for_each(str.begin(), str.end(), [](char &c) { c = ::toupper(c); });
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    }

    inline std::string to_capitalize(const std::string &text) {
        std::string s;
        for (int it : text) {
            if (it > 96 && it < 123) {
                s += char(it - 32);
            } else {
                s += char(it);
            }
        }
        return s;
    }


    inline bool has_prefix(const std::string &str, const_chars prefix) {
        int sl = std::strlen(prefix), ml = str.length();
        if (ml >= sl) {
            if (str.compare(0, sl, prefix) == 0)
                return true;
        }
        return false;
    }

    inline bool has_prefix(const std::string &str, const std::string &prefix) {
        int sl = prefix.length(), ml = str.length();
        if (ml >= sl) {
            if (str.compare(0, sl, prefix) == 0)
                return true;
        }
        return false;
    }

    inline bool has_suffix(const std::string &str, const_chars suffix) {
        int sl = std::strlen(suffix), ml = str.length();
        if (ml >= sl) {
            if (str.compare(ml - sl, sl, suffix) == 0)
                return true;
        }
        return false;
    }

    inline bool has_suffix(const std::string &str, const std::string &suffix) {
        int sl = suffix.length(), ml = str.length();
        if (ml >= sl) {
            if (str.compare(ml - sl, sl, suffix) == 0)
                return true;
        }
        return false;
    }

    inline std::string wrap(const std::string &str, char pre = '[', char post = ']') {
        std::stringstream ss;
        ss << pre << str << post;
        return ss.str();
    }

    inline std::string wrap(const std::string &str, char const *pre = "[", char const *post = "]") {
        std::stringstream ss;
        ss << pre << str << post;
        return ss.str();
    }

    inline std::string wrap(const std::string &str, std::string const &pre, std::string const &post) {
        std::stringstream ss;
        ss << pre << str << post;
        return ss.str();
    }

    inline std::string quoted(const std::string &str) {
        return wrap(str, '"', '"');
    }

    std::string read_until(std::istream &in, char delimiter = ',');

    inline std::string read_until(std::istream &in, char delimiter) {
        std::string cr;
        size_t sz = 1, tot;
        do {
            std::string temp;
            std::getline(in, temp, delimiter);
            cr += temp + delimiter;
            tot = cr.size();
        } while ((tot < sz) || (cr.at(tot - sz) != delimiter));
        return cr.substr(0, tot - sz); // or return cr; if you want to keep the delimiter
    }

    inline std::string read_until(std::istream &in, char const *delimiter = ",") {
        std::string cr;
        char delim = *delimiter;
        size_t sz = std::strlen(delimiter), tot;
        do {
            std::string temp;
            std::getline(in, temp, delim);
            cr += temp + delim;
            tot = cr.size();
        } while ((tot < sz) || (cr.substr(tot - sz, sz) != delimiter));
        return cr.substr(0, tot - sz); // or return cr; if you want to keep the delimiter
    }

    inline bool read_until(std::istream &in, std::string &ret, char const *delimiter = ",") {
        std::string cr;
        char delim = *delimiter;
        size_t sz = std::strlen(delimiter), tot;
        do {
            std::string temp;
            if (!std::getline(in, temp, delim))
                return false;
            cr += temp + delim;
            tot = cr.size();
        } while ((tot < sz) || (cr.substr(tot - sz, sz) != delimiter));
        ret = cr.substr(0, tot - sz); // or return cr; if you want to keep the delimiter
        return true;
    }


    inline std::string strip(const std::string &str, char const *pre, char const *post) {
        std::size_t p1 = 0, p2 = str.length();
        if (has_prefix(str, pre))
            p1 = std::strlen(pre);
        if (has_suffix(str, post))
            p2 -= std::strlen(post);
        return str.substr(p1, p2 - p1);
    }

    inline std::string strip_quotes(const std::string &str) {
        std::size_t p1 = 0, p2 = str.length() - 1;
        while (str[p1] == '"' || str[p1] == '\'') p1++;
        while (str[p2] == '"' || str[p2] == '\'') p2--;
        if (p2 + 1 < p1) p2 = p1 - 1;
        return str.substr(p1, p2 + 1);
    }

    inline void strip_quotes(std::istream &is, std::string &s) {
        auto ch = is.peek();
        if (ch == '\'' || ch == '"') {
            char c;
            is >> c;
            s = read_until(is, ch);
        } else
            is >> s;
    }


    inline std::string &trim_right_space(std::string &s) {
        s.erase(s.find_last_not_of(" \n\r\t\v\f") + 1);
        return s;
    }

    inline std::string trim_right_space(std::string &&s) {
        s.erase(s.find_last_not_of(" \n\r\t\v\f") + 1);
        return std::move(s);
    }

    inline std::string &trim_left_space(std::string &s) {
        s.erase(0, s.find_first_not_of(" \n\r\t\v\f"));
        return s;
    }

    inline std::string trim_left_space(std::string &&s) {
        s.erase(0, s.find_first_not_of(" \n\r\t\v\f"));
        return std::move(s);
    }


    inline std::string expand_env(std::string text) {
        // static const std::regex env_re{R"--(\$\{?([^}]+)\}?)--"};
        static const std::regex env_re{R"(\$\{?([a-z0-9_]+)\}?)", std::regex::icase};
        std::smatch match;
        while (std::regex_search(text, match, env_re)) {
            auto const &from = match[0];
            auto const &th = match[1];
            auto const &var_name = th.str();
            auto ptr = std::getenv(var_name.c_str());
            text.replace(from.first, from.second, ptr ? ptr : "");
        }
        return text;
    }


    namespace conv {

        //        static constexpr int is_signed = std::is_signed<T>::value ? 1 : 0;
        //        // 2.40824 == ln(256) / ln(10), converting from byte length to digits.
        //        static_assert(sz > constexpr_ceil(2.40824 * (sizeof(T) - 0.125 * is_signed)) + is_signed,
        //                      "The size of the array is not large enough to hold the maximum value of T.");

        const char digit_pairs[201] = {
                "00010203040506070809"
                "10111213141516171819"
                "20212223242526272829"
                "30313233343536373839"
                "40414243444546474849"
                "50515253545556575859"
                "60616263646566676869"
                "70717273747576777879"
                "80818283848586878889"
                "90919293949596979899"};


        //
        // from: https://stackoverflow.com/questions/4351371/c-performance-challenge-integer-to-stdstring-conversion
        //
        inline std::string &itoa(int n, std::string &s) {
            if (n == 0) {
                s = "0";
                return s;
            }

            int sign = -(n < 0);
            unsigned int val = (n ^ sign) - sign;

            int size;
            if (val >= 10000) {
                if (val >= 10000000) {
                    if (val >= 1000000000)
                        size = 10;
                    else if (val >= 100000000)
                        size = 9;
                    else
                        size = 8;
                } else {
                    if (val >= 1000000)
                        size = 7;
                    else if (val >= 100000)
                        size = 6;
                    else
                        size = 5;
                }
            } else {
                if (val >= 100) {
                    if (val >= 1000)
                        size = 4;
                    else
                        size = 3;
                } else {
                    if (val >= 10)
                        size = 2;
                    else
                        size = 1;
                }
            }
            size -= sign;
            s.resize(size);
            char *c = &s[0];
            if (sign)
                *c = '-';

            c += size - 1;
            while (val >= 100) {
                int pos = val % 100;
                val /= 100;
                *(short *) (c - 1) = *(short *) (digit_pairs + 2 * pos);
                c -= 2;
            }
            while (val > 0) {
                *c-- = '0' + (val % 10);
                val /= 10;
            }
            return s;
        }

        inline std::string &itoa(unsigned val, std::string &s) {
            if (val == 0) {
                s = "0";
                return s;
            }

            int size;
            if (val >= 10000) {
                if (val >= 10000000) {
                    if (val >= 1000000000)
                        size = 10;
                    else if (val >= 100000000)
                        size = 9;
                    else
                        size = 8;
                } else {
                    if (val >= 1000000)
                        size = 7;
                    else if (val >= 100000)
                        size = 6;
                    else
                        size = 5;
                }
            } else {
                if (val >= 100) {
                    if (val >= 1000)
                        size = 4;
                    else
                        size = 3;
                } else {
                    if (val >= 10)
                        size = 2;
                    else
                        size = 1;
                }
            }

            s.resize(size);
            char *c = &s[size - 1];
            while (val >= 100) {
                int pos = val % 100;
                val /= 100;
                *(short *) (c - 1) = *(short *) (digit_pairs + 2 * pos);
                c -= 2;
            }
            while (val > 0) {
                *c-- = '0' + (val % 10);
                val /= 10;
            }
            return s;
        }


    } // namespace conv

} // namespace cmdr::string

#if __GNUC__ < 10
#include <math.h>
#else
#include <cmath>
#endif

namespace cmdr::text {

    typedef double distance;

    namespace detail {

#if __GNUC__ < 10
        inline double floor(double x) { return ::floor(x); }
#else
        inline double floor(double x) { return std::floor(x); }
#endif

        // constexpr float EPSILON = 0.0001; // 1e-4
        constexpr float EPSILON = 0.00000001; // 1e-8

        /// @brief      See if two floating point numbers are approximately equal.
        /// @param[in]  a        number 1
        /// @param[in]  b        number 2
        /// @param[in]  epsilon  A small value such that if the difference between the two numbers is
        ///                      smaller than this they can safely be considered to be equal.
        /// @return     true if the two numbers are approximately equal, and false otherwise
        inline bool is_float_eq(float a, float b, float epsilon = EPSILON) {
            return ((a - b) < epsilon) && ((b - a) < epsilon);
        }
        inline bool is_double_eq(double a, double b, double epsilon = EPSILON) {
            return ((a - b) < epsilon) && ((b - a) < epsilon);
        }

        /// @brief      See if floating point number `a` is > `b`
        /// @param[in]  a        number 1
        /// @param[in]  b        number 2
        /// @param[in]  epsilon  a small value such that if `a` is > `b` by this amount, `a` is considered
        ///             to be definitively > `b`
        /// @return     true if `a` is definitively > `b`, and false otherwise
        inline bool is_float_gt(float a, float b, float epsilon = EPSILON) {
            return a > b + epsilon;
        }
        inline bool is_double_gt(double a, double b, double epsilon = EPSILON) {
            return a > b + epsilon;
        }

        /// @brief      See if floating point number `a` is < `b`
        /// @param[in]  a        number 1
        /// @param[in]  b        number 2
        /// @param[in]  epsilon  a small value such that if `a` is < `b` by this amount, `a` is considered
        ///             to be definitively < `b`
        /// @return     true if `a` is definitively < `b`, and false otherwise
        inline bool is_float_lt(float a, float b, float epsilon = EPSILON) {
            return a < b - epsilon;
        }
        inline bool is_double_lt(double a, double b, double epsilon = EPSILON) {
            return a < b - epsilon;
        }

        /// @brief      Returns true if `a` is definitively >= `b`, and false otherwise
        inline bool is_float_ge(float a, float b, float epsilon = EPSILON) {
            return a > b - epsilon;
        }
        inline bool is_double_ge(double a, double b, double epsilon = EPSILON) {
            return a > b - epsilon;
        }

        /// @brief      Returns true if `a` is definitively <= `b`, and false otherwise
        inline bool is_float_le(float a, float b, float epsilon = EPSILON) {
            return a < b + epsilon;
        }
        inline bool is_double_le(double a, double b, double epsilon = EPSILON) {
            return a < b + epsilon;
        }

    } // namespace detail


    class distance_base {
    public:
        // distance operator()(const_chars text) { return calc(text, 0.7); }
        distance operator()(const_chars text, const_chars input) { return calc(text, input, false); }
        // distance_base &operator+(const_chars text, const_chars input) {
        //     calc(text, input, 0.7, true);
        //     return (*this);
        // }
        explicit operator distance() const { return get_distance(); }

        bool operator==(double d) const { return detail::is_double_eq(get_distance(), d); }
        bool equal(double d) const { return detail::is_double_eq(get_distance(), d); }

        distance_base &calculate(const_chars text, const_chars input) {
            calc(text, input, false);
            return (*this);
        }
        bool matched() const { return detail::is_double_ge(get_distance(), get_threshold()); }

    protected:
        virtual distance calc(const_chars text, const_chars input, bool append) = 0;
        [[nodiscard]] virtual distance get_distance() const = 0;
        [[nodiscard]] virtual distance get_threshold() const = 0;
    };


    class jaro_winkler_distance final : public distance_base {
    public:
        explicit jaro_winkler_distance(distance threshold = 0.7, bool case_sensitive = true)
            : _threshold(threshold)
            , _case_sensitive(case_sensitive) {}
        ~jaro_winkler_distance() = default;

        [[nodiscard]] distance get_distance() const override { return _distance; }

    protected:
        distance calc(const_chars text, const_chars input, bool append) override {
            if (append) {
                if (!detail::is_double_eq(_distance, 0))
                    return _distance;
            }

            _distance = 0;

            auto len1 = std::strlen(text);
            auto len2 = std::strlen(input);

            // Exit early if either are empty
            if (len1 == 0 || len2 == 0) {
                return _distance;
            }

            std::string s1(text);
            std::string s2(input);

            // Convert to lower if case-sensitive is false
            if (!_case_sensitive) {
                std::transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
                std::transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
            }

            // Exit early if they're an exact match.
            if (s1 == s2) {
                return 1;
            }

            // Maximum distance upto which matching
            // is allowed
            int max_dist = (int) (detail::floor(std::max(len1, len2) / 2) - 1);

            // Count of matches
            int match = 0;

            // Hash for matches
            std::vector<int> hash_s1{};
            std::vector<int> hash_s2{};
            hash_s1.reserve(len1);
            hash_s2.reserve(len2);
            for (int i = 0; i < (int) len1; i++) hash_s1[i] = 0;
            for (int i = 0; i < (int) len2; i++) hash_s2[i] = 0;
            // int hash_s1[s1.length()] = { 0 },
            //         hash_s2[s2.length()] = { 0 };

            // Traverse through the first string
            for (int i = 0; i < (int) len1; i++) {

                // Check if there is any matches
                for (int j = std::max(0, i - max_dist);
                     j < std::min((int) len2, i + max_dist + 1); j++)

                    // If there is a match
                    if (s1[i] == s2[j] && hash_s2[j] == 0) {
                        hash_s1[i] = 1;
                        hash_s2[j] = 1;
                        match++;
                        break;
                    }
            }

            // If there is no match
            if (match == 0)
                return _distance;

            // Number of transpositions
            distance t = 0;

            int point = 0;

            // Count number of occurances
            // where two characters match but
            // there is a third matched character
            // in between the indices
            for (int i = 0; i < (int) len1; i++)
                if (hash_s1[i]) {

                    // Find the next matched character
                    // in second string
                    while (hash_s2[point] == 0)
                        point++;

                    if (s1[i] != s2[point++])
                        t++;
                }

            t /= 2;

            // Return the Jaro Similarity
            _distance = (((distance) match) / ((distance) len1) + ((distance) match) / ((distance) len2) + ((distance) match - t) / ((distance) match)) / 3.0;
            return _distance;
        }
        [[nodiscard]] distance get_threshold() const override { return _threshold; }

    private:
        distance _threshold{0.7};
        // const double StringMetricFactor = 100000000000;
        bool _case_sensitive{true};
        distance _distance{};
    };

} // namespace cmdr::text

#endif //CMDR_CXX11_CMDR_STRING_HH
