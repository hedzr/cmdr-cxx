//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_APP_HH
#define CMDR_CXX11_CMDR_APP_HH

#include <utility>

#include "cmdr_dbg.hh"
#include "cmdr_terminal.hh"

#include "cmdr_var_t.hh"

#include "cmdr_cmn.hh"

#include "cmdr_arg.hh"
#include "cmdr_cmd.hh"
#include "cmdr_opts.hh"

namespace cmdr {

    /**
     * @brief an CLI application here.
     * 
     * An app holds all commands and sub-commands and its flags.
     * 
     */
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

        using tcolorize = cmdr::terminal::colors::colorize;

    public:
#undef PROP_SET
#define PROP_SET(mn)         \
    app &mn(const_chars s) { \
        if (s)               \
            _##mn = s;       \
        return (*this);      \
    }                        \
    const std::string &mn() const { return _##mn; }

        PROP_SET(name)
        PROP_SET(version)
        PROP_SET(author)
        PROP_SET(copyright)
        PROP_SET(header)
        PROP_SET(tail_line)
        // PROP_SET(description)
        // PROP_SET(examples)
        PROP_SET(store_prefix)

        [[nodiscard]] std::string const &cli_prefix() const { return _long; }
        void cli_prefix(std::string const &s) { _long = s; }

#undef PROP_SET

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

        void print_usages(opt::cmd const *start = nullptr);

    public:
        [[maybe_unused]] void dummy() {}
        //[[nodiscard]] bool help_hit() const { return _help_hit > 0; }
        //[[nodiscard]] cmd *command_hit() const { return _cmd_hit; }

        [[nodiscard]] vars::store const &store() const { return _store; }
        vars::store &store() { return _store; }

        app &set_match_longest_first(bool b = true) {
            _longest_first = b;
            return (*this);
        }

        app &set_minimal_tab_stop(int width = 43) {
            _minimal_tab_width = width;
            return (*this);
        }

    public:
        app &set_global_pre_invoke_handler(opt::types::on_pre_invoke &&cb) {
            _global_on_pre_invoke = std::move(cb);
            return (*this);
        }
        app &set_global_post_invoke_handler(opt::types::on_post_invoke &&cb) {
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

        app &set_global_on_arg_added(types::on_arg_added &&cb) {
            _on_arg_added.push_back(cb);
            return (*this);
        }
        app &set_global_on_cmd_added(types::on_cmd_added &&cb) {
            _on_cmd_added.push_back(cb);
            return (*this);
        }
        app &set_global_on_arg_matched(types::on_arg_matched &&cb) {
            _on_arg_matched.push_back(cb);
            return (*this);
        }
        app &set_global_on_cmd_matched(types::on_cmd_matched &&cb) {
            _on_cmd_matched.push_back(cb);
            return (*this);
        }
        app &set_global_on_loading_externals(types::on_loading_externals &&cb) {
            _on_loading_externals.push_back(cb);
            return (*this);
        }

        app &set_global_on_command_not_hooked(opt::types::on_invoke const &cb) {
            _on_command_not_hooked = cb;
            return (*this);
        }

        app &set_on_handle_exception_ptr(std::function<void(const std::exception_ptr &eptr)> const &cb) {
            _on_handle_exception_ptr = cb;
            return (*this);
        }

    protected:
        friend class opt::cmd;

        void on_arg_added(opt::arg *a);
        void on_cmd_added(opt::cmd *a);
        void on_arg_matched(opt::arg *a, opt::arg::var_type &value);
        void on_cmd_matched(opt::cmd *a);
        void on_loading_externals();

    private:
        struct parsing_context {
            opt::cmd *_root;
            std::string title{};
            int index{}; // argv's index
            bool is_flag{false};
            bool passthru_flag{false};
            int matching_flag_type{}; // short: 0, long: 1, special: 2, ...
            std::size_t pos{};        // start position of title

            explicit parsing_context(opt::cmd *a)
                : _root(a) {}

        private:
            opt::types::arg_pointers matched_flags{};
            opt::types::cmd_pointers _matched_commands{};
            string_array unknown_flags{};
            string_array unknown_commands{};
            string_array non_commands{};
            typedef std::unordered_map<opt::arg *, opt::arg::var_type> val_map;
            val_map _values_map;

        public:
            [[nodiscard]] opt::cmd &curr_command() {
                if (_matched_commands.empty())
                    return *_root;
                return *_matched_commands.back();
            }
            [[nodiscard]] opt::cmd &last_matched_cmd() {
                if (_matched_commands.empty())
                    return opt::cmd::null_command();
                return *_matched_commands.back();
            }
            [[nodiscard]] opt::arg &last_matched_flg() {
                if (matched_flags.empty())
                    return opt::cmd::null_arg();
                return *matched_flags.back();
            }

            void add_matched_cmd(opt::cmd *obj) { _matched_commands.push_back(obj); }
            void add_matched_arg(opt::arg *obj) { matched_flags.push_back(obj); }
            void add_matched_arg(opt::arg *obj, opt::arg::var_type const &v);
            void add_unknown_cmd(std::string const &obj) { unknown_commands.push_back(obj); }
            void add_unknown_arg(std::string const &obj) { unknown_flags.push_back(obj); }
            void add_remain_arg(std::string const &arg) { non_commands.push_back(arg); }
            [[nodiscard]] string_array const &remain_args() const { return non_commands; }
            auto &matched_commands() { return _matched_commands; }
            // void reverse_foreach_matched_commands(std::function<void(opt::details::cmd_pointers<V>::value_type &it)> f) {
            //     std::for_each(matched_commands.rbegin(), matched_commands.rend(), f);
            // }
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
        opt::Action process_flag(parsing_context &pc, int argc, char *argv[], int leading_chars, std::function<arg_matching_result(parsing_context &)> const &matcher);
        opt::Action process_special_flag(parsing_context &pc, int argc, char *argv[]);
        opt::Action process_long_flag(parsing_context &pc, int argc, char *argv[]);
        opt::Action process_short_flag(parsing_context &pc, int argc, char *argv[]);

        static cmd_matching_result matching_command(parsing_context &pc);
        static arg_matching_result matching_special_flag(parsing_context &pc);
        static arg_matching_result matching_long_flag(parsing_context &pc);
        static arg_matching_result matching_short_flag(parsing_context &pc);
        static arg_matching_result matching_flag_on(parsing_context &pc,
                                                    bool is_long, bool is_special,
                                                    std::function<opt::types::indexed_args const &(opt::cmd *)> const &li);

        opt::Action unknown_command_found(parsing_context &pc, cmd_matching_result &cmr);
        opt::Action unknown_long_flag_found(parsing_context &pc, arg_matching_result &amr);
        opt::Action unknown_short_flag_found(parsing_context &pc, arg_matching_result &amr);

        void initialize_internal_commands();
        void register_actions();
        static void add_global_options(app &cli);
        static void add_generator_menu(app &cli);

        void prepare();
        void prepare_common_env();
        void prepare_env_vars();
        void load_externals();
        void apply_env_vars();
        int after_run(opt::Action rc, parsing_context &pc, int argc, char *argv[]);
        int internal_action(opt::Action rc, parsing_context &pc, int argc, char *argv[]);
        int invoke_command(opt::cmd &cc, string_array const &remain_args, parsing_context &pc);

        void handle_eptr(const std::exception_ptr &eptr) const;

        static void fatal_exit(const char *format) {
            std::cout << format << '\n';
            exit(-1);
        }
        template<class A, class... Args>
        static void fatal_exit(const char *format, A const &a0, Args &&...args) {
            for (; *format != '\0'; format++) {
                if (*format == '%') {
                    std::cout << a0;
                    tprintf(format + 1, args...); // 递归调用
                    return;
                }
                std::cout << *format;
            }
        }

        void print_cmd(std::ostream &ss,
                       tcolorize &c, 
                       opt::cmd const *cc,
                       std::string const &app_name, std::string const &exe_name);

        int print_debug_info_screen(parsing_context &pc, int argc, char *argv[]);
        static int print_manual_screen(parsing_context &pc, int argc, char *argv[]);
        int print_tree_screen(parsing_context &pc, int argc, char *argv[]);

        int on_invoking_print_cmd(opt::cmd const &hit, string_array const &remain_args);

    public:
        app &operator+(opt::arg const &a) override;
        app &operator+=(opt::arg const &a) override;
        app &operator+(opt::cmd const &a) override;
        app &operator+=(opt::cmd const &a) override;

        app &operator+(opt::sub_cmd const &a) override;
        app &operator+=(opt::sub_cmd const &a) override;
        app &operator+(opt::opt const &a) override;
        app &operator+=(opt::opt const &a) override;


        opt::arg &operator[](const_chars long_title) override { return find_flag(long_title); }
        const opt::arg &operator[](const_chars long_title) const override { return const_cast<app *>(this)->find_flag(long_title); }

        opt::arg &operator[](const std::string &long_title) { return find_flag(long_title.c_str()); }
        const opt::arg &operator[](const std::string &long_title) const { return const_cast<app *>(this)->find_flag(long_title.c_str()); }


        opt::cmd &operator()(const_chars long_title, bool extensive = false) override { return find_command(long_title, extensive); }
        const opt::cmd &operator()(const_chars long_title, bool extensive = false) const override { return const_cast<app *>(this)->find_command(long_title, extensive); }

        opt::cmd &operator()(const std::string &long_title, bool extensive = false) { return find_command(long_title.c_str(), extensive); }
        const opt::cmd &operator()(const std::string &long_title, bool extensive = false) const { return const_cast<app *>(this)->find_command(long_title.c_str(), extensive); }


        /**
         * @brief get the value without store-prefix from Option Store.
         * 
         * The store-prefix can be get or set by store_prefix()/store_prefix(prefix).
         * The default store-prefix is 'app'.
         * 
         * To retrieve a item value from Option Store directly, use 
         * cmdr::get_store().get_raw()/get_raw_p(), which functions needs to pass a
         * full key with any prefix.
         * 
         * For example, `get_raw("app.someone") == get("someone")`, in which store-prefix is 'app'..
         * 
         * @param key 
         * @return 
         */
        vars::variable const &get(char const *key) const { return _store.get_raw_p(_store_prefix.c_str(), key); }
        vars::variable &get(char const *key) { return _store.get_raw_p(_store_prefix.c_str(), key); }
        [[nodiscard]] vars::variable const &get(std::string const &key) const { return _store.get_raw_p(_store_prefix, key); }
        [[nodiscard]] vars::variable &get(std::string const &key) { return _store.get_raw_p(_store_prefix, key); }

        /**
         * @brief get the commands/flags' value from Option Store.
         * 
         * The value (or default-value) of a flag is been stored in Option Store with a prefix 'app.cli'.
         * You can change the prefix by cli_prefix(prefix) and get it by cli_prefix().
         * 
         * @param key 
         * @return 
         */
        vars::variable const &get_for_cli(char const *key) const { return _store.get_raw_p(_long.c_str(), key); }
        vars::variable &get_for_cli(char const *key) { return _store.get_raw_p(_long.c_str(), key); }
        [[nodiscard]] vars::variable const &get_for_cli(std::string const &key) const { return _store.get_raw_p(_long, key); }
        [[nodiscard]] vars::variable &get_for_cli(std::string const &key) { return _store.get_raw_p(_long, key); }

    public:
        template<class A, typename... Args,
                 std::enable_if_t<
                         std::is_constructible<vars::variable, A, Args...>::value &&
                                 !std::is_same<std::decay_t<A>, vars::variable>::value &&
                                 !std::is_same<std::decay_t<A>, app>::value,
                         int> = 0>
        void set(char const *key, A &&a0, Args &&...args) {
            _store.set_raw_p(store_prefix().c_str(), key, a0, args...);
        }
        template<class A,
                 std::enable_if_t<std::is_constructible<vars::variable, A>::value &&
                                          !std::is_same<std::decay_t<A>, vars::variable>::value &&
                                          !std::is_same<std::decay_t<A>, app>::value,
                                  int> = 0>
        void set(char const *key, A &&a) {
            _store.set_raw_p(store_prefix().c_str(), key, a);
        }
        void set(char const *key, vars::variable &&a);
        void set(char const *key, vars::variable const &a);


        void dump_tree_f(std::ostream &os = std::cout,
                         std::function<bool(std::pair<vars::store::key_type, vars::store::node_pointer> const &)> const &on_filter = nullptr,
                         const_chars leading_title = nullptr,
                         vars::store::node *start = nullptr) const {
            _store.dump_tree_f(os, on_filter, leading_title, start);
        }
        void dump_full_keys_f(std::ostream &os = std::cout,
                              std::function<bool(std::pair<vars::store::key_type, vars::store::node_pointer> const &)> const &on_filter = nullptr,
                              const_chars leading_title = nullptr,
                              vars::store::node *start = nullptr) const {
            _store.dump_full_keys_f(os, on_filter, leading_title, start);
        }

        void dump_tree(std::ostream &os = std::cout,
                       const_chars leading_title = nullptr,
                       vars::store::node *start = nullptr) const {
            _store.dump_tree(os, leading_title, start);
        }
        void dump_full_keys(std::ostream &os = std::cout,
                            const_chars leading_title = nullptr,
                            vars::store::node *start = nullptr) const {
            _store.dump_full_keys(os, leading_title, start);
        }

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
        cmdr::vars::store _store;
        std::string _store_prefix{DEFAULT_KEY_PREFIX};

        // static colorize &colorizer() {...}

        opt::types::on_pre_invoke _global_on_pre_invoke;
        opt::types::on_post_invoke _global_on_post_invoke;
        bool _treat_unknown_input_command_as_error = true;
        bool _treat_unknown_input_flag_as_error = true;
        opt::types::on_unknown_argument_found _on_unknown_argument_found;

        //int _help_hit{};
        //cmd *_cmd_hit{};
        std::unordered_map<opt::Action,
                           std::function<int(parsing_context &pc, int argc, char *argv[])>>
                _internal_actions{};

        int _minimal_tab_width{-1};
        static bool _longest_first;

        std::vector<types::on_arg_added> _on_arg_added;
        std::vector<types::on_cmd_added> _on_cmd_added;
        std::vector<types::on_arg_matched> _on_arg_matched;
        std::vector<types::on_cmd_matched> _on_cmd_matched;
        std::vector<types::on_loading_externals> _on_loading_externals;
        opt::types::on_invoke _on_command_not_hooked;
        std::function<void(const std::exception_ptr &eptr)> _on_handle_exception_ptr;
    };

    inline bool app::_longest_first = true;

} // namespace cmdr

#endif // CMDR_CXX11_CMDR_APP_HH
