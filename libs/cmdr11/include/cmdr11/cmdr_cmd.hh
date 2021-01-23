//
// Created by Hedzr Yeh on 2021/1/12.
//

#ifndef CMDR_CXX11_CMDR_CMD_HH
#define CMDR_CXX11_CMDR_CMD_HH

#include "cmdr_arg.hh"
#include "cmdr_cmn.hh"

namespace cmdr::opt {

    /**
     * @brief A command or sub-command
     */
    class cmd : public bas {
    protected:
        details::arg_list _all_args{};
        details::grouped_arg_list _grouped_args{{UNSORTED_GROUP, details::arg_pointers{}}};
        details::indexed_args _indexed_args{};
        details::cmd_list _all_commands{};
        details::grouped_cmd_list _grouped_commands{{UNSORTED_GROUP, details::cmd_pointers{}}};
        details::indexed_commands _indexed_commands{};

        cmd *_last_added_command;
        arg *_last_added_arg;

    public:
        cmd() = default;
        ~cmd() override = default;
        cmd(const cmd &o)
            : bas(o) { _copy(o); }
        cmd &operator=(const cmd &o) {
            _copy(o);
            return (*this);
        }

        void _copy(const cmd &o) {
            bas::_copy(o);

            __COPY(_all_args);
            __COPY(_grouped_args);
            __COPY(_indexed_args);
            __COPY(_all_commands);
            __COPY(_grouped_commands);
            __COPY(_indexed_commands);
            __COPY(_last_added_command);
            __COPY(_last_added_arg);
        }

    public:
#undef PROP_SET
#undef PROP_SET2
#undef PROP_SET3
#define PROP_SET(mn)         \
    cmd &mn(const_chars s) { \
        if (s) _##mn = s;    \
        return (*this);      \
    }
#define PROP_SET2(mn)                \
    cmd &title_##mn(const_chars s) { \
        if (s) _##mn = s;            \
        return (*this);              \
    }                                \
    const std::string &title_##mn() const { return _##mn; }
#define PROP_SET3(mn, typ)          \
    cmd &title_##mn(const typ &s) { \
        _##mn = s;                  \
        return (*this);             \
    }

        //

#undef PROP_SET
#undef PROP_SET2
#undef PROP_SET3

        [[nodiscard]] cmd *last_added_command() const { return _last_added_command; }
        [[nodiscard]] arg *last_added_arg() const { return _last_added_arg; }

    public:
        static bool is_leading_switch_char(const_chars flag) { return (flag[0] == '-' || flag[0] == '/'); }
        static bool is_leading_switch_char(const std::string &flag) { return (flag[0] == '-' || flag[0] == '/'); }

    public:
        virtual cmd &operator+(const arg &a);
        virtual cmd &operator+=(const arg &a);
        virtual cmd &operator+(const cmd &a);
        virtual cmd &operator+=(const cmd &a);

        friend cmd &operator+(cmd &lhs, const opts::cmd_base &rhs);
        friend cmd &operator+=(cmd &lhs, const opts::cmd_base &rhs);
        friend cmd &operator+(cmd &lhs, const opts::opt_base &rhs);
        friend cmd &operator+=(cmd &lhs, const opts::opt_base &rhs);

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
        cmd &operator()(const_chars long_title);
        /**
         * @brief return the matched sub-command object or null_command if not found.
         * To ensure the return obj is a valid one, use [#cmd.valid()]
         * @param long_title
         * @return
         */
        const cmd &operator()(const_chars long_title) const;

        static cmd &null_command() {
            static cmd c;
            return c;
        }
        static arg &null_arg() {
            static arg c;
            return c;
        }

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
        void print_commands(std::ostream &ss, bool grouped = true);
        void print_flags(std::ostream &ss, bool grouped = true);

    }; // class cmd

} // namespace cmdr::opt


#endif //CMDR_CXX11_CMDR_CMD_HH
