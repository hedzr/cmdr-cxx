//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_IMPL_HH
#define CMDR_CXX11_CMDR_IMPL_HH

#include <exception>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>


#include "cmdr_app.hh"
#include "cmdr_arg.hh"
#include "cmdr_cmd.hh"
#include "cmdr_cmn.hh"
#include "cmdr_internals.hh"
#include "cmdr_terminal.hh"


namespace cmdr::opt {


    inline string_array app::remain_args(parsing_context &pc, char *argv[], int i, int argc) {
        string_array a;
        for (auto &it : pc.remain_args()) {
            a.push_back(it);
        }
        for (; i < argc; i++) {
            a.push_back(argv[i]);
        }
        return a;
    }

    inline string_array app::remain_args(char *argv[], int i, int argc) {
        string_array a;
        for (; i < argc; i++) {
            a.push_back(argv[i]);
        }
        return a;
    }

    inline details::Action app::process_command(app::parsing_context &pc, int argc, char *argv[]) {
        auto cmr = matching_command(pc);
        if (cmr.matched) {
            cmr.obj->hit_title(pc.title.c_str());
            pc.add_matched_cmd(cmr.obj);
            if (cmr.obj->on_command_hit()) {
                auto rc = cmr.obj->on_command_hit()(
                        pc.last_matched_cmd(),
                        remain_args(argv, pc.index + 1, argc));
                // if (rc < details::OK || rc >= details::Abortion)
                return rc;
            }
            return details::Continue;
        }
        if (cmr.should_abort)
            return details::Abortion;
        pc.add_unknown_cmd(argv[pc.index]);
        if (_treat_unknown_input_command_as_error) {
            return unknown_command_found(pc, cmr);
        }
        return details::Continue;
    }

    inline details::Action app::process_special_flag(app::parsing_context &pc, int argc, char *argv[]) {
        pc.title = pc.title.substr(2);
        auto amr = matching_special_flag(pc);
        if (amr.matched) {
            amr.obj->hit_title(pc.title.c_str());
            pc.add_matched_arg(amr.obj);
            if (amr.obj->on_flag_hit()) {
                auto rc = amr.obj->on_flag_hit()(
                        pc.last_matched_cmd(),
                        pc.last_matched_flg(),
                        remain_args(argv, pc.index + 1, argc));
                // if (rc < details::OK || rc >= details::Abortion)
                return rc;
            }
            return details::Continue;
        }
        if (amr.should_abort)
            return details::Abortion;
        pc.title = argv[pc.index];
        pc.add_unknown_arg(pc.title);
        if (_treat_unknown_input_flag_as_error) {
            return unknown_long_flag_found(pc, amr);
        }
        return details::Continue;
    }

    inline details::Action app::process_long_flag(app::parsing_context &pc, int argc, char *argv[]) {
        pc.title = pc.title.substr(2);
        auto amr = matching_long_flag(pc);
        if (amr.matched) {
            amr.obj->hit_title(pc.title.c_str());
            pc.add_matched_arg(amr.obj);
            if (amr.obj->on_flag_hit()) {
                auto rc = amr.obj->on_flag_hit()(
                        pc.last_matched_cmd(),
                        pc.last_matched_flg(),
                        remain_args(argv, pc.index + 1, argc));
                return rc;
            }
            return details::Continue;
        }

        if (amr.should_abort)
            return details::Abortion;

        pc.title = argv[pc.index];
        pc.add_unknown_arg(pc.title);
        if (_treat_unknown_input_flag_as_error) {
            return unknown_long_flag_found(pc, amr);
        }
        return details::Continue;
    }

    inline details::Action app::process_short_flag(app::parsing_context &pc, int argc, char *argv[]) {
        pc.title = pc.title.substr(1);
    next_combined:
        auto amr = matching_short_flag(pc);
        if (amr.matched) {
            amr.obj->hit_title(amr.matched_str.c_str());
            pc.add_matched_arg(amr.obj);
            if (amr.obj->on_flag_hit()) {
                auto rc = amr.obj->on_flag_hit()(
                        pc.last_matched_cmd(),
                        pc.last_matched_flg(),
                        remain_args(argv, pc.index + 1, argc));
                if (rc < details::OK || rc >= details::Abortion)
                    return rc;

                pc.pos += amr.matched_length;
                if (pc.pos < pc.title.length())
                    goto next_combined;

                return rc;
            }

            pc.pos += amr.matched_length;
            if (pc.pos < pc.title.length())
                goto next_combined;

            return details::Continue;
        }

        if (amr.should_abort)
            return details::Abortion;

        pc.title = argv[pc.index];
        pc.add_unknown_arg(pc.title);
        if (_treat_unknown_input_flag_as_error) {
            return unknown_short_flag_found(pc, amr);
        }
        return details::Continue;
    }

    inline app::cmd_matching_result app::matching_command(app::parsing_context &pc) {
        cmd_matching_result cmr;
        cmd &c = pc.curr_command();
        details::indexed_commands &li = c._long_commands;
        if (auto const &it = li.find(pc.title); it != li.end()) {
            cmr.matched = true;
            cmr.obj = (cmd *) &(it->second->update_hit_count(pc.title, 1, true));
        } else {
            li = c._short_commands;
            auto const &it1 = li.find(pc.title);
            if (it1 != li.end()) {
                cmr.matched = true;
                cmr.obj = (cmd *) &(it->second->update_hit_count(pc.title, 1, false));
            }
        }
        return cmr;
    }

    inline app::arg_matching_result
    app::matching_flag_on(app::parsing_context &pc,
                          bool is_long, bool is_special,
                          std::function<details::indexed_args const &(cmd *)> li) {
        arg_matching_result amr;
        pc.reverse_foreach_matched_commands([=, &amr](auto &it) {
            // auto d = li(it);
            // if (auto const &itz = d.find(pc.title); itz != d.end()) {
            //     amr.matched = true;
            //     amr.obj = (arg *) &(itz->second->update_hit_count(pc.title, 1, is_long, is_special));
            //     return;
            // }

            auto z = li(it);
            // std::unordered_map<std::string, arg *> z{};
            for (auto itz = z.begin(); itz != z.end(); itz++) {
                if (string::has_prefix(pc.title, itz->first)) {
                    amr.matched = true;
                    amr.matched_length = itz->first.length();
                    amr.matched_str = itz->first;
                    amr.obj = (arg *) &(itz->second->update_hit_count(pc.title.substr(0, itz->first.length()), 1, is_long, is_special));
                    return;
                }
            }
        });
        return amr;
    }

    inline app::arg_matching_result app::matching_special_flag(app::parsing_context &pc) {
        return matching_flag_on(pc, true, true, [](cmd *c) -> details::indexed_args & {
            return c->_long_args;
        });
    }

    inline app::arg_matching_result app::matching_long_flag(app::parsing_context &pc) {
        return matching_flag_on(pc, true, false, [](cmd *c) -> details::indexed_args & {
            return c->_long_args;
        });
    }

    inline app::arg_matching_result app::matching_short_flag(app::parsing_context &pc) {
        return matching_flag_on(pc, false, false, [](cmd *c) -> details::indexed_args & {
            return c->_short_args;
        });
    }

    inline details::Action app::unknown_long_flag_found(app::parsing_context &pc, arg_matching_result &amr) {
        unused(pc);
        unused(amr);
        if (_on_unknown_argument_found)
            if (auto rc = _on_unknown_argument_found(pc.title, pc.last_matched_cmd(), true, false); rc != details::RunDefaultAction)
                return rc;
        std::cerr << "Unknown long flag: " << std::quoted(pc.title);
        auto &c = pc.last_matched_cmd();
        if (c.valid())
            std::cerr << " under matched command: " << std::quoted(c.title());
        std::cerr << '\n';
        return details::Abortion;
    }

    inline details::Action app::unknown_short_flag_found(app::parsing_context &pc, arg_matching_result &amr) {
        unused(pc);
        unused(amr);
        if (_on_unknown_argument_found)
            if (auto rc = _on_unknown_argument_found(pc.title, pc.last_matched_cmd(), false, false); rc != details::RunDefaultAction)
                return rc;
        std::cerr << "Unknown short flag: " << std::quoted(pc.title);
        auto &c = pc.last_matched_cmd();
        if (c.valid())
            std::cerr << " under matched command: " << std::quoted(c.title());
        std::cerr << '\n';
        return details::Abortion;
    }

    inline details::Action app::unknown_command_found(parsing_context &pc, cmd_matching_result &cmr) {
        unused(cmr);
        if (_on_unknown_argument_found)
            if (auto rc = _on_unknown_argument_found(pc.title, pc.last_matched_cmd(), false, true); rc != details::RunDefaultAction)
                return rc;
        std::cerr << "Unknown command: " << std::quoted(pc.title);
        auto &c = pc.last_matched_cmd();
        if (c.valid())
            std::cerr << " under matched command: " << std::quoted(c.title());
        std::cerr << '\n';
        return details::Abortion;
    }

    inline int app::invoke_command(cmd &c, string_array remain_args, parsing_context &pc) {
        unused(pc);

        int rc{0};
        if (_global_on_pre_invoke)
            rc = _global_on_pre_invoke(c, remain_args);
        if (rc == 0)
            if (c.on_pre_invoke())
                rc = c.on_pre_invoke()(c, remain_args);
        try {
            if (rc == 0) {
                if (c.on_invoke())
                    rc = c.on_invoke()(c, remain_args);
                else {
                    std::cout << "INVOKE: " << std::quoted(c.title()) << ".\n";
#if defined(_DEBUG)
                    // store.root().dump_full_keys(std::cout);
                    _store.root().dump_tree(std::cout);
#endif
                }
            }
        } catch (...) {
            if (c.on_post_invoke())
                c.on_post_invoke()(c, remain_args);
            if (_global_on_post_invoke)
                _global_on_post_invoke(c, remain_args);

            std::exception_ptr eptr = std::current_exception();
            if (eptr) {
                std::rethrow_exception(eptr);
            }
        }
        return rc;
    }

    inline void app::print_cmd(std::ostream &ss, cmdr::terminal::colors::colorize &c, cmd *cc, std::string const &app_name, std::string const &exe_name) {
        if (!cc->description().empty()) {
            ss << std::endl
               << "Description" << std::endl
               << string::pad_left(cc->description()) << std::endl;
        }
        if (!cc->examples().empty()) {
            ss << std::endl
               << "Examples" << std::endl
               << string::pad_left(string::reg_replace(cc->examples(), "~", exe_name)) << std::endl;
        }
        ss << std::endl
           << "Usage" << std::endl;
        ss << string::pad_left(exe_name, 2) << " [commands] [options] [Tail Args]" << std::endl;

        ss << std::endl
           << "Commands" << std::endl;

        std::ostringstream os1;
        cc->print_commands(os1, c, _minimal_tab_width, true, 0);

        std::ostringstream os2;
        auto trivial = cc;
        auto saved_minimal_tab_width = _minimal_tab_width;
        do {
            std::ostringstream tt;
            if (trivial == cc)
                tt << "Options";
            else if (trivial->owner()) {
                tt << "Parent Options of " << std::quoted(trivial->title());
            } else {
                tt << "Global Options";
            }

            os2 << std::endl
                << tt.str() << std::endl;
            trivial->print_flags(os2, c, _minimal_tab_width, true, 0);
        } while ((trivial = trivial->owner()) != nullptr);

        if (saved_minimal_tab_width < _minimal_tab_width) {
            std::ostringstream os3;
            cc->print_commands(os3, c, _minimal_tab_width, true, 0);
            ss << os3.str() << os2.str();
        } else {
            ss << os1.str() << os2.str();
        }

        unused(app_name);
        unused(exe_name);
    }

    inline void app::print_usages(cmd *start) {
        std::string exe_name = path::executable_name();
        auto c = cmdr::terminal::colors::colorize::create();

        std::stringstream ss;
        ss << _name << ' ' << 'v' << _version;
        if (_header.empty())
            ss << " by " << _author << '.' << ' ' << _copyright << std::endl;
        else
            ss << _header << std::endl;

        print_cmd(ss, c, start ? start : this, _name, exe_name);

        if (!_tail_line.empty()) {
            ss << std::endl
               << _tail_line << std::endl;
        } else {
            ss << std::endl
               << "Type `" << exe_name << " --help` to get help screen (this screen)." << std::endl;
        }

        std::cout << ss.str();
    }

    inline void app::reset() {
        // todo reset all internal states so that we can restart a new session for parsing.
    }

    inline int app::run(int argc, char *argv[]) {
        // std::cout << "Hello, World!" << std::endl;

        parsing_context pc{this};
        pc.add_matched_cmd(this);
        details::Action rc;

        for (int i = 1; i < argc; i++) {
            pc.title = argv[i];
            pc.index = i;
            pc.pos = 0;

            if (pc.title[0] == '~' && pc.title[1] == '~') {
                // special flags
                rc = process_special_flag(pc, argc, argv);
                if (rc < details::OK || rc >= details::Abortion)
                    return 0;
                continue;
            }

            if (pc.title[0] == '-') {
                // flag?
                if (pc.title[1] == '-') {
                    if (pc.title[2] == 0) {
                        pc.passthru_flag = true;
                        break;
                    }

                    // long
                    rc = process_long_flag(pc, argc, argv);
                    if (rc < details::OK || rc >= details::Abortion)
                        return 0;
                    continue;
                }

                // short flag
                rc = process_short_flag(pc, argc, argv);
                if (rc < details::OK || rc >= details::Abortion)
                    return 0;
                continue;
            }

            // command
            auto &c = pc.last_matched_cmd();
            if (c.valid()) {
                if (c.no_sub_commands()) {
                    pc.add_remain_arg(pc.title);
                    continue;
                }
            }
            rc = process_command(pc, argc, argv);
            if (rc < details::OK || rc >= details::Abortion)
                return 0;
        }

        return after_run(rc, pc, argc, argv);
    }

    inline int app::after_run(details::Action rc, parsing_context &pc, int argc, char *argv[]) {
        if (rc > details::OK && rc < details::Continue)
            return internal_action(rc, pc, argc, argv);

        if (auto cc = pc.last_matched_cmd(); cc.valid()) {
            if (cc.no_sub_commands()) {
                // invoking cc
                return invoke_command(cc, remain_args(pc, argv, pc.index + 1, argc), pc);
            }

            print_usages(&cc);
        } else {
            print_usages(&pc.curr_command());
        }
        return 0;
    }

    inline int app::internal_action(details::Action rc, parsing_context &pc, int argc, char *argv[]) {
        if (auto it = _internal_actions.find(rc); it != _internal_actions.end())
            return it->second(pc, argc, argv);
        return 0;
    }

} // namespace cmdr::opt


#endif //CMDR_CXX11_CMDR_IMPL_HH
