//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_ARG_HH
#define CMDR_CXX11_CMDR_ARG_HH

#include <any>
#include <cassert>
#include <iomanip>
#include <sstream>
#include <type_traits>
#include <utility>
#include <variant>


namespace cmdr::opt {

    class arg : public obj {
    protected:
        std::string _long;
        std::string _short;
        string_array _aliases;
        std::string _desc_long;
        std::string _description;
        std::string _examples;
        std::string _placeholder;
        std::string _group, _toggle_group;
        support_types _default;

    public:
        arg() = default;
        ~arg() = default;

        arg(const arg &o) { _copy(o); }

        arg &operator=(const arg &o) {
            _copy(o);
            return (*this);
        }

        void _copy(const arg &o) {
            __COPY(_long);
            __COPY(_short);
            __COPY(_aliases);
            __COPY(_desc_long);
            __COPY(_description);
            __COPY(_examples);
            __COPY(_group);
            __COPY(_toggle_group);
            __COPY(_placeholder);
            __COPY(_default);
        }

        [[nodiscard]] bool valid() const {
            if (_long.empty()) return false;
            return true;
        }

    public:
#undef PROP_SET
#undef PROP_SET2
#undef PROP_SET3
#define PROP_SET(mn)         \
    arg &mn(const_chars s) { \
        if (s) _##mn = s;    \
        return (*this);      \
    }
#define PROP_SET2(mn)                \
    arg &title_##mn(const_chars s) { \
        if (s) _##mn = s;            \
        return (*this);              \
    }
#define PROP_SET3(mn, typ)          \
    arg &title_##mn(const typ &s) { \
        _##mn = s;                  \
        return (*this);             \
    }

        PROP_SET2(long)

        PROP_SET2(short)

        PROP_SET3(aliases, string_array)
        // PROP_SET(description)
        PROP_SET(examples)

        PROP_SET(group)

        PROP_SET(toggle_group)

        PROP_SET(desc_long)

#undef PROP_SET
#undef PROP_SET2
#undef PROP_SET3

    public:
        [[nodiscard]] std::string titles() const {
            std::stringstream ss;
            if (!_long.empty()) {
                ss << '-' << '-' << _long;
                if (!_placeholder.empty()) {
                    ss << '=' << _placeholder;
                }
            }
            if (!_short.empty()) {
                if (!_long.empty())
                    ss << ',' << ' ';
                ss << '-' << _short;
            }
            for (auto &x : _aliases) {
                ss << ',' << '-' << '-' << x;
            }
            return ss.str();
        }

        [[nodiscard]] std::string descriptions() const {
            std::stringstream ss;
            ss << _description;
            return ss.str();
        }

        [[nodiscard]] std::string defaults() const {
            std::stringstream ss;
            if (!std::holds_alternative<std::monostate>(_default)) {
                ss << ' ' << '[' << "DEFAULT=" << variant_to_string(_default) << ']';
            }
            return ss.str();
        }

        [[nodiscard]] std::string group_name() const {
            std::stringstream ss;
            if (_group.empty())
                ss << _toggle_group;
            else
                ss << _group;
            return ss.str();
        }

        [[nodiscard]] const std::string &toggle_group_name() const {
            return _toggle_group;
        }

        [[nodiscard]] bool is_toggleable() const { return !_toggle_group.empty(); }

    public:
        arg &default_value(const support_types &v) {
            _default = v;
            return (*this);
        }

        arg &description(const_chars desc, const_chars long_desc = nullptr, const_chars examples = nullptr) {
            if (desc)
                _description = desc;
            if (long_desc)
                _desc_long = long_desc;
            if (examples)
                _examples = examples;
            return (*this);
        }

        arg &placeholder(const_chars s) {
            if (s) _placeholder = s;
            return (*this);
        }

        arg &titles(const_chars title_long) {
            if (title_long) this->_long = title_long;
            return (*this);
        }

        arg &titles(const_chars title_long, const_chars title_short) {
            if (title_long) this->_long = title_long;
            if (title_short) this->_short = title_short;
            return (*this);
        }

        template<typename... T>
        arg &titles(const_chars title_long, const_chars title_short, T... title_aliases) {
            if (title_long) this->_long = title_long;
            if (title_short) this->_short = title_short;
            if (sizeof...(title_aliases) > 0) {
                this->aliases(title_aliases...);
            }
            // // must_print("%s\n", aliases...);
            // for (const_chars x : {aliases...}) {
            //     this->_aliases.push_back(x);
            // }
            return (*this);
        }

        template<typename... T>
        arg &aliases(T... title_aliases) {
            if (sizeof...(title_aliases) > 0) {
                _aliases.push_back({title_aliases...});
            }
            //for (const_chars x : {title_aliases...}) {
            //    this->_aliases.push_back(x);
            //}
            return (*this);
        }
    };

} // namespace cmdr::opt

#endif //CMDR_CXX11_CMDR_ARG_HH
