//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_OPT_HH
#define CMDR_CXX11_CMDR_OPT_HH

#include "cmdr_arg.hh"
#include "cmdr_cmd.hh"
#include "cmdr_cmn.hh"

namespace cmdr::opt {

    namespace opts {

        class opt_base : public obj {
        public:
            opt_base() = default;
            ~opt_base() override = default;
            opt_base(opt_base &&o) noexcept = delete;
            opt_base &operator=(const opt_base &o) = delete;
            template<typename A, typename... Args,
                     std::enable_if_t<
                             std::is_constructible<arg, A, Args...>::value &&
                                     !std::is_same<std::decay_t<A>, opt_base>::value,
                             int> = 0>
            explicit opt_base(A &&a0, Args &&...args)
                : _arg(std::forward<A>(a0), std::forward<Args>(args)...) {}
            template<typename A,
                     std::enable_if_t<!std::is_same<std::decay_t<A>, arg>::value &&
                                              !std::is_same<std::decay_t<A>, opt_base>::value,
                                      int> = 0>
            explicit opt_base(A &&v)
                : _arg(std::forward<A>(v)) {}
            //explicit opt_base(arg &&v)
            //    : _arg(std::move(v)) {}

        public:
            virtual details::option operator()() const {
                return [this](cmd &a) {
                    a += _arg;
                };
            }

            [[nodiscard]] virtual details::option get() const {
                return [this](cmd &a) {
                    a += _arg;
                };
            }

            explicit operator arg() const { return _arg; }
            [[nodiscard]] const arg &underlying() const { return _arg; }

        public:
            opt_base &titles(const_chars title_long) {
                _arg.titles(title_long);
                return (*this);
            }

            opt_base &titles(const_chars title_long, const_chars title_short) {
                _arg.titles(title_long, title_short);
                return (*this);
            }

            template<typename... T>
            opt_base &titles(const_chars title_long, const_chars title_short, T... aliases) {
                _arg.titles(title_long, title_short, aliases...);
                return (*this);
            }

            opt_base &description(const_chars desc, const_chars long_desc = nullptr, const_chars examples = nullptr) {
                _arg.description(desc, long_desc, examples);
                return (*this);
            }

            opt_base &group(const_chars s) {
                _arg.group(s);
                return (*this);
            }

            opt_base &toggle_group(const_chars s) {
                _arg.toggle_group(s);
                return (*this);
            }

            opt_base &placeholder(const_chars s) {
                _arg.placeholder(s);
                return (*this);
            }

            opt_base &default_value(support_types const &v) {
                _arg.default_value(v);
                return (*this);
            }

            template<typename... T>
            opt_base &env_vars(T... args) {
                // (this->_env_vars.push_back(title_aliases), ...);
                _arg.env_vars(args...);
                return (*this);
            }

        private:
            arg _arg;
        }; // class opt_base

        class cmd_base : public obj {
        public:
            cmd_base() = default;
            ~cmd_base() override = default;

        public:
            virtual details::option operator()() const {
                return [this](class cmd &a) {
                    a += _cmd;
                };
            }

            [[nodiscard]] virtual details::option get() const {
                return [this](class cmd &a) {
                    a += _cmd;
                };
            }

            explicit operator cmd() const { return _cmd; }
            [[nodiscard]] const cmd &underlying() const { return _cmd; }

        public:
            cmd_base &titles(const_chars title_long) {
                _cmd.titles(title_long);
                return (*this);
            }

            cmd_base &titles(const_chars title_long, const_chars title_short) {
                _cmd.titles(title_long, title_short);
                return (*this);
            }

            template<typename... T>
            cmd_base &titles(const_chars title_long, const_chars title_short, T... aliases) {
                _cmd.titles(title_long, title_short, aliases...);
                return (*this);
            }

            cmd_base &description(const_chars desc, const_chars long_desc = nullptr, const_chars examples = nullptr) {
                _cmd.description(desc, long_desc, examples);
                return (*this);
            }

            cmd_base &group(const_chars s) {
                _cmd.group(s);
                return (*this);
            }

            cmd_base &opt(const details::option &opt_) {
                _cmd.option(opt_);
                return (*this);
            }
            cmd_base &option(const details::option &opt_) {
                _cmd.option(opt_);
                return (*this);
            }

        private:
            class cmd _cmd;
        }; // class cmd_base
    }      // namespace opts


    //
    //
    //


    class opt_dummy : public opts::opt_base {
    public:
        opt_dummy() = default;
        ~opt_dummy() override = default;
    };

    class opt_subcmd : public opts::cmd_base {
    public:
        opt_subcmd() = default;
        ~opt_subcmd() override = default;
    };

    /**
     * @brief opt_new is a generic options definer
     */
    template<class T>
    class opt_new : public opts::opt_base {
    public:
        opt_new() = default;
        explicit opt_new(T const &default_value) {
            this->default_value(default_value);
        }
        ~opt_new() override = default;
    };

    class opt : public opts::opt_base {
    public:
        opt() = default;
        ~opt() override = default;
        template<typename A, typename... Args,
                 std::enable_if_t<
                         std::is_constructible<arg, A, Args...>::value &&
                                 !std::is_same<std::decay_t<A>, opt>::value,
                         int> = 0>
        explicit opt(A &&a0, Args &&...args)
            : opts::opt_base(std::forward<A>(a0), std::forward<Args>(args)...) {}
        explicit opt(arg &&v)
            : opts::opt_base(std::move(v)) {}
    };

#define DEFINE_OPT_BY_TYPE(typ)               \
    class opt_##typ : public opts::opt_base { \
    public:                                   \
        opt_##typ() = default;                \
        ~opt_##typ() override = default;      \
    }

    DEFINE_OPT_BY_TYPE(bool);
    DEFINE_OPT_BY_TYPE(int);
    DEFINE_OPT_BY_TYPE(string);


} // namespace cmdr::opt

#endif //CMDR_CXX11_CMDR_OPT_HH
