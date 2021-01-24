//
// Created by Hedzr Yeh on 2021/1/21.
//

#ifndef CMDR_CXX11_CMDR_CMN_HH
#define CMDR_CXX11_CMDR_CMN_HH

#include <list>
#include <unordered_map>

namespace cmdr::opt {
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

    namespace details {

        //
        // parsing and hitting
        //

        enum Action {
            RequestHelpScreen = 1,
            OK = 0,
        };

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
        class opt_base;
        class cmd_base;
    } // namespace opts

} // namespace cmdr::opt

#endif //CMDR_CXX11_CMDR_CMN_HH
