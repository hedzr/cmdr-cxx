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


    inline string_array app::remain_args(opt::types::parsing_context &pc, char *argv[], int i, int argc) {
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

    inline opt::Action app::process_command(opt::types::parsing_context &pc, int argc, char *argv[]) {
        pc.is_flag = false;
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
    app::process_flag(opt::types::parsing_context &pc, int argc, char *argv[], int leading_chars,
                      std::function<arg_matching_result(opt::types::parsing_context &)> const &matcher) {
        pc.title = pc.title.substr(leading_chars);
        bool value_parsed{};
        bool extra_argv;
        auto rc{opt::Continue};
        // int matched_length = 0;
        // std::vector<arg_matching_result> tmp_amr_list;

    next_combined:
        auto amr = matcher(pc);
        if (amr.matched) {
            CMDR_ASSERT(amr.obj);

            // if (amr.matched_length>matched_length){
            //     matched_length=amr.matched_length;
            //     tmp_amr_list.push_back(amr);
            // }

            // std::cout << " - " << amr.matched_str << ' ' << '(' << amr.obj->dotted_key() << ')';
            cmdr_verbose_debug(" - %s (%s)", amr.matched_str.c_str(), amr.obj->dotted_key().c_str());

            amr.obj->hit_title(amr.matched_str.c_str()); // pc.title.c_str());

            if (auto &typ = amr.obj->default_value()->type();
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
                opt::arg::var_type &val = amr.obj->default_value();
                auto xb = sst.tellg();
                sst >> *val.get();
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
                cmdr_verbose_debug("   -> value: %s", val->as_string().c_str());

                get_app().on_arg_matched(amr.obj, val);

            } else if (typ == typeid(bool)) {
                pc.add_matched_arg(amr.obj);
                value_parsed = true;

                bool v{true};
                auto ch = pc.title[pc.pos + amr.matched_length];
                switch (ch) {
                    case '-':
                        v = false;
                        amr.matched_length++;
                        break;
                    case '+':
                        amr.matched_length++;
                        break;
                }
                auto value = std::make_shared<vars::variable>(v);
                get_app().on_arg_matched(amr.obj, value);

            } else {
                pc.add_matched_arg(amr.obj);
                value_parsed = true;

                auto value = std::make_shared<vars::variable>(true);
                get_app().on_arg_matched(amr.obj, value);
            }

            // std::cout << '\n';
            pc.title_fragment = amr.matched_str;

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

        pc.title_fragment = pc.title;
        pc.title = argv[pc.index];
        pc.add_unknown_arg(pc.title);
        if (_treat_unknown_input_flag_as_error) {
            // if (pc.is_flag) {
            switch (pc.matching_flag_type) {
                case 0:
                    return unknown_short_flag_found(pc, amr);
                default:
                    return unknown_long_flag_found(pc, amr);
            }
            // }
        }
        return opt::Continue;
    }

    inline opt::Action app::process_special_flag(opt::types::parsing_context &pc, int argc, char *argv[]) {
        return process_flag(pc.mft(2), argc, argv, 2, matching_special_flag);
    }

    inline opt::Action app::process_long_flag(opt::types::parsing_context &pc, int argc, char *argv[]) {
        return process_flag(pc.mft(1), argc, argv, 2, matching_long_flag);
    }

    inline opt::Action app::process_short_flag(opt::types::parsing_context &pc, int argc, char *argv[]) {
        return process_flag(pc.mft(0), argc, argv, 1, matching_short_flag);
    }

    inline typename app::cmd_matching_result app::matching_command(opt::types::parsing_context &pc) {
        cmd_matching_result cmr;
        auto &c = pc.curr_command();
        opt::types::indexed_commands &li = c._long_commands;
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
    app::matching_flag_on(opt::types::parsing_context &pc,
                          bool is_long, bool is_special,
                          std::function<opt::types::indexed_args const &(opt::cmd *)> const &li) {
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
                            is_long |= itz.second->hit_long();
                            is_special |= itz.second->hit_special();
                            amr.obj = (cmdr::opt::arg *) &(itz.second->update_hit_count(title.substr(0, itz.first.length()), 1, is_long, is_special));
                        }
                    } else {
                        amr.matched = true;
                        amr.matched_length = itz.first.length();
                        amr.matched_str = itz.first;
                        is_long |= itz.second->hit_long();
                        is_special |= itz.second->hit_special();
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
    app::matching_special_flag(opt::types::parsing_context &pc) {
        return matching_flag_on(pc, true, true, [](opt::cmd *c) -> opt::types::indexed_args const & {
            return c->_long_args;
        });
    }

    inline typename app::arg_matching_result
    app::matching_long_flag(opt::types::parsing_context &pc) {
        return matching_flag_on(pc, true, false, [](opt::cmd *c) -> opt::types::indexed_args const & {
            return c->_long_args;
        });
    }

    inline typename app::arg_matching_result
    app::matching_short_flag(opt::types::parsing_context &pc) {
        return matching_flag_on(pc, false, false, [](opt::cmd *c) -> opt::types::indexed_args const & {
            return c->_short_args;
        });
    }

    inline opt::Action app::unknown_long_flag_found(opt::types::parsing_context &pc, arg_matching_result &amr) {
        UNUSED(pc, amr);
        if (_on_unknown_argument_found)
            if (auto rc = _on_unknown_argument_found(pc.title, pc.last_matched_cmd(), true, false);
                rc != opt::RunDefaultAction)
                return rc;
        std::cerr << "Unknown long flag: --" << std::quoted(pc.title_fragment);
        auto *c = &pc.last_matched_cmd();
        if (c->valid()) {
            std::cerr << " under matched command: " << std::quoted(c->title_sequences());
            std::cerr << '\n';

            text::jaro_winkler_distance jaro{_jaro_winkler_matching_threshold};
            std::ostringstream os;
            os << "  Did you mean";
            do {
                for (auto const &cc : c->_all_args) {
                    if (jaro.calculate(pc.title.c_str(), cc.title_long().c_str()).matched())
                        os << '\n'
                           << "   - " << std::quoted(cc.title_long()) << ' ' << '[' << jaro.get_distance() << ']' << " (under command " << std::quoted(c->title_sequences()) << ')' << '?';
                    for (auto const &str : cc.title_aliases()) {
                        if (jaro.calculate(pc.title.c_str(), str.c_str()).matched())
                            os << '\n'
                               << "   - " << std::quoted(str) << ' ' << '[' << jaro.get_distance() << ']' << " (under command " << std::quoted(c->title_sequences()) << ')' << '?';
                    }
                }
            } while ((c = c->owner()) != nullptr);
            if (os.tellp() > 0) {
                std::cerr << os.str();
            }
        }
        std::cerr << '\n';
        return opt::Abortion;
    }

    inline opt::Action app::unknown_short_flag_found(opt::types::parsing_context &pc, arg_matching_result &amr) {
        UNUSED(pc, amr);
        if (_on_unknown_argument_found)
            if (auto rc = _on_unknown_argument_found(pc.title, pc.last_matched_cmd(), false, false);
                rc != opt::RunDefaultAction)
                return rc;
        std::cerr << "Unknown short flag: -" << std::quoted(pc.title_fragment);
        auto *c = &pc.last_matched_cmd();
        if (c->valid()) {
            std::cerr << " under matched command: " << std::quoted(c->title_sequences());
            std::cerr << '\n';

            text::jaro_winkler_distance jaro{_jaro_winkler_matching_threshold};
            std::ostringstream os;
            os << "  Did you mean";
            do {
                for (auto const &cc : c->_all_args) {
                    if (jaro.calculate(pc.title.c_str(), cc.title_short().c_str()).matched())
                        os << '\n'
                           << "   - " << std::quoted(cc.title_short()) << ' ' << '[' << jaro.get_distance() << ']' << " (under command " << std::quoted(c->title_sequences()) << ')' << '?';
                }
            } while ((c = c->owner()) != nullptr);
            if (os.tellp() > 0) {
                std::cerr << os.str();
            }
        }
        std::cerr << '\n';
        return opt::Abortion;
    }

    inline opt::Action app::unknown_command_found(opt::types::parsing_context &pc, cmd_matching_result &cmr) {
        UNUSED(pc, cmr);
        if (_on_unknown_argument_found)
            if (auto rc = _on_unknown_argument_found(pc.title, pc.last_matched_cmd(), false, true);
                rc != opt::RunDefaultAction)
                return rc;
        std::cerr << "Unknown command: " << std::quoted(pc.title);
        auto &c = pc.last_matched_cmd();
        if (c.valid()) {
            std::cerr << " under matched command: " << std::quoted(c.title_sequences());
            std::cerr << '\n';

            text::jaro_winkler_distance jaro{_jaro_winkler_matching_threshold};
            std::cerr << "  Did you mean";
            for (auto const &cc : c._all_commands) {
                if (jaro.calculate(pc.title.c_str(), cc._long.c_str()).matched())
                    std::cerr << '\n'
                              << "   - " << std::quoted(cc._long) << '?';
                if (jaro.calculate(pc.title.c_str(), cc._short.c_str()).matched())
                    std::cerr << '\n'
                              << "   - " << std::quoted(cc._short) << '?';
                for (auto const &str : cc._aliases) {
                    if (jaro.calculate(pc.title.c_str(), str.c_str()).matched())
                        std::cerr << '\n'
                                  << "   - " << std::quoted(str) << '?';
                }
            }
        }
        std::cerr << '\n';
        return opt::Abortion;
    }


    inline void app::on_arg_added(opt::arg *a) {
        auto key = a->dotted_key();
        auto *val = a->default_value().get();
        _store.set_raw(key.c_str(), *val);
        for (auto &cb : _on_arg_added) {
            if (cb)
                cb(a);
        }
    }

    inline void app::on_cmd_added(opt::cmd *a) {
        // auto key = a->dotted_key();
        // _store.set(key, a->default_value());
        for (auto &cb : _on_cmd_added) {
            if (cb)
                cb(a);
        }
    }

    inline void app::on_arg_matched(opt::arg *a, opt::arg::var_type &value) {
        auto key = a->dotted_key();
        auto *val = value.get();
        _store.set_raw(key.c_str(), *val);
        for (auto &cb : _on_arg_matched) {
            if (cb)
                cb(a);
        }
    }

    inline void app::on_cmd_matched(opt::cmd *a) {
        // auto key = a->dotted_key();
        // _store.set(key, a->default_value());
        for (auto &cb : _on_cmd_matched) {
            if (cb)
                cb(a);
        }
    }

    inline void app::on_loading_externals() {
        for (auto &_on_loading_external : _on_loading_externals) {
            if (_on_loading_external)
                _on_loading_external(*this);
        }
    }


    inline void app::set(char const *key, vars::variable &&a) {
        _store.set_raw_p(store_prefix().c_str(), key, a);
    }

    inline void app::set(char const *key, vars::variable const &a) {
        _store.set_raw_p(store_prefix().c_str(), key, a);
    }


    inline int app::invoke_command(opt::cmd &c, string_array const &remain_args, opt::types::parsing_context &pc) {
        UNUSED(pc, c);

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

            if (c.on_post_invoke())
                c.on_post_invoke()(c, remain_args);
            if (_global_on_post_invoke)
                _global_on_post_invoke(c, remain_args);

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
                               opt::cmd const *cc,
                               std::string const &app_name, std::string const &exe_name) {
        if (!cc->description().empty()) {
            ss << '\n'
               << "Description" << '\n'
               << string::pad_left(cc->description()) << '\n';
        }
        if (!cc->examples().empty()) {
            ss << '\n'
               << "Examples" << '\n'
               << string::pad_left(string::reg_replace(cc->examples(), "~", exe_name)) << '\n';
        }
        ss << '\n'
           << "Usage" << '\n';
        ss << string::pad_left(exe_name, 2) << ' ' << cc->title_sequences() << " [options] [Tail Args]" << '\n';

        ss << '\n'
           << "Commands" << '\n';

        std::ostringstream os1;
        cc->print_commands(os1, c, _minimal_tab_width, true, -1);

        std::vector<std::ostringstream *> os2;
        auto saved_minimal_tab_width = _minimal_tab_width;
    restart:
        auto trivial = cc;
        do {
            std::ostringstream tt;
            if (trivial == cc)
                tt << "Options";
            else if (trivial->owner()) {
                tt << "Parent Options of " << std::quoted(trivial->title());
            } else {
                tt << "Global Options";
            }

            auto *os = new std::ostringstream();
            (*os) << '\n'
                  << tt.str() << '\n';

            auto smtw = _minimal_tab_width;
            trivial->print_flags(*os, c, _minimal_tab_width, true, -1);
            if (smtw < _minimal_tab_width) {
                os2.clear();
                delete os;
                goto restart;
            }

            os2.push_back(os);
        } while ((trivial = trivial->owner()) != nullptr);

        if (saved_minimal_tab_width < _minimal_tab_width) {
            std::ostringstream os3;
            cc->print_commands(os3, c, _minimal_tab_width, true, -1);
            ss << os3.str();
        } else {
            ss << os1.str();
        }

        for (auto &os : os2) {
            ss << os->str();
            delete os;
        }

        UNUSED(app_name, exe_name);
    }

    inline void app::print_usages(opt::cmd const *start) {
        std::string exe_name = path::executable_name();
        auto c = cmdr::terminal::colors::colorize::create();

        if (is_no_color())
            cmdr::terminal::colors::colorize::enable(false);

        std::stringstream ss;
        ss << _name << ' ' << 'v' << _version;
        if (_header.empty())
            ss << " by " << _author << '.' << ' ' << _copyright << '\n';
        else
            ss << _header << '\n';

        print_cmd(ss, c, start ? start : (opt::cmd const *) this, _name, exe_name);

        if (!_tail_line.empty()) {
            ss << '\n'
               << _tail_line << '\n';
        } else {
            ss << '\n'
               << "Type `" << exe_name << " --help` to get help screen (this screen)." << '\n';
        }

        std::cout << ss.str();
    }

    inline void app::reset() {
        _store.reset();

        _global_on_pre_invoke = nullptr;
        _global_on_post_invoke = nullptr;
        _on_unknown_argument_found = nullptr;

        _internal_actions.clear();

        _on_arg_added.clear();
        _on_cmd_added.clear();
        _on_arg_matched.clear();
        _on_cmd_matched.clear();
        _on_loading_externals.clear();
        _on_command_not_hooked = nullptr;
        _on_handle_exception_ptr = nullptr;
    }

    inline void app::prepare_common_env() {
#if !defined(_DEBUG)
        _store.set_dump_with_type_name(false);
#endif
    }

    inline void app::prepare_env_vars() {
        setenv("APP_NAME", _name.c_str(), 1);
        setenv("EXE_DIR", path::get_executable_dir().c_str(), 1);
        setenv("EXE_PATH", path::get_executable_path().c_str(), 1);
    }

    inline void app::load_externals() {
        cmdr_verbose_debug("   - load_externals ...");
        on_loading_externals();
    }

    inline void app::apply_env_vars() {
        cmdr_verbose_debug("   - apply_env_vars ...");
        // auto env vars
        _store.walk_by_full_keys([](std::pair<std::string, vars::store::node_pointer> const &val) {
            auto ks = string::reg_replace(val.first, R"([.-])", "_");
            string::to_upper(ks);
            char *ptr = std::getenv(ks.c_str());
            if (ptr) {
                std::stringstream(ptr) >> (*val.second);
                cmdr_verbose_debug("      ENV[%s (%s)] => %s", ks.c_str(), val.first.c_str(), ptr);
                // std::cout << "  ENV[" << ks << '(' << val.first << ")] => " << ptr << '\n';
            }
        });

        // cmd & args
        walk_args([&](opt::arg &a) {
            for (auto &ev : a.env_vars_get()) {
                char *ptr = std::getenv(ev.c_str());
                if (ptr) {
                    auto dk = a.dotted_key();
                    auto &v = _store.get_raw(dk);
                    std::stringstream(ptr) >> v;
                    // std::cout << "  ENV[" << ev << '(' << dk << ")] => " << ptr << " / " << _store.get_raw(dk) << '\n';
                    cmdr_verbose_debug("      ENV[%s (%s)] => %s / %s", ev.c_str(), dk.c_str(), ptr, _store.get_raw(dk).as_string().c_str());
                    a.default_value(v);
                    a.update_hit_count_from_env(ev, 1);
                    // std::cout << " / " << a.default_value() << '\n';
                }
            }
        });
    }

    inline void app::prepare() {
        prepare_common_env();
        prepare_env_vars();
        load_externals();
        apply_env_vars();
    }

    inline int app::run(int argc, char *argv[]) {
        // debug::SigSegVInstaller _sigsegv_installer;
        // _sigsegv_installer.baz();

        cmdr_verbose_debug(" - app::run ...");
        std::lock_guard _guard(_run_is_singleton);

        class post_runner {
            std::function<void()> _fn;

        public:
            post_runner(std::function<void()> const &fn)
                : _fn(fn) {}
            ~post_runner() {
                if (_fn) _fn();
            }
        };
        // // optional to post_run(), for the rare exception post processing if necessary
        post_runner post_runner_([=]() { post_run(); });

        // std::cout << "Hello, World!" << '\n';
        try {
            prepare();

            cmdr_verbose_debug("   - app::parsing args ...");
            opt::types::parsing_context pc{(opt::cmd *) this};
            pc.add_matched_cmd((opt::cmd *) this);
            opt::Action rc{opt::OK};

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

            cmdr_verbose_debug("   - app::after_run ...");
            return after_run(rc, pc, argc, argv);

        } catch (cmdr::exception::cmdr_biz_error const &e) {
            if (_no_catch_cmdr_biz_error)
                throw e;
            CMDR_DUMP_WITHOUT_STACK_TRACE(e);

        } catch (std::exception const &e) {
            std::cerr << "Exception caught : " << e.what() << '\n';
            std::cerr << "         cmdline : [";
            for (int i = 0; i < argc; i++) {
                std::cerr << ' ' << argv[i];
            }
            std::cerr << " ]" << '\n';
            CMDR_DUMP_STACK_TRACE(e);

            // } catch (...) {
        }
        return -1;
    }

    inline int app::after_run(opt::Action rc, opt::types::parsing_context &pc, int argc, char *argv[]) {
        if (rc > opt::OK && rc < opt::Continue)
            return internal_action(rc, pc, argc, argv);

        auto &help_arg = store().get_raw_p(_long, "help");
        if (help_arg.cast_as<bool>()) {
            print_usages(&pc.curr_command());
            return 0;
        }

        if (auto cc = pc.last_matched_cmd(); cc.valid()) {
            if (cc.no_sub_commands()) {
                // invoking cc
                try {
                    return invoke_command(cc, remain_args(pc, argv, pc.index + 1, argc), pc);
                } catch (opt::cmdr_requests_exception const &ex) {
                    return internal_action(ex._action, pc, argc, argv);
                }
            }

            print_usages(&cc);
        } else {
            print_usages(&pc.curr_command());
        }
        return 0;
    }

    inline int app::internal_action(opt::Action rc, opt::types::parsing_context &pc, int argc, char *argv[]) {
        if (auto it = _internal_actions.find(rc); it != _internal_actions.end())
            return it->second(pc, argc, argv);
        return 0;
    }


    inline void app::handle_eptr(const std::exception_ptr &eptr) const {
        if (_on_handle_exception_ptr) {
            _on_handle_exception_ptr(eptr);
            return;
        }

        try {
            if (eptr) {
                std::rethrow_exception(eptr);
            }
        } catch (const std::exception &e) {
            std::cout << "Caught exception \"" << e.what() << "\"\n";
            CMDR_DUMP_STACK_TRACE(e);
        }
    }


    inline app &app::operator+(opt::arg const &a) {
        add(a);
        return (*this);
    }
    inline app &app::operator+=(opt::arg const &a) {
        add(a);
        return (*this);
    }
    inline app &app::operator+(opt::cmd const &a) {
        add(a);
        return (*this);
    }
    inline app &app::operator+=(opt::cmd const &a) {
        add(a);
        return (*this);
    }

    inline app &app::operator+(const opt::sub_cmd &rhs) {
        add(rhs.underlying());
        return (*this);
    }
    inline app &app::operator+=(const opt::sub_cmd &rhs) {
        add(rhs.underlying());
        return (*this);
    }
    inline app &app::operator+(const opt::opt &rhs) {
        add(rhs.underlying());
        return (*this);
    }
    inline app &app::operator+=(const opt::opt &rhs) {
        add(rhs.underlying());
        return (*this);
    }


    //
    //
    //


    // inline void app::parsing_context::add_matched_arg(opt::arg *obj, opt::arg::var_type const &v) {
    //     matched_flags.push_back(obj);
    //     _values_map.emplace(std::make_pair(obj, v));
    // }


} // namespace cmdr

namespace cmdr::opt {
    //
    //
    //


    inline cmd &cmd::operator+(const sub_cmd &rhs) {
        add(rhs.underlying());
        return (*this);
    }

    inline cmd &cmd::operator+=(const sub_cmd &rhs) {
        add(rhs.underlying());
        return (*this);
    }

    inline cmd &cmd::operator+(const cmdr::opt::opt &rhs) {
        add(rhs.underlying());
        return (*this);
    }

    inline cmd &cmd::operator+=(const cmdr::opt::opt &rhs) {
        add(rhs.underlying());
        return (*this);
    }


} // namespace cmdr::opt


#endif //CMDR_CXX11_CMDR_IMPL_HH
