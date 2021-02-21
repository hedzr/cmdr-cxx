//
// Created by Hedzr Yeh on 2021/1/12.
//

#ifndef CMDR_CXX11_CMDR_CMD_HH
#define CMDR_CXX11_CMDR_CMD_HH

#include "cmdr_cmn.hh"

#include "cmdr_arg.hh"

#include "cmdr_dbg.hh"
#include "cmdr_terminal.hh"


namespace cmdr::opt {


    /**
     * @brief A command or sub-command
     */
    class cmd : public bas {
    protected:
        friend class cmdr::app;

        types::arg_list _all_args{};
        types::grouped_arg_list _grouped_args{{UNSORTED_GROUP, types::arg_pointers{}}};
        types::indexed_args _indexed_args{}; // just long-titles
        types::indexed_args _short_args{};
        types::indexed_args _long_args{};
        types::hit_toggle_groups _hit_toggle_groups{};
        types::toggle_groups _toggle_groups{};
        types::cmd_list _all_commands{};
        types::grouped_cmd_list _grouped_commands{{UNSORTED_GROUP, types::cmd_pointers{}}};
        types::indexed_commands _indexed_commands{}; // just long-titles
        types::indexed_commands _short_commands{};
        types::indexed_commands _long_commands{};

        cmd *_last_added_command{};
        arg *_last_added_arg{};

        types::on_command_hit _on_command_hit;
        types::on_pre_invoke _on_pre_invoke;
        types::on_invoke _on_invoke;
        types::on_post_invoke _on_post_invoke;
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
            __COPY(_hit_toggle_groups);
            __COPY(_toggle_groups);

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

        // PROP_SET3(hit_toggle_groups, types::hit_toggle_groups)

        PROP_SET3(on_command_hit, types::on_command_hit)
        PROP_SET3(on_pre_invoke, types::on_pre_invoke)
        PROP_SET3(on_invoke, types::on_invoke)
        PROP_SET3(on_post_invoke, types::on_post_invoke)
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

        std::string title_sequences() const;

        void toggle_group_set(std::string const &key, arg *a);
        std::string const &toggle_group(std::string const &key) const;

    public:
        void add(arg const &a);
        void add(cmd const &a);
        virtual cmd &operator+(arg const &a);
        virtual cmd &operator+=(arg const &a);
        virtual cmd &operator+(cmd const &a);
        virtual cmd &operator+=(cmd const &a);

        // friend cmd &operator+(cmd &lhs, const opts::cmd_base &rhs);
        // friend cmd &operator+=(cmd &lhs, const opts::cmd_base &rhs);
        // friend cmd &operator+(cmd &lhs, const opts::opt_base &rhs);
        // friend cmd &operator+=(cmd &lhs, const opts::opt_base &rhs);

        virtual cmd &operator+(const sub_cmd &rhs);
        virtual cmd &operator+=(const sub_cmd &rhs);
        virtual cmd &operator+(const opt &rhs);
        virtual cmd &operator+=(const opt &rhs);

        /**
         * @brief return the matched arg/flag object or null_arg if not found.
         * To ensure the return obj is a valid one, use [#ar.valid()]
         * @param long_title
         * @return
         */
        virtual arg &operator[](const_chars long_title);
        /**
         * @brief return the matched arg/flag object or null_arg if not found.
         * To ensure the return obj is a valid one, use [#arg.valid()]
         * @param long_title
         * @return
         */
        virtual const arg &operator[](const_chars long_title) const;

        /**
         * @brief return the matched sub-command object or null_command if not found.
         * To ensure the return obj is a valid one, use [#cmd.valid()]
         * @param long_title
         * @return
         */
        virtual cmd &operator()(const_chars long_title, bool extensive = false);
        /**
         * @brief return the matched sub-command object or null_command if not found.
         * To ensure the return obj is a valid one, use [#cmd.valid()]
         * @param long_title
         * @return
         */
        virtual const cmd &operator()(const_chars long_title, bool extensive = false) const;

        static cmd &null_command() {
            static cmd c;
            return c;
        }
        static arg &null_arg() {
            static arg c;
            return c;
        }

    protected:
        arg &find_flag(const_chars long_title, bool extensive = false);
        cmd &find_command(const_chars long_title, bool extensive = false);
        arg const *find_flag(std::string const &long_title, bool extensive = false) const;
        cmd const *find_command(std::string const &long_title, bool extensive = false) const;

    public:
        virtual cmd &opt(const types::option &opt_) {
            opt_(*this);
            return (*this);
        }
        virtual cmd &option(const types::option &opt_) {
            opt_(*this);
            return (*this);
        }

        virtual int run(int argc, char *argv[]);

        virtual void post_run() const {}

    public:
        // wt: total width for description part.
        void print_commands(std::ostream &ss, cmdr::terminal::colors::colorize &c, int &wt, bool grouped, bool show_hidden_items, bool shell_completion_mode, int level = -1) const;
        // wt: total width for description part.
        void print_flags(std::ostream &ss, cmdr::terminal::colors::colorize &c, int &wt, bool grouped, bool show_hidden_items, bool shell_completion_mode, int level = -1) const;

        void walk_args(std::function<void(arg &)> const &cb);
    }; // class cmd

} // namespace cmdr::opt


namespace cmdr::exception {

    class dup_short_cmd_found : public dup_error {
        cmdr::opt::cmd const *_o;
        cmdr::opt::cmd const *_c;

    public:
        using dup_error::dup_error;
        [[maybe_unused]] explicit dup_short_cmd_found(const char *file, int line, opt::cmd const *c, opt::cmd const *owner);
    };

    class dup_long_cmd_found : public dup_error {
        cmdr::opt::cmd const *_o;
        cmdr::opt::cmd const *_c;

    public:
        using dup_error::dup_error;
        explicit dup_long_cmd_found(const char *file, int line, opt::cmd const *c, opt::cmd const *owner);
    };

    class dup_alias_cmd_found : public dup_error {
        cmdr::opt::cmd const *_o;
        cmdr::opt::cmd const *_c;

    public:
        using dup_error::dup_error;
        explicit dup_alias_cmd_found(const char *file, int line, const char *title, opt::cmd const *c, opt::cmd const *owner);
    };

    class dup_short_flag_found : public dup_error {
        cmdr::opt::arg const *_o;
        cmdr::opt::cmd const *_c;

    public:
        using dup_error::dup_error;
        explicit dup_short_flag_found(const char *file, int line, opt::arg const *a, opt::cmd const *c);
    };

    class dup_long_flag_found : public dup_error {
        cmdr::opt::arg const *_o;
        cmdr::opt::cmd const *_c;

    public:
        using dup_error::dup_error;
        explicit dup_long_flag_found(const char *file, int line, opt::arg const *a, opt::cmd const *c);
    };

    class dup_alias_flag_found : public dup_error {
        cmdr::opt::arg const *_o;
        cmdr::opt::cmd const *_c;

    public:
        using dup_error::dup_error;
        explicit dup_alias_flag_found(const char *file, int line, const char *title, opt::arg const *a, opt::cmd const *c);
    };

} // namespace cmdr::exception

#endif //CMDR_CXX11_CMDR_CMD_HH
