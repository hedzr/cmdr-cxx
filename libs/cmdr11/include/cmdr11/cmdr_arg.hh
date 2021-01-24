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

#include "cmdr_var_t.hh"


namespace cmdr::opt {

    /**
     * @brief base class for command/flag.
     */
    class bas : public obj {
    protected:
        std::string _long;
        std::string _short;
        string_array _aliases;
        std::string _desc_long;
        std::string _description;
        std::string _examples;
        std::string _group;
        bool _hidden;

    public:
        bas() = default;
        ~bas() override = default;
        bas(const bas &o) { _copy(o); }
        bas &operator=(const bas &o) {
            _copy(o);
            return (*this);
        }

        [[nodiscard]] bool valid() const {
            if (_long.empty()) return false;
            return true;
        }

    protected:
        void _copy(const bas &o) {
            __COPY(_long);
            __COPY(_short);
            __COPY(_aliases);
            __COPY(_desc_long);
            __COPY(_description);
            __COPY(_examples);
            __COPY(_group);
            __COPY(_hidden);
        }

    public:
#undef PROP_SET
#undef PROP_SET2
#undef PROP_SET3
#define PROP_SET(mn)         \
    bas &mn(const_chars s) { \
        if (s) _##mn = s;    \
        return (*this);      \
    }                        \
    std::string const &mn() const { return _##mn; }
#define PROP_SET2(mn)                \
    bas &title_##mn(const_chars s) { \
        if (s) _##mn = s;            \
        return (*this);              \
    }                                \
    std::string const &title_##mn() const { return _##mn; }
#define PROP_SET3(mn, typ)          \
    bas &title_##mn(const typ &s) { \
        _##mn = s;                  \
        return (*this);             \
    }                               \
    typ const &title_##mn() { return _##mn; }
#define PROP_SET4(mn, typ)  \
    bas &mn(const typ &s) { \
        _##mn = s;          \
        return (*this);     \
    }                       \
    typ const &mn() { return _##mn; }

        PROP_SET2(long)
        PROP_SET2(short)
        PROP_SET3(aliases, string_array)
        PROP_SET(examples)
        // PROP_SET(group)
        // PROP_SET(description)
        PROP_SET(desc_long)
        PROP_SET4(hidden, bool)

#undef PROP_SET
#undef PROP_SET2
#undef PROP_SET3
#undef PROP_SET4

    public:
        [[nodiscard]] virtual std::string title() const {
            std::stringstream ss;
            if (!_long.empty()) {
                ss << _long;
            }
            if (!_short.empty()) {
                if (ss.tellp() > 0)
                    ss << ',' << ' ';
                ss << _short;
            }
            auto sp = " ";
            for (auto &x : _aliases) {
                if (ss.tellp() > 0)
                    ss << ',' << sp, sp = "";
                ss << x;
            }
            return ss.str();
        }

        [[nodiscard]] virtual std::string descriptions() const {
            std::stringstream ss;
            ss << _description;
            return ss.str();
        }

        bas &group(const_chars s) {
            if (s) _group = s;
            return (*this);
        }
        [[nodiscard]] virtual std::string group_name() const {
            std::stringstream ss;
            ss << _group;
            return ss.str();
        }

    public:
        [[nodiscard]] std::string const &description() const {
            if (_description.empty())
                return _desc_long;
            return _description;
        }
        // [[nodiscard]] std::string const &examples() const {
        //     return _examples;
        // }
        bas &description(const_chars desc, const_chars long_desc = nullptr, const_chars examples = nullptr) {
            if (desc)
                _description = desc;
            if (long_desc)
                _desc_long = long_desc;
            if (examples)
                _examples = examples;
            return (*this);
        }

        bas &titles(const_chars title_long) {
            if (title_long) this->_long = title_long;
            return (*this);
        }

        // bas &titles(const_chars title_long, const_chars title_short) {
        //     if (title_long) this->_long = title_long;
        //     if (title_short) this->_short = title_short;
        //     return (*this);
        // }

        template<typename... T>
        bas &titles(const_chars title_long, const_chars title_short, T... title_aliases) {
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
        bas &aliases(T... title_aliases) {
            (this->_aliases.push_back(title_aliases), ...);
            // if (sizeof...(title_aliases) > 0) {
            //     // append_to_vector(_aliases, title_aliases...);
            //     for (auto &&x : {title_aliases...}) {
            //         this->_aliases.push_back(x);
            //     }
            // }
            return (*this);
        }
    }; // class bas


    /**
     * @brief A flag, such as: '--help', ....
     */
    class arg : public bas {
    protected:
        // support_types _default;
        vars::streamable_any _default;
        string_array _env_vars;
        std::string _placeholder;
        std::string _toggle_group;

        // friend class app;

    public:
        arg() = default;
        ~arg() override = default;
        arg(const arg &o)
            : bas(o) { _copy(o); }
        arg &operator=(const arg &o) {
            _copy(o);
            return (*this);
        }
        //arg(const arg &o) = delete;
        //arg &operator=(const arg &o) = delete;
        arg(arg &&o) noexcept = default;
        template<typename A, typename... Args,
                 std::enable_if_t<
                         std::is_constructible<vars::streamable_any, A, Args...>::value &&
                                 !std::is_same<std::decay_t<A>, arg>::value,
                         int> = 0>
        explicit arg(A &&a0, Args &&...args)
            : _default(std::forward<A>(a0), std::forward<Args>(args)...) {}
        template<typename A,
                 std::enable_if_t<!std::is_same<std::decay_t<A>, vars::streamable_any>::value &&
                                          !std::is_same<std::decay_t<A>, arg>::value,
                                  int> = 0>
        explicit arg(A &&v)
            : _default(std::forward<A>(v)) {}
        // explicit arg(vars::streamable_any &&v)
        //     : _default(std::move(v)) {}

    protected:
        void _copy(const arg &o) {
            bas::_copy(o);

            __COPY(_env_vars);
            __COPY(_toggle_group);
            __COPY(_placeholder);
            __COPY(_default);
        }

    public:
#undef PROP_SET
#undef PROP_SET2
#undef PROP_SET3
#define PROP_SET(mn)         \
    arg &mn(const_chars s) { \
        if (s) _##mn = s;    \
        return (*this);      \
    }                        \
    std::string const &mn() const { return _##mn; }
#define PROP_SET2(mn)                \
    arg &title_##mn(const_chars s) { \
        if (s) _##mn = s;            \
        return (*this);              \
    }                                \
    std::string const &title_##mn() const { return _##mn; }
#define PROP_SET3(mn, typ)     \
    arg &_##mn(const typ &s) { \
        _##mn = s;             \
        return (*this);        \
    }                          \
    typ const &mn() const { return _##mn; }

        // PROP_SET3(env_vars, string_array)
        // PROP_SET(toggle_group)
        PROP_SET(placeholder)
        // PROP_SET(default)

#undef PROP_SET
#undef PROP_SET2
#undef PROP_SET3

    public:
        [[nodiscard]] std::string title() const override {
            std::stringstream ss;
            if (!_long.empty()) {
                ss << '-' << '-' << _long;
                if (!_placeholder.empty()) {
                    ss << '=' << _placeholder;
                }
            }
            if (!_short.empty()) {
                if (ss.tellp() > 0)
                    ss << ',' << ' ';
                ss << '-' << _short;
            }
            auto sp = " ";
            for (auto &x : _aliases) {
                if (ss.tellp() > 0)
                    ss << ',' << sp, sp = "";
                ss << '-' << '-' << x;
            }
            return ss.str();
        }

        [[nodiscard]] std::string descriptions() const override {
            std::stringstream ss;
            ss << _description;
            return ss.str();
        }

        [[nodiscard]] std::string group_name() const override {
            std::stringstream ss;
            if (_group.empty())
                ss << _toggle_group;
            else
                ss << _group;
            return ss.str();
        }

    public:
        [[nodiscard]] virtual std::string defaults() const {
            std::stringstream ss;
            ss << ' ' << '[' << "DEFAULT=" << _default << ']';
            // if (!std::holds_alternative<std::monostate>(_default)) {
            //     ss << ' ' << '[' << "DEFAULT=" << variant_to_string(_default) << ']';
            // }
            return ss.str();
        }

        [[nodiscard]] virtual const std::string &toggle_group_name() const {
            return _toggle_group;
        }

        [[nodiscard]] virtual bool is_toggleable() const { return !_toggle_group.empty(); }

    public:
        arg &default_value(const support_types &v) {
            _default = v;
            return (*this);
        }
        arg &toggle_group(const_chars s) {
            if (s) _toggle_group = s;
            if (!_group.empty()) _group = _toggle_group;
            return (*this);
        }

        template<typename... T>
        arg &env_vars(T... args) {
            (this->_env_vars.push_back(args), ...);
            return (*this);
        }
        [[nodiscard]] const string_array &env_vars_get() const { return _env_vars; }

    }; // class arg

} // namespace cmdr::opt

#endif //CMDR_CXX11_CMDR_ARG_HH
