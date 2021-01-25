//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_APP_HH
#define CMDR_CXX11_CMDR_APP_HH

#include "cmdr_arg.hh"
#include "cmdr_cmd.hh"
#include "cmdr_cmn.hh"
#include "cmdr_opts.hh"
#include "cmdr_terminal.hh"


namespace cmdr::opt {

    class arg;

    class app;

    class app : public cmd {
        std::string _name;
        std::string _version;
        std::string _author;
        std::string _copyright;
        std::string _header;
        // std::string _description;
        std::string _tail_line;
        // std::string _examples;

        //
        cmdr::opt::vars::store _store;

        // static colorize &colorizer() {...}

    private:
        app() = default;

        app(const_chars name, const_chars version, const_chars author = nullptr,
            const_chars copyright = nullptr, const_chars description_ = nullptr,
            const_chars examples = nullptr);

        app(const app &o);
        app &operator=(const app &o);

    public:
        ~app() override = default;

    public:
#undef PROP_SET
#define PROP_SET(mn)         \
    app &mn(const_chars s) { \
        if (s)               \
            _##mn = s;       \
        return (*this);      \
    }

        PROP_SET(name)
        PROP_SET(version)
        PROP_SET(author)
        PROP_SET(copyright)
        PROP_SET(header)
        // PROP_SET(description)
        // PROP_SET(examples)

#undef PROP_SET

        [[nodiscard]] cmdr::opt::vars::store const &store() const { return _store; }
        cmdr::opt::vars::store &store() { return _store; }

    public:
        /**
         *
         * @return new instance of app
         */
        static app create(const_chars name, const_chars version,
                          const_chars author = nullptr,
                          const_chars copyright = nullptr,
                          const_chars description = nullptr,
                          const_chars examples = nullptr);

        //        app &option(const option &opt) {
        //            opt(*this);
        //            return (*this);
        //        }

        int run(int argc, char *argv[]) override;

        void post_run() const override {
            if (std::current_exception() != nullptr) {
                handle_eptr(std::current_exception());
            } else {
                if (help_hit()) {
                    //
                } else {
                    //
                }
            }
        }

        const bool treat_unknown_input_command_as_error = true;
        const bool treat_unknown_input_flag_as_error = true;

    private:
        struct parsing_pkg {
            cmd *_root;
            std::string title{};
            int index{};
            bool is_flag{false};
            bool help_requesting{false};
            bool passthru_flag{false};
            details::arg_pointers matched_flags{};
            details::cmd_pointers matched_commands{};
            string_array unknown_flags{};
            string_array unknown_commands{};
            string_array non_commands{};

            [[nodiscard]] cmd &curr_command() {
                if (matched_commands.empty())
                    return *_root;
                return *matched_commands.back();
            }
            [[nodiscard]] cmd &last_matched_cmd() {
                if (matched_commands.empty())
                    return null_command();
                return *matched_commands.back();
            }
            [[nodiscard]] arg &last_matched_flg() {
                if (matched_flags.empty())
                    return null_arg();
                return *matched_flags.back();
            }
        };
        struct cmd_matching_result {
            bool matched{};
            bool should_abort{};
            cmd *obj;
            std::exception e;
        };
        struct arg_matching_result {
            bool matched{};
            bool should_abort{};
            arg *obj;
            std::exception e;
        };

        static string_array remain_args(parsing_pkg &pp, char *argv[], int i, int argc);
        static string_array remain_args(char *argv[], int i, int argc);

        details::Action process_command(parsing_pkg &pp, int argc, char *argv[]);
        details::Action process_long_flag(parsing_pkg &pp, int argc, char *argv[]);
        details::Action process_short_flag(parsing_pkg &pp, int argc, char *argv[]);

        cmd_matching_result matching_command(parsing_pkg &pp);
        static arg_matching_result matching_long_flag(parsing_pkg &pp);
        static arg_matching_result matching_short_flag(parsing_pkg &pp);

        details::Action on_unknown_command_found(parsing_pkg &pp, cmd_matching_result &cmr);
        details::Action on_unknown_long_flag_found(parsing_pkg &pp, arg_matching_result &fmr);
        details::Action on_unknown_short_flag_found(parsing_pkg &pp, arg_matching_result &fmr);

        int invoke_command(cmd& cc, string_array remain_args, parsing_pkg& pp);

        void handle_eptr(std::exception_ptr eptr) const {
            try {
                if (eptr) {
                    std::rethrow_exception(eptr);
                }
            } catch (const std::exception &e) {
                std::cout << "Caught exception \"" << e.what() << "\"\n";
            }
        }

    public:
        [[maybe_unused]] void dummy() {}

    public:
        app &operator+(const arg &a) override;
        app &operator+=(const arg &a) override;
        app &operator+(const cmd &a) override;
        app &operator+=(const cmd &a) override;

    public:
        [[nodiscard]] bool help_hit() const { return _help_hit > 0; }
        [[nodiscard]] cmd *command_hit() const { return _cmd_hit; }

    public:
        void print_usages(cmd *start = nullptr);

    private:
        static void print_cmd(std::ostream &ss, cmdr::terminal::colors::colorize &c, cmd *cc,
                       std::string const &app_name, std::string const &exe_name);

        void initialize_internal_commands();
        void add_global_options(cmdr::opt::app &cli);
        void add_generator_menu(cmdr::opt::app &cli);

    private:
        int _help_hit{};
        cmd *_cmd_hit{nullptr};
    };

} // namespace cmdr::opt

#endif // CMDR_CXX11_CMDR_APP_HH
