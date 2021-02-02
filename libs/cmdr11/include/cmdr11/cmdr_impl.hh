//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_IMPL_HH
#define CMDR_CXX11_CMDR_IMPL_HH

#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>

#include "cmdr_log.hh"
#include "cmdr_terminal.hh"

#include "cmdr_cmn.hh"

#include "cmdr_app.hh"
#include "cmdr_arg.hh"
#include "cmdr_cmd.hh"
#include "cmdr_internals.hh"
#include "cmdr_opts.hh"


namespace cmdr {


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

    inline opt::Action app::process_command(parsing_context &pc, int argc, char *argv[]) {
        auto cmr = matching_command(pc);
        if (cmr.matched) {
            cmr.obj->hit_title(pc.title.c_str());
            pc.add_matched_cmd(cmr.obj);
            get_app().on_cmd_matched(cmr.obj);
            if (cmr.obj->on_command_hit()) {
                auto rc = cmr.obj->on_command_hit()(
                        pc.last_matched_cmd(),
                        remain_args(argv, pc.index + 1, argc));
                // if (rc < details::OK || rc >= details::Abortion)
                return rc;
            }
            return opt::Continue;
        }
        if (cmr.should_abort)
            return opt::Abortion;
        pc.add_unknown_cmd(argv[pc.index]);
        if (_treat_unknown_input_command_as_error) {
            return unknown_command_found(pc, cmr);
        }
        return opt::Continue;
    }

    inline opt::Action
    app::process_flag(parsing_context &pc, int argc, char *argv[], int leading_chars,
                      std::function<arg_matching_result(parsing_context &)> const &matcher) {
        pc.title = pc.title.substr(leading_chars);
        bool value_parsed{};
        bool extra_argv;
        auto rc{opt::Continue};
    next_combined:
        auto amr = matcher(pc);
        if (amr.matched) {
            assert(amr.obj);

            // std::cout << " - " << amr.matched_str << ' ' << '(' << amr.obj->dotted_key() << ')';
            verbose_debug(" - %s (%s)", amr.matched_str.c_str(), amr.obj->dotted_key().c_str());

            amr.obj->hit_title(pc.title.c_str());

            if (auto &typ = amr.obj->default_value().type();
                typ != typeid(bool) && typ != typeid(void)) {

                // try solving the following input as value of this matched arg.
                auto remains = pc.title.substr(pc.pos + amr.matched_length);
                auto remains_orig_len = remains.length();
                if (remains[0] == '=') {
                    remains = remains.substr(1);
                }
                remains = string::strip_quotes(remains);
                if (remains.empty() && pc.index + 1 < argc) {
                    remains = argv[++pc.index];
                    extra_argv = true;
                }

                std::stringstream sst(remains);
                vars::variable &val = amr.obj->default_value();
                auto xb = sst.tellg();
                sst >> val;
                auto xe = sst.tellg();
                int read = (int) (xe - xb);
                value_parsed = read > 0 || sst.eof();
                if (!value_parsed) {
                    if (extra_argv)
                        --pc.index; // restore the parsing pointer
                } else if (read > 0)
                    amr.matched_length += read;
                else if (!extra_argv)
                    amr.matched_length += remains_orig_len;

                pc.add_matched_arg(amr.obj, val);
                // std::cout << " -> " << val;
                verbose_debug("   -> value: %s", val.as_string().c_str());

            } else {
                pc.add_matched_arg(amr.obj);
                value_parsed = true;
            }

            // std::cout << std::endl;

            get_app().on_arg_matched(amr.obj);
            if (amr.obj->on_flag_hit()) {
                rc = amr.obj->on_flag_hit()(
                        pc.last_matched_cmd(),
                        pc.last_matched_flg(),
                        remain_args(argv, pc.index + 1, argc));
                if (rc < opt::OK || rc >= opt::Abortion)
                    return rc;
            }

            pc.pos += amr.matched_length;

            if (value_parsed) {
                if (pc.pos < pc.title.length())
                    goto next_combined;
                return rc;
            }

            // cannot parse the arg's value, ignore it and continue for the next arg
            std::cerr << "cannot parse the value of the flag " << std::quoted(amr.matched_str) << '\n';
            return opt::Continue;
        }

        if (amr.should_abort)
            return opt::Abortion;

        pc.title = argv[pc.index];
        pc.add_unknown_arg(pc.title);
        if (_treat_unknown_input_flag_as_error) {
            return unknown_long_flag_found(pc, amr);
        }
        return opt::Continue;
    }

    inline opt::Action app::process_special_flag(parsing_context &pc, int argc, char *argv[]) {
        return process_flag(pc, argc, argv, 2, matching_special_flag);
    }

    inline opt::Action app::process_long_flag(parsing_context &pc, int argc, char *argv[]) {
        return process_flag(pc, argc, argv, 2, matching_long_flag);
    }

    inline opt::Action app::process_short_flag(parsing_context &pc, int argc, char *argv[]) {
        return process_flag(pc, argc, argv, 1, matching_short_flag);
    }

    inline typename app::cmd_matching_result app::matching_command(parsing_context &pc) {
        cmd_matching_result cmr;
        auto &c = pc.curr_command();
        opt::details::indexed_commands &li = c._long_commands;
        if (auto const &it = li.find(pc.title); it != li.end()) {
            cmr.matched = true;
            auto p2 = it->second;
            cmr.obj = (opt::cmd *) &(p2->update_hit_count(pc.title, 1, true));
        } else {
            li = c._short_commands;
            auto const &it1 = li.find(pc.title);
            if (it1 != li.end()) {
                cmr.matched = true;
                auto p2 = it1->second;
                cmr.obj = (opt::cmd *) &(p2->update_hit_count(pc.title, 1, false));
            }
        }
        return cmr;
    }

    inline typename app::arg_matching_result
    app::matching_flag_on(parsing_context &pc,
                          bool is_long, bool is_special,
                          std::function<opt::details::indexed_args const &(opt::cmd *)> li) {
        arg_matching_result amr;
        auto &mc = pc.matched_commands();

        for (auto it = mc.rbegin(); it != mc.rend(); it++) {
            auto ptr = (*it);
            auto title = pc.title.substr(pc.pos);
        lookup_parents:
            auto z = li(ptr);
            // std::unordered_map<std::string, arg *> z{};
            for (auto &itz : z) {
                if (string::has_prefix(title, itz.first)) {
                    if (_longest_first) {
                        if (itz.first.length() > (std::size_t) amr.matched_length) {
                            amr.matched = true;
                            amr.matched_length = itz.first.length();
                            amr.matched_str = itz.first;
                            amr.obj = (cmdr::opt::arg *) &(itz.second->update_hit_count(title.substr(0, itz.first.length()), 1, is_long, is_special));
                        }
                    } else {
                        amr.matched = true;
                        amr.matched_length = itz.first.length();
                        amr.matched_str = itz.first;
                        amr.obj = (cmdr::opt::arg *) &(itz.second->update_hit_count(title.substr(0, itz.first.length()), 1, is_long, is_special));
                        break;
                    }
                }
            }

            if (ptr->owner()) {
                ptr = ptr->owner();
                goto lookup_parents;
            }
        }
        return amr;
    }

    inline typename app::arg_matching_result
    app::matching_special_flag(parsing_context &pc) {
        return matching_flag_on(pc, true, true, [](opt::cmd *c) -> opt::details::indexed_args const & {
            return c->_long_args;
        });
    }

    inline typename app::arg_matching_result
    app::matching_long_flag(parsing_context &pc) {
        return matching_flag_on(pc, true, false, [](opt::cmd *c) -> opt::details::indexed_args const & {
            return c->_long_args;
        });
    }

    inline typename app::arg_matching_result
    app::matching_short_flag(parsing_context &pc) {
        return matching_flag_on(pc, false, false, [](opt::cmd *c) -> opt::details::indexed_args const & {
            return c->_short_args;
        });
    }

    inline opt::Action app::unknown_long_flag_found(parsing_context &pc, arg_matching_result &amr) {
        unused(pc);
        unused(amr);
        if (_on_unknown_argument_found)
            if (auto rc = _on_unknown_argument_found(pc.title, pc.last_matched_cmd(), true, false);
                rc != opt::RunDefaultAction)
                return rc;
        std::cerr << "Unknown long flag: " << std::quoted(pc.title.substr(pc.pos));
        auto &c = pc.last_matched_cmd();
        if (c.valid())
            std::cerr << " under matched command: " << std::quoted(c.title());
        std::cerr << '\n';
        return opt::Abortion;
    }

    inline opt::Action app::unknown_short_flag_found(parsing_context &pc, arg_matching_result &amr) {
        unused(pc);
        unused(amr);
        if (_on_unknown_argument_found)
            if (auto rc = _on_unknown_argument_found(pc.title, pc.last_matched_cmd(), false, false);
                rc != opt::RunDefaultAction)
                return rc;
        std::cerr << "Unknown short flag: " << std::quoted(pc.title.substr(pc.pos));
        auto &c = pc.last_matched_cmd();
        if (c.valid())
            std::cerr << " under matched command: " << std::quoted(c.title());
        std::cerr << '\n';
        return opt::Abortion;
    }

    inline opt::Action app::unknown_command_found(parsing_context &pc, cmd_matching_result &cmr) {
        unused(cmr);
        if (_on_unknown_argument_found)
            if (auto rc = _on_unknown_argument_found(pc.title, pc.last_matched_cmd(), false, true);
                rc != opt::RunDefaultAction)
                return rc;
        std::cerr << "Unknown command: " << std::quoted(pc.title);
        auto &c = pc.last_matched_cmd();
        if (c.valid())
            std::cerr << " under matched command: " << std::quoted(c.title());
        std::cerr << '\n';
        return opt::Abortion;
    }

    inline int app::invoke_command(opt::cmd &c, string_array remain_args, parsing_context &pc) {
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
                    if (_on_command_not_hooked)
                        rc = _on_command_not_hooked(c, remain_args);
#if defined(_DEBUG)
                        // store.root().dump_full_keys(std::cout);
                        // _store.dump_tree(std::cout);
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

    inline void app::print_cmd(std::ostream &ss, cmdr::terminal::colors::colorize &c,
                               opt::cmd *cc,
                               std::string const &app_name, std::string const &exe_name) {
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
        std::vector<std::string> cmds;
        auto pcc = cc;
        while (pcc && pcc->owner()) {
            cmds.insert(cmds.begin(), pcc->hit_count() > 0 ? pcc->hit_title() : pcc->title_long());
            pcc = pcc->owner();
        }
        ss << string::pad_left(exe_name, 2) << ' ' << string::join(cmds, ' ') << " [options] [Tail Args]" << std::endl;

        ss << std::endl
           << "Commands" << std::endl;

        std::ostringstream os1;
        cc->print_commands(os1, c, _minimal_tab_width, true, -1);

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
            trivial->print_flags(os2, c, _minimal_tab_width, true, -1);
        } while ((trivial = trivial->owner()) != nullptr);

        if (saved_minimal_tab_width < _minimal_tab_width) {
            std::ostringstream os3;
            cc->print_commands(os3, c, _minimal_tab_width, true, -1);
            ss << os3.str() << os2.str();
        } else {
            ss << os1.str() << os2.str();
        }

        unused(app_name);
        unused(exe_name);
    }

    inline void app::print_usages(opt::cmd *start) {
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

    inline void app::prepare() {
        prepare_env_vars();
        load_externals();
        apply_env_vars();
    }

    inline void app::prepare_env_vars() {
        setenv("APP_NAME", _name.c_str(), 1);
        setenv("EXE_DIR", path::get_executable_dir().c_str(), 1);
        setenv("EXE_PATH", path::get_executable_path().c_str(), 1);
    }

    inline void app::load_externals() {
        on_loading_externals();
    }

    inline void app::apply_env_vars() {
        // auto env vars
        _store.walk_by_full_keys([](std::pair<std::string, vars::variable *> const &val) {
            auto ks = string::reg_replace(val.first, R"([.-])", "_");
            string::to_upper(ks);
            char *ptr = std::getenv(ks.c_str());
            if (ptr) {
                std::stringstream(ptr) >> (*val.second);
                verbose_debug("  ENV[%s (%s)] => %s", ks.c_str(), val.first.c_str(), ptr);
                // std::cout << "  ENV[" << ks << '(' << val.first << ")] => " << ptr << '\n';
            }
        });

        // cmd & args
        this->walk_args([&](opt::arg &a) {
            for (auto &ev : a.env_vars_get()) {
                char *ptr = std::getenv(ev.c_str());
                if (ptr) {
                    auto dk = a.dotted_key();
                    auto &v = _store.get_raw(dk);
                    std::stringstream(ptr) >> v;
                    // std::cout << "  ENV[" << ev << '(' << dk << ")] => " << ptr << " / " << _store.get_raw(dk) << '\n';
                    verbose_debug("  ENV[%s (%s)] => %s / %s", ev.c_str(), dk.c_str(), ptr, _store.get_raw(dk).as_string().c_str());
                    a.default_value(v);
                    a.update_hit_count_from_env(ev, 1);
                    // std::cout << " / " << a.default_value() << '\n';
                }
            }
        });
    }

    inline int app::run(int argc, char *argv[]) {
        // std::cout << "Hello, World!" << std::endl;

        prepare();

        parsing_context pc{this};
        pc.add_matched_cmd(this);
        opt::Action rc;

        for (int i = 1; i < argc; i = pc.index + 1) {
            pc.title = argv[i];
            pc.index = i;
            pc.pos = 0;

            if (pc.title[0] == '~' && pc.title[1] == '~') {
                // special flags
                rc = process_special_flag(pc, argc, argv);
                if (rc < opt::OK || rc >= opt::Abortion)
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
                    if (rc < opt::OK || rc >= opt::Abortion)
                        return 0;
                    continue;
                }

                // short flag
                rc = process_short_flag(pc, argc, argv);
                if (rc < opt::OK || rc >= opt::Abortion)
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
            if (rc < opt::OK || rc >= opt::Abortion)
                return 0;
        }

        return after_run(rc, pc, argc, argv);
    }

    inline int app::after_run(opt::Action rc, parsing_context &pc, int argc, char *argv[]) {
        if (rc > opt::OK && rc < opt::Continue)
            return internal_action(rc, pc, argc, argv);

        if (store().get_raw_p(_long, "help").cast_as<bool>()) {
            print_usages(&pc.curr_command());
            return true;
        }

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

    inline int app::internal_action(opt::Action rc, parsing_context &pc, int argc, char *argv[]) {
        if (auto it = _internal_actions.find(rc); it != _internal_actions.end())
            return it->second(pc, argc, argv);
        return 0;
    }


} // namespace cmdr

namespace cmdr::opt {
    //
    //
    //


    inline cmd &operator+(cmd &lhs, const sub_cmd &rhs) {
        lhs += rhs.underlying();
        return lhs;
    }

    inline cmd &operator+=(cmd &lhs, const sub_cmd &rhs) {
        lhs += rhs.underlying();
        return lhs;
    }

    inline cmd &operator+(cmd &lhs, const opt &rhs) {
        lhs += rhs.underlying();
        return lhs;
    }

    inline cmd &operator+=(cmd &lhs, const opt &rhs) {
        lhs += rhs.underlying();
        return lhs;
    }


} // namespace cmdr::opt


#endif //CMDR_CXX11_CMDR_IMPL_HH
