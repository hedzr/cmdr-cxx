//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_OPT_HH
#define CMDR_CXX11_CMDR_OPT_HH


namespace cmdr::opt {

    namespace opts {
        class opt : public obj {
            arg _arg;

        public:
            opt() = default;

            virtual ~opt() = default;

        public:
            virtual option operator()() const {
                return [this](cmd &a) {
                    a += _arg;
                };
            }

            [[nodiscard]] virtual option get() const {
                return [this](cmd &a) {
                    a += _arg;
                };
            }

        public:
            opt &titles(const_chars title_long) {
                _arg.titles(title_long);
                return (*this);
            }

            opt &titles(const_chars title_long, const_chars title_short) {
                _arg.titles(title_long, title_short);
                return (*this);
            }

            template<typename... T>
            opt &titles(const_chars title_long, const_chars title_short, T... aliases) {
                _arg.titles(title_long, title_short, aliases...);
                return (*this);
            }

            opt &description(const_chars desc, const_chars long_desc = nullptr, const_chars examples = nullptr) {
                _arg.description(desc, long_desc, examples);
                return (*this);
            }

            opt &placeholder(const_chars s) {
                _arg.placeholder(s);
                return (*this);
            }

            opt &default_value(const support_types &v) {
                _arg.default_value(v);
                return (*this);
            }
        };

        class cmd_base : public obj {
            cmd _cmd;

        public:
            cmd_base() = default;

            virtual ~cmd_base() = default;

        public:
            virtual option operator()() const {
                return [this](cmd &a) {
                    a += _cmd;
                };
            }

            [[nodiscard]] virtual option get() const {
                return [this](cmd &a) {
                    a += _cmd;
                };
            }

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

            //            cmd_base &add_opt(opt &&opt_) {
            //                opt_(_cmd);
            //                return (*this);
            //            }

            cmd_base &option(const option &opt_) {
                _cmd.option(opt_);
                return (*this);
            }
        };
    } // namespace opts

    class opt_dummy : public opts::opt {
    public:
        opt_dummy() = default;

        ~opt_dummy() override = default;
    };

    class opt_subcmd : public opts::cmd_base {
    public:
        opt_subcmd() = default;

        ~opt_subcmd() override = default;
    };

    class opt_int : public opts::opt {
    public:
    };

    class opt_string : public opts::opt {
    public:
    };

} // namespace cmdr::opt

#endif //CMDR_CXX11_CMDR_OPT_HH
