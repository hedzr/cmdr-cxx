//
// Created by Hedzr Yeh on 2021/1/12.
//

#ifndef CMDR_CXX11_CMDR_CMD_HH
#define CMDR_CXX11_CMDR_CMD_HH

#include "cmdr_arg.hh"
#include "cmdr_cmn.hh"
#include "cmdr_terminal.hh"


namespace cmdr::opt {


    /**
     * @brief A command or sub-command
     */
    class cmd : public bas {
    protected:
        friend class cmdr::app;

        details::arg_list _all_args{};
        details::grouped_arg_list _grouped_args{{UNSORTED_GROUP, details::arg_pointers{}}};
        details::indexed_args _indexed_args{}; // just long-titles
        details::indexed_args _short_args{};
        details::indexed_args _long_args{};
        details::cmd_list _all_commands{};
        details::grouped_cmd_list _grouped_commands{{UNSORTED_GROUP, details::cmd_pointers{}}};
        details::indexed_commands _indexed_commands{}; // just long-titles
        details::indexed_commands _short_commands{};
        details::indexed_commands _long_commands{};

        cmd *_last_added_command{};
        arg *_last_added_arg{};

        details::on_command_hit _on_command_hit;
        details::on_pre_invoke _on_pre_invoke;
        details::on_invoke _on_invoke;
        details::on_post_invoke _on_post_invoke;
        // std::string _hit_title;

    public:
        cmd() = default;
        ~cmd() override = default;
        cmd(const cmd &o)
            : bas(o) { _copy(o); }
        cmd &operator=(const cmd &o) {
            if (this == &o)
                return *this;
            _copy(o);
            return (*this);
        }

        void _copy(const cmd &o) {
            bas::_copy(o);

            __COPY(_all_args);
            __COPY(_grouped_args);
            __COPY(_indexed_args);
            __COPY(_short_args);
            __COPY(_long_args);

            __COPY(_all_commands);
            __COPY(_grouped_commands);
            __COPY(_indexed_commands);
            __COPY(_short_commands);
            __COPY(_long_commands);

            __COPY(_last_added_command);
            __COPY(_last_added_arg);

            __COPY(_on_command_hit);
            __COPY(_on_pre_invoke);
            __COPY(_on_invoke);
            __COPY(_on_post_invoke);

            // __COPY(_hit_title);
        }

    public:
#undef PROP_SET
#undef PROP_SET2
#undef PROP_SET3
#define PROP_SET(mn)         \
    cmd &mn(const_chars s) { \
        if (s) _##mn = s;    \
        return (*this);      \
    }                        \
    std::string const &mn() const { return _##mn; }
#define PROP_SET2(mn)                \
    cmd &title_##mn(const_chars s) { \
        if (s) _##mn = s;            \
        return (*this);              \
    }                                \
    const std::string &title_##mn() const { return _##mn; }
#define PROP_SET3(mn, typ)  \
    cmd &mn(const typ &s) { \
        _##mn = s;          \
        return (*this);     \
    }                       \
    typ const &mn() const { return _##mn; }

        PROP_SET3(on_command_hit, details::on_command_hit)
        PROP_SET3(on_pre_invoke, details::on_pre_invoke)
        PROP_SET3(on_invoke, details::on_invoke)
        PROP_SET3(on_post_invoke, details::on_post_invoke)
        // PROP_SET(hit_title)

#undef PROP_SET
#undef PROP_SET2
#undef PROP_SET3

        [[nodiscard]] cmd *last_added_command() const { return _last_added_command; }
        [[nodiscard]] arg *last_added_arg() const { return _last_added_arg; }

    public:
        static bool is_leading_switch_char(const_chars flag) { return (flag[0] == '-' || flag[0] == '/'); }
        static bool is_leading_switch_char(const std::string &flag) { return (flag[0] == '-' || flag[0] == '/'); }

        [[nodiscard]] bool no_sub_commands() const { return _all_commands.empty(); }

    public:
        virtual cmd &operator+(arg const &a);
        virtual cmd &operator+=(arg const &a);
        virtual cmd &operator+(cmd const &a);
        virtual cmd &operator+=(cmd const &a);

        // friend cmd &operator+(cmd &lhs, const opts::cmd_base &rhs);
        // friend cmd &operator+=(cmd &lhs, const opts::cmd_base &rhs);
        // friend cmd &operator+(cmd &lhs, const opts::opt_base &rhs);
        // friend cmd &operator+=(cmd &lhs, const opts::opt_base &rhs);

        friend cmd &operator+(cmd &lhs, const sub_cmd &rhs);
        friend cmd &operator+=(cmd &lhs, const sub_cmd &rhs);
        friend cmd &operator+(cmd &lhs, const opt &rhs);
        friend cmd &operator+=(cmd &lhs, const opt &rhs);

        /**
         * @brief return the matched arg/flag object or null_arg if not found.
         * To ensure the return obj is a valid one, use [#ar.valid()]
         * @param long_title
         * @return
         */
        arg &operator[](const_chars long_title);
        /**
         * @brief return the matched arg/flag object or null_arg if not found.
         * To ensure the return obj is a valid one, use [#arg.valid()]
         * @param long_title
         * @return
         */
        const arg &operator[](const_chars long_title) const;

        /**
         * @brief return the matched sub-command object or null_command if not found.
         * To ensure the return obj is a valid one, use [#cmd.valid()]
         * @param long_title
         * @return
         */
        cmd &operator()(const_chars long_title, bool extensive = false);
        /**
         * @brief return the matched sub-command object or null_command if not found.
         * To ensure the return obj is a valid one, use [#cmd.valid()]
         * @param long_title
         * @return
         */
        const cmd &operator()(const_chars long_title, bool extensive = false) const;

        static cmd &null_command() {
            static cmd c;
            return c;
        }
        static arg &null_arg() {
            static arg c;
            return c;
        }

    private:
        arg &find_flag(const_chars long_title, bool extensive = false);
        cmd &find_command(const_chars long_title, bool extensive = false);

    public:
        virtual cmd &opt(const details::option &opt_) {
            opt_(*this);
            return (*this);
        }
        virtual cmd &option(const details::option &opt_) {
            opt_(*this);
            return (*this);
        }

        virtual int run(int argc, char *argv[]);

        virtual void post_run() const {}

    public:
        // wt: total width for description part.
        void print_commands(std::ostream &ss, cmdr::terminal::colors::colorize &c, int &wt, bool grouped = true, int level = -1);
        // wt: total width for description part.
        void print_flags(std::ostream &ss, cmdr::terminal::colors::colorize &c, int &wt, bool grouped = true, int level = -1);

        void walk_args(std::function<void(arg &)> const &cb);
    }; // class cmd

} // namespace cmdr::opt


#endif //CMDR_CXX11_CMDR_CMD_HH
