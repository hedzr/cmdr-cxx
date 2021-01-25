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

#include <any>
#include <cassert>
#include <iomanip>
#include <regex>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>


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
                ss << std::setfill(c) << std::setw(n + l.length()) << l << std::endl;
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

#endif //CMDR_CXX11_CMDR_STRING_HH
