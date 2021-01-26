//
// Created by Hedzr Yeh on 2021/1/21.
//

#ifndef CMDR_CXX11_CMDR_CMN_HH
#define CMDR_CXX11_CMDR_CMN_HH

#include <list>
#include <unordered_map>

namespace cmdr {
    class app;
};

namespace cmdr::opt {

    // template<class V = support_types>
    class arg;

    namespace details {
        typedef std::list<arg> arg_list;
        typedef std::list<arg *> arg_pointers;
        typedef std::unordered_map<std::string, arg_pointers> grouped_arg_list; // key: group-name
        typedef std::unordered_map<std::string, arg *> indexed_args;            // key: long-title
    }                                                                           // namespace details

    class cmd;

    namespace details {
        typedef std::list<cmd> cmd_list;
        typedef std::list<cmd *> cmd_pointers;
        typedef std::unordered_map<std::string, cmd_pointers> grouped_cmd_list; // key: group-name
        typedef std::unordered_map<std::string, cmd *> indexed_commands;        // key: long-title
        typedef std::function<void(cmd &)> option;                              // void (*option)(app &a);
    }                                                                           // namespace details

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

    namespace details {

        typedef std::function<Action(
                std::string &title, cmd const &last_matched_cmd,
                bool long_or_short, bool cmd_or_arg)>
                on_unknown_argument_found;

        /**
         * @brief return negative will abort the parsing and the command invoking later.
         *
         * NOTE the return value will be discarded and the OS will got 0 for
         * a termination normally. If you wanna pass the OS return code,
         * using [exit(n)] in C-API.
         */
        typedef std::function<Action(cmd const &c,
                                     string_array const &remain_args)>
                on_command_hit; // callback handler

        /**
         * @brief return negative will abort the parsing and the command invoking later.
         *
         * NOTE the return value will be discarded and the OS will got 0 for
         * a termination normally. If you wanna pass the OS return code,
         * using [exit(n)] in C-API.
         */
        typedef std::function<Action(cmd const &c,
                                     arg const &f,
                                     string_array const &remain_args)>
                on_flag_hit; // callback handler

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
        typedef std::function<int(cmd const &c,
                                  string_array const &remain_args)>
                on_pre_invoke; // callback handler
        /**
         * @brief the return value will be transferred to OS.
         */
        typedef std::function<int(cmd const &c,
                                  string_array const &remain_args)>
                on_invoke; // callback handler
        /**
         * @brief NOTE that on_post_invoke() will be always invoked before app terminating.
         *
         * NOTE the return value will be discarded and the OS will got 0 for
         * a termination normally. If you wanna pass the OS return code,
         * using [exit(n)] in C-API.
         */
        typedef std::function<int(cmd const &c,
                                  string_array const &remain_args)>
                on_post_invoke; // callback handler

    } // namespace details


    namespace opts {
        // class opt_base;
        class cmd_base;
    } // namespace opts

    class subcmd;
    template<class T>
    class opt;


} // namespace cmdr::opt

#endif //CMDR_CXX11_CMDR_CMN_HH
