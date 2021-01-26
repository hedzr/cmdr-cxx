//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_APP_HH
#define CMDR_CXX11_CMDR_APP_HH

#include <utility>

#include "cmdr_arg.hh"
#include "cmdr_cmd.hh"
#include "cmdr_cmn.hh"
#include "cmdr_opts.hh"
#include "cmdr_terminal.hh"


namespace cmdr {


    // template<class V = support_types>
    class app : public opt::cmd {
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

        /**
         * @brief reset all internal parsing states and preparing for new command-line parsing.
         */
        void reset();

        int run(int argc, char *argv[]) override;

        void post_run() const override {
            if (std::current_exception() != nullptr) {
                handle_eptr(std::current_exception());
            } else {
                // if (help_hit()) {
                //     //
                // } else {
                //     //
                // }
            }
        }

        void print_usages(cmd *start = nullptr);

    public:
        [[maybe_unused]] void dummy() {}
        //[[nodiscard]] bool help_hit() const { return _help_hit > 0; }
        //[[nodiscard]] cmd *command_hit() const { return _cmd_hit; }

    public:
        app &set_global_pre_invoke_handler(opt::details::on_pre_invoke cb) {
            _global_on_pre_invoke = std::move(cb);
            return (*this);
        }
        app &set_global_post_invoke_handler(opt::details::on_post_invoke cb) {
            _global_on_post_invoke = std::move(cb);
            return (*this);
        }
        app &treat_unknown_input_command_as_error(bool b) {
            _treat_unknown_input_command_as_error = b;
            return (*this);
        }
        app &treat_unknown_input_flag_as_error(bool b) {
            _treat_unknown_input_flag_as_error = b;
            return (*this);
        }

    private:
        struct parsing_context {
            opt::cmd *_root;
            std::string title{};
            int index{}; // argv's index
            bool is_flag{false};
            bool passthru_flag{false};
            int matching_flag_type{}; // short: 0, long: 1, special: 2, ...
            std::size_t pos{};        // start position of title

            explicit parsing_context(app *a)
                : _root(a) {}

        private:
            opt::details::arg_pointers matched_flags{};
            opt::details::cmd_pointers matched_commands{};
            string_array unknown_flags{};
            string_array unknown_commands{};
            string_array non_commands{};

        public:
            [[nodiscard]] opt::cmd &curr_command() {
                if (matched_commands.empty())
                    return *_root;
                return *matched_commands.back();
            }
            [[nodiscard]] opt::cmd &last_matched_cmd() {
                if (matched_commands.empty())
                    return null_command();
                return *matched_commands.back();
            }
            [[nodiscard]] opt::arg &last_matched_flg() {
                if (matched_flags.empty())
                    return null_arg();
                return *matched_flags.back();
            }

            void add_matched_cmd(opt::cmd *obj) { matched_commands.push_back(obj); }
            void add_matched_arg(opt::arg *obj) { matched_flags.push_back(obj); }
            void add_unknown_cmd(std::string const &obj) { unknown_commands.push_back(obj); }
            void add_unknown_arg(std::string const &obj) { unknown_flags.push_back(obj); }
            void add_remain_arg(std::string const &arg) { non_commands.push_back(arg); }
            [[nodiscard]] string_array const &remain_args() const { return non_commands; }
            auto &mc() { return matched_commands; }
            void reverse_foreach_matched_commands(std::function<void(opt::details::cmd_pointers::value_type &it)> f) {
                std::for_each(matched_commands.rbegin(), matched_commands.rend(), f);
            }
        };
        struct cmd_matching_result {
            bool matched{};
            bool should_abort{};
            opt::cmd *obj;
            std::exception e;
        };
        struct arg_matching_result {
            bool matched{};
            bool should_abort{};
            opt::arg *obj;
            int matched_length{};
            std::string matched_str{};
            std::exception e;
        };

        static string_array remain_args(parsing_context &pc, char *argv[], int i, int argc);
        static string_array remain_args(char *argv[], int i, int argc);

        opt::Action process_command(parsing_context &pc, int argc, char *argv[]);
        opt::Action process_special_flag(parsing_context &pc, int argc, char *argv[]);
        opt::Action process_long_flag(parsing_context &pc, int argc, char *argv[]);
        opt::Action process_short_flag(parsing_context &pc, int argc, char *argv[]);

        static cmd_matching_result matching_command(parsing_context &pc);
        static arg_matching_result matching_special_flag(parsing_context &pc);
        static arg_matching_result matching_long_flag(parsing_context &pc);
        static arg_matching_result matching_short_flag(parsing_context &pc);
        static arg_matching_result matching_flag_on(parsing_context &pc,
                                                    bool is_long, bool is_special,
                                                    std::function<opt::details::indexed_args const &(cmd *)> li);

        opt::Action unknown_command_found(parsing_context &pc, cmd_matching_result &cmr);
        opt::Action unknown_long_flag_found(parsing_context &pc, arg_matching_result &amr);
        opt::Action unknown_short_flag_found(parsing_context &pc, arg_matching_result &amr);

        void initialize_internal_commands();
        void register_actions();
        static void add_global_options(app &cli);
        static void add_generator_menu(app &cli);

        void prepare();
        int after_run(opt::Action rc, parsing_context &pc, int argc, char *argv[]);
        int internal_action(opt::Action rc, parsing_context &pc, int argc, char *argv[]);
        int invoke_command(cmd &cc, string_array remain_args, parsing_context &pc);

        static void handle_eptr(std::exception_ptr eptr) {
            try {
                if (eptr) {
                    std::rethrow_exception(eptr);
                }
            } catch (const std::exception &e) {
                std::cout << "Caught exception \"" << e.what() << "\"\n";
            }
        }

        static void fatal_exit(const std::string &msg);

        void print_cmd(std::ostream &ss,
                       cmdr::terminal::colors::colorize &c, cmd *cc,
                       std::string const &app_name, std::string const &exe_name);

        static int print_debug_info_screen(parsing_context &pc, int argc, char *argv[]);
        static int print_manual_screen(parsing_context &pc, int argc, char *argv[]);
        int print_tree_screen(parsing_context &pc, int argc, char *argv[]);

    public:
        app &operator+(const opt::arg &a) override;
        app &operator+=(const opt::arg &a) override;
        app &operator+(const opt::cmd &a) override;
        app &operator+=(const opt::cmd &a) override;

    private:
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

        opt::details::on_pre_invoke _global_on_pre_invoke;
        opt::details::on_post_invoke _global_on_post_invoke;
        bool _treat_unknown_input_command_as_error = true;
        bool _treat_unknown_input_flag_as_error = true;
        opt::details::on_unknown_argument_found _on_unknown_argument_found;

        //int _help_hit{};
        //cmd *_cmd_hit{};
        std::unordered_map<opt::Action,
                           std::function<int(parsing_context &pc, int argc, char *argv[])>>
                _internal_actions{};

        int _minimal_tab_width{43};
        static bool _longest_first;
    };

    inline bool app::_longest_first = true;

} // namespace cmdr

#endif // CMDR_CXX11_CMDR_APP_HH
