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
        typedef std::unordered_map<std::string, arg_pointers> grouped_arg_list;
        typedef std::unordered_map<std::string, arg *> indexed_args;
    } // namespace details

    class cmd;

    namespace details {
        typedef std::list<cmd> cmd_list;
        typedef std::list<cmd *> cmd_pointers;
        typedef std::unordered_map<std::string, cmd_pointers> grouped_cmd_list; // key: group-name
        typedef std::unordered_map<std::string, cmd *> indexed_commands;        // key: long-title
        typedef std::function<void(cmd &)> option;                              // void (*option)(app &a);
    }                                                                           // namespace details

    namespace opts {
        class opt_base;
        class cmd_base;
    } // namespace opts

} // namespace cmdr::opt

#endif //CMDR_CXX11_CMDR_CMN_HH
