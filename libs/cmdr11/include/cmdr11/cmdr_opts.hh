//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_OPT_HH
#define CMDR_CXX11_CMDR_OPT_HH

#include "cmdr_arg.hh"
#include "cmdr_cmd.hh"
#include "cmdr_cmn.hh"

namespace cmdr::opt {

    class app;

    namespace opts {

#if 0
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
        }; //class opt_base
#endif

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

            // friend app &operator+(app &a, const cmd_base &o);
            // friend app &operator+=(app &a, const cmd_base &o);
            // friend cmd &operator+(cmd &a, const cmd_base &o);
            // friend cmd &operator+=(cmd &a, const cmd_base &o);

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

            cmd_base &hidden(bool b = true) {
                _cmd.hidden(b);
                return (*this);
            }

            cmd_base &on_hit(details::on_command_hit const &cb) {
                _cmd.on_command_hit(cb);
                return (*this);
            }
            cmd_base &on_pre_invoke(details::on_pre_invoke const &cb) {
                _cmd.on_pre_invoke(cb);
                return (*this);
            }
            cmd_base &on_invoke(details::on_invoke const &cb) {
                _cmd.on_invoke(cb);
                return (*this);
            }
            cmd_base &on_post_invoke(details::on_post_invoke const &cb) {
                _cmd.on_post_invoke(cb);
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


    class subcmd : public opts::cmd_base {
    public:
        subcmd() = default;
        ~subcmd() override = default;
        // friend app &operator+(app &a, const subcmd &o);
        // friend app &operator+=(app &a, const subcmd &o);
        // friend cmd &operator+(cmd &a, const subcmd &o);
        // friend cmd &operator+=(cmd &a, const subcmd &o);
    };

    /**
     * @brief opt_new is a generic options definer
     */
    template<class T>
    class opt : public obj {
    public:
        opt() = default;
        explicit opt(T const &default_value)
            : _arg(default_value) {}
        ~opt() override = default;

        template<typename A = T, typename... Args,
                 std::enable_if_t<
                         std::is_constructible<arg, A, Args...>::value &&
                                 !std::is_same<std::decay_t<A>, opt>::value,
                         int> = 0>
        explicit opt(A &&a0, Args &&...args)
            : _arg(std::forward<A>(a0), std::forward<Args>(args)...) {}
        template<typename A = T,
                 std::enable_if_t<!std::is_same<std::decay_t<A>, arg>::value &&
                                          !std::is_same<std::decay_t<A>, opt>::value,
                                  int> = 0>
        explicit opt(A &&v)
            : _arg(std::forward<A>(v)) {}
        explicit opt(arg &&v)
            : _arg(std::move(v)) {}

        // friend app &operator+(app &a, const opt &o);
        // friend app &operator+=(app &a, const opt &o);
        // friend cmd &operator+(cmd &a, const opt &o);
        // friend cmd &operator+=(cmd &a, const opt &o);

    public:
        virtual details::option operator()() const {
            return [&](cmd &a) {
                a += _arg;
            };
        }

        [[nodiscard]] virtual details::option get() const {
            return [&](cmd &a) {
                a += _arg;
            };
        }

        explicit operator arg() const { return _arg; }
        [[nodiscard]] const arg &underlying() const { return _arg; }

    public:
        opt &titles(const_chars title_long) {
            _arg.titles(title_long);
            return (*this);
        }

        opt &titles(const_chars title_long, const_chars title_short) {
            _arg.titles(title_long, title_short);
            return (*this);
        }

        template<typename... A>
        opt &titles(const_chars title_long, const_chars title_short, A... aliases) {
            _arg.titles(title_long, title_short, aliases...);
            return (*this);
        }

        opt &description(const_chars desc, const_chars long_desc = nullptr, const_chars examples = nullptr) {
            _arg.description(desc, long_desc, examples);
            return (*this);
        }

        opt &group(const_chars s) {
            _arg.group(s);
            return (*this);
        }

        opt &toggle_group(const_chars s) {
            _arg.toggle_group(s);
            return (*this);
        }

        opt &hidden(bool b = true) {
            _arg.hidden(b);
            return (*this);
        }

        /**
         * @brief a special flag is a 2-tilde-leading options, such as '~~tree'.
         *
         * NOTE that any special flags will be hidden in help screen.
         *
         * @param b
         * @return
         */
        opt &special(bool b = true) {
            _arg.special(b);
            return (*this);
        }
        /**
         * @brief the 2-hyphen-leading option shouldn't be matched.
         * @param b
         * @return
         */
        opt &no_non_special(bool b = true) {
            _arg.no_non_special(b);
            return (*this);
        }

        opt &placeholder(const_chars s) {
            _arg.placeholder(s);
            return (*this);
        }

        // opt_base &default_value(support_types const &v) {
        //     _arg.default_value(v);
        //     return (*this);
        // }
        opt &default_value(vars::streamable_any const &v) {
            _arg.default_value(v);
            return (*this);
        }
        template<typename A>
        opt &default_value(A const &v) {
            _arg.default_value(v);
            return (*this);
        }

        template<typename... A>
        opt &env_vars(A... args) {
            // (this->_env_vars.push_back(title_aliases), ...);
            _arg.env_vars(args...);
            return (*this);
        }

        opt &on_hit(details::on_flag_hit const &cb) {
            _arg.on_flag_hit(cb);
            return (*this);
        }

    private:
        arg _arg;
    }; // class opt


    class opt_dummy : public opt<bool> {
    public:
        opt_dummy() = default;
        ~opt_dummy() override = default;
    };

#if 0
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
#endif
#if 0
#define DEFINE_OPT_BY_TYPE(typ)               \
    class opt_##typ : public opts::opt_base { \
    public:                                   \
        opt_##typ() = default;                \
        ~opt_##typ() override = default;      \
    }

    DEFINE_OPT_BY_TYPE(bool);
    DEFINE_OPT_BY_TYPE(int);
    DEFINE_OPT_BY_TYPE(string);
#endif


} // namespace cmdr::opt

#endif //CMDR_CXX11_CMDR_OPT_HH
