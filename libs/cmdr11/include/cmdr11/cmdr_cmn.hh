//
// Created by Hedzr Yeh on 2021/1/21.
//

#ifndef CMDR_CXX11_CMDR_CMN_HH
#define CMDR_CXX11_CMDR_CMN_HH

#include <list>
#include <unordered_map>

namespace cmdr {
    class app;

    namespace opt {
        class arg;
        class cmd;
    }
    
    namespace details {
        using on_arg_added = std::function<void(opt::arg*)>;
        using on_cmd_added = std::function<void(opt::cmd*)>;
        using on_arg_matched = std::function<void(opt::arg*)>;
        using on_cmd_matched = std::function<void(opt::cmd*)>;
        
        // using exceptions if loading failed...
        using on_loading_externals = std::function<void(app& cli)>; 
    }
}; // namespace cmdr

// namespace cmdr::opt::vars {
//     template<class V = support_types>
//     class store;
// } // namespace cmdr::opt::vars

//
// Core
//
namespace cmdr::opt {

    class arg;

    namespace details {
        using arg_list = std::list<arg>;
        using arg_pointers = std::list<arg *>;
        using grouped_arg_list = std::unordered_map<std::string, arg_pointers>; // key: group-name
        using indexed_args = std::unordered_map<std::string, arg *>;            // key: long-title
    }                                                                           // namespace details

    class cmd;

    namespace details {
        using cmd_list = std::list<cmd>;
        using cmd_pointers = std::list<cmd *>;
        using grouped_cmd_list = std::unordered_map<std::string, cmd_pointers>; // key: group-name
        using indexed_commands = std::unordered_map<std::string, cmd *>;        // key: long-title
        using option = std::function<void(cmd &)>;                              // void (*option)(app &a);
    }                                                                           // namespace details

} // namespace cmdr::opt


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

    namespace details {

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

    } // namespace details

} // namespace cmdr::opt

#endif //CMDR_CXX11_CMDR_CMN_HH
