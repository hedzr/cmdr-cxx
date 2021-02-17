//
// Created by Hedzr Yeh on 2021/1/21.
//

#ifndef CMDR_CXX11_CMDR_CMN_HH
#define CMDR_CXX11_CMDR_CMN_HH

#include "cmdr_dbg.hh"
#include <list>
#include <unordered_map>


namespace cmdr {
    class app;

    namespace opt {
        class arg;
        class cmd;
    } // namespace opt

    namespace types {
        using on_arg_added = std::function<void(opt::arg *)>;
        using on_cmd_added = std::function<void(opt::cmd *)>;
        using on_arg_matched = std::function<void(opt::arg *)>;
        using on_cmd_matched = std::function<void(opt::cmd *)>;

        // using exceptions if loading failed...
        using on_loading_externals = std::function<void(app &cli)>;
        using on_post_run = std::function<void(app const &cli)>;
    } // namespace types
} // namespace cmdr

// namespace cmdr::opt::vars {
//     template<class V = support_types>
//     class store;
// } // namespace cmdr::opt::vars

//
// Core
//
namespace cmdr::opt {

    class arg;

    namespace types {
        using arg_list = std::list<arg>;
        using arg_pointers = std::list<arg *>;
        using grouped_arg_list = std::unordered_map<std::string, arg_pointers>; // key: group-name
        using indexed_args = std::unordered_map<std::string, arg *>;            // key: long-title
    }                                                                           // namespace types

    class cmd;

    namespace types {
        using cmd_list = std::list<cmd>;
        using cmd_pointers = std::list<cmd *>;
        using grouped_cmd_list = std::unordered_map<std::string, cmd_pointers>; // key: group-name
        using indexed_commands = std::unordered_map<std::string, cmd *>;        // key: long-title
        using option = std::function<void(cmd &)>;                              // void (*option)(app &a);
    }                                                                           // namespace types

} // namespace cmdr::opt


namespace cmdr::vars {
    class variable;
} // namespace cmdr::vars


//
// API for developers
//
namespace cmdr::opt {

    class sub_cmd;

    class opt;

    //
    // parsing and hitting
    //

    enum Action {
        OK = 0,

        // the action between OK and Continue would like to request cmdr
        // internal features, such as: print help screen, ...

        RequestHelpScreen = 10,
        RequestBuildInfoScreen,
        RequestVersionsScreen,
        RequestTreeScreen,
        RequestManualScreen,

        RequestDebugInfoScreen = 20,

        Continue = 31,

        // the action beyond (and) Abortion will notify cmdr parsing processor
        // to exit itself right now.

        Abortion = 33,

        // and, RunDefaultAction is a special action especially for
        // unknown_command_found handlers.
        // Your handlers could be return this value and cmdr's
        // processor will invoke the default logic.
        RunDefaultAction,
    };


    class cmdr_requests_exception : public cmdr::exception::cmdr_exception {
    public:
        // using cmdr_exception::cmdr_exception;
        explicit cmdr_requests_exception(enum Action action, char const *file = __FILE__, int line = __LINE__)
            : cmdr_exception(file, line, "req")
            , _action(action) {
        }
        enum Action _action;
    };


    class cmd;
    class arg;


    namespace types {

        using on_unknown_argument_found =
                std::function<Action(
                        std::string &title,
                        cmdr::opt::cmd const &last_matched_cmd,
                        bool long_or_short,
                        bool cmd_or_arg)>;

        /**
         * @brief return negative will abort the parsing and the command invoking later.
         *
         * NOTE the return value will be discarded and the OS will got 0 for
         * a termination normally. If you wanna pass the OS return code,
         * using [exit(n)] in C-API.
         */
        using on_command_hit =
                std::function<Action(cmdr::opt::cmd const &c,
                                     string_array const &remain_args)>; // callback handler

        /**
         * @brief return negative will abort the parsing and the command invoking later.
         *
         * NOTE the return value will be discarded and the OS will got 0 for
         * a termination normally. If you wanna pass the OS return code,
         * using [exit(n)] in C-API.
         */
        using on_flag_hit =
                std::function<Action(cmdr::opt::cmd const &c,
                                     cmdr::opt::arg const &f,
                                     string_array const &remain_args)>; // callback handler

        //
        // invoke a command finally
        //

        /**
         * @brief return non-0 will cancel the on_invoke().
         *
         * But, on_post_invoke() will be always invoked before app terminating.
         *
         * NOTE the return value will be discarded and the OS will got 0 for
         * a termination normally. If you wanna pass the OS return code,
         * using [exit(n)] in C-API.
         */
        using on_pre_invoke =
                std::function<int(cmdr::opt::cmd const &c,
                                  string_array const &remain_args)>; // callback handler
        /**
         * @brief the return value will be transferred to OS.
         */
        using on_invoke =
                std::function<int(cmdr::opt::cmd const &c,
                                  string_array const &remain_args)>; // callback handler
        /**
         * @brief NOTE that on_post_invoke() will be always invoked before app terminating.
         *
         * NOTE the return value will be discarded and the OS will got 0 for
         * a termination normally. If you wanna pass the OS return code,
         * using [exit(n)] in C-API.
         */
        using on_post_invoke =
                std::function<int(cmdr::opt::cmd const &c,
                                  string_array const &remain_args)>; // callback handler


        struct parsing_context {
            cmd *_root;                                 // root command, i.e. app
            std::string title{};                        // the input fragment text (one arg) in command-line
            std::string title_fragment{};               // the matching pieces in title
            int index{};                                // index to argv
            bool is_flag{false};       //
            bool passthru_flag{false}; //
            int matching_flag_type{};                   // short: 0, long: 1, special: 2, ...
            std::size_t pos{};                          // start position of title

            explicit parsing_context(cmd *a)
                : _root(a) {}

        private:
            arg_pointers matched_flags{};
            cmd_pointers _matched_commands{};
            string_array unknown_flags{};
            string_array unknown_commands{};
            string_array non_commands{};
            typedef std::unordered_map<arg *, std::shared_ptr<vars::variable>> val_map;
            val_map _values_map;

        public:
            [[nodiscard]] cmd &curr_command();
            [[nodiscard]] cmd &last_matched_cmd();
            [[nodiscard]] arg &last_matched_flg();

            parsing_context &mft(int mft) {
                is_flag = true;
                matching_flag_type = mft;
                return (*this);
            }

            void add_matched_cmd(cmd *obj) { _matched_commands.push_back(obj); }
            void add_matched_arg(arg *obj) { matched_flags.push_back(obj); }
            void add_matched_arg(arg *obj, std::shared_ptr<vars::variable> const &v);
            void add_unknown_cmd(std::string const &obj) { unknown_commands.push_back(obj); }
            void add_unknown_arg(std::string const &obj) { unknown_flags.push_back(obj); }
            void add_remain_arg(std::string const &arg) { non_commands.push_back(arg); }
            [[nodiscard]] string_array const &remain_args() const { return non_commands; }
            auto &matched_commands() { return _matched_commands; }
        };

        using on_internal_action = std::function<int(parsing_context &pc, int argc, char *argv[])>;

    } // namespace types

} // namespace cmdr::opt

#endif //CMDR_CXX11_CMDR_CMN_HH
