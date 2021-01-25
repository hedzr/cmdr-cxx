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
#include "cmdr_terminal.hh"


namespace cmdr {

    class AppHolder final : public util::Singleton<AppHolder> {
    public:
        explicit AppHolder(token) { /*std::cout << "AppHolder constructed" << std::endl;*/
        }
        ~AppHolder() { /*std::cout << "AppHolder destructed" << std::endl;*/
        }

        [[maybe_unused]] void use() const { std::cout << "in use" << std::endl; }

    public:
        opt::app *get_ptr() { return _app; }
        opt::app &operator*() { return *_app; }
        opt::app *operator->() { return _app; }
        // explicit operator opt::app() { return _app; }

    private:
        opt::app *_app{};
        void put(opt::app *ptr) { _app = ptr; }
        friend class opt::app;
    };

    inline opt::app &get_app() { return *AppHolder::instance().get_ptr(); }
    inline auto &get_store() { return AppHolder::instance().get_ptr()->store(); }

    inline opt::app cli(const_chars name, const_chars version,
                        const_chars author = nullptr, const_chars copyright = nullptr,
                        const_chars description = nullptr,
                        const_chars examples = nullptr) {
        if (AppHolder::instance().get_ptr()) {
            throw std::logic_error("can't invoke create_app() multiple times.");
        }

        return opt::app::create(name, version, author, copyright, description, examples);
    }


} // namespace cmdr

namespace cmdr::opt {

    //
    //
    //


    inline cmd &cmd::operator+(const arg &a) {
        if (a.valid()) {
            auto gn = a.group_name();
            if (gn.empty()) {
                if (a.toggle_group_name().empty())
                    gn = UNSORTED_GROUP;
                else
                    gn = a.toggle_group_name();
            }

            if (gn != a.group_name()) {
                auto ac = a;
                ac.group(gn.c_str());
                _all_args.push_back(ac);
            } else
                _all_args.push_back(a);

            auto ptr = &_all_args.back();

            if (_grouped_args.find(gn) == _grouped_args.end())
                _grouped_args.emplace(gn, details::arg_pointers{});
            // auto ptr = &_all_args.back();
            // auto it = _grouped_args.find(gn);
            // (*it).second.push_back(ptr);
            _grouped_args[gn].push_back(ptr);

            _indexed_args.insert({a.title_long(), ptr});
            if (!a.title_short().empty()) {
                if (auto const &it = _short_args.find(a.title_short()); it != _short_args.end())
                    throw std::logic_error(std::string("duplicated short flag found: -") + a.title_short());
                _short_args.insert({a.title_short(), ptr});
            }
            if (auto const &it = _long_args.find(a.title_long()); it != _long_args.end())
                throw std::logic_error(std::string("duplicated long flag found: --") + a.title_long());
            _long_args.insert({a.title_long(), ptr});
            for (auto const &itz : a.title_aliases()) {
                if (auto const &it = _long_args.find(itz); it != _long_args.end())
                    throw std::logic_error(std::string("duplicated alias flag found: -") + itz);
                _long_args.insert({itz, ptr});
            }

            _last_added_arg = ptr;
            // std::cout << gn << ',' << _grouped_args[gn].size() << std::endl;
        }
        return *this;
    }

    inline cmd &cmd::operator+=(const arg &a) {
        this->operator+(a);
        return *this;
    }

    inline cmd &cmd::operator+(const cmd &a) {
        if (a.valid()) {
            auto gn = a.group_name();
            if (gn.empty()) {
                gn = UNSORTED_GROUP;
            }

            if (gn != a.group_name()) {
                auto ac = a;
                ac.group(gn.c_str());
                _all_commands.push_back(ac);
            } else
                _all_commands.push_back(a);

            auto ptr = &_all_commands.back();

            if (_grouped_commands.find(gn) == _grouped_commands.end())
                _grouped_commands.emplace(gn, details::cmd_pointers{});
            _grouped_commands[gn].push_back(ptr);

            _indexed_commands.insert({a.title_long(), ptr});
            if (!a.title_short().empty()) {
                if (auto const &it = _short_commands.find(a.title_short()); it != _short_commands.end())
                    throw std::logic_error(std::string("duplicated short command found: -") + a.title_short());
                _short_commands.insert({a.title_short(), ptr});
            }
            if (auto const &it = _long_commands.find(a.title_long()); it != _long_commands.end())
                throw std::logic_error(std::string("duplicated long command found: --") + a.title_long());
            _long_commands.insert({a.title_long(), ptr});
            for (auto const &itz : a.title_aliases()) {
                if (auto const &it = _long_commands.find(itz); it != _long_commands.end())
                    throw std::logic_error(std::string("duplicated alias command found: -") + itz);
                _long_commands.insert({itz, ptr});
            }

            _last_added_command = ptr;
        }
        return *this;
    }

    inline cmd &cmd::operator+=(const cmd &a) {
        this->operator+(a);
        return *this;
    }

    inline cmd &operator+(cmd &lhs, const opts::cmd_base &rhs) {
        lhs += rhs.underlying();
        return lhs;
    }

    inline cmd &operator+=(cmd &lhs, const opts::cmd_base &rhs) {
        lhs += rhs.underlying();
        return lhs;
    }

    inline cmd &operator+(cmd &lhs, const opts::opt_base &rhs) {
        lhs += rhs.underlying();
        return lhs;
    }

    inline cmd &operator+=(cmd &lhs, const opts::opt_base &rhs) {
        lhs += rhs.underlying();
        return lhs;
    }

    inline arg &cmd::find_flag(const_chars long_title, bool extensive) {
        auto s = long_title;
        if (is_leading_switch_char(s)) s++;
        if (is_leading_switch_char(s)) s++;
        auto it = _indexed_args.find(s);
        if (it != _indexed_args.end())
            return *((*it).second);
        if (extensive) {
            for (auto itz : _indexed_args) {
                if (itz.second->title_short() == long_title)
                    return *((itz).second);
                for (auto &k : itz.second->title_aliases()) {
                    if (k == long_title)
                        return *((itz).second);
                }
            }
        }
        return null_arg();
    }

    inline arg &cmd::operator[](const_chars long_title) { return find_flag(long_title); }
    inline const arg &cmd::operator[](const_chars long_title) const { return const_cast<cmd &>(*this).find_flag(long_title); }

    inline cmd &cmd::find_command(const_chars long_title, bool extensive) {
        auto it = _indexed_commands.find(long_title);
        if (it != _indexed_commands.end())
            return *((*it).second);
        if (extensive) {
            for (auto itz : _indexed_commands) {
                if (itz.second->title_short() == long_title)
                    return *((itz).second);
                for (auto &k : itz.second->title_aliases()) {
                    if (k == long_title)
                        return *((itz).second);
                }
            }
        }
        return null_command();
    }

    inline cmd &cmd::operator()(const_chars long_title, bool extensive) { return find_command(long_title, extensive); }
    inline cmd const &cmd::operator()(const_chars long_title, bool extensive) const { return const_cast<cmd &>(*this).find_command(long_title, extensive); }

    inline int cmd::run(int argc, char *argv[]) {
        unused(argc);
        unused(argv);
        return 0;
    }

    inline void cmd::print_commands(std::ostream &ss, cmdr::terminal::colors::colorize &c, bool grouped) {
        unused(grouped);

        std::set<std::string> keys;
        std::map<std::string, std::string> dotted_key_on_keys;
        for (auto &it : _grouped_commands) {
            keys.insert(it.first);
            auto ptr = it.first.find('.');
            if (ptr != std::string::npos) {
                auto dotted = it.first.substr(0, ptr);
                dotted_key_on_keys.insert({dotted, it.first});
            } else {
                dotted_key_on_keys.insert({NOBODY_GROUP_SORTER, it.first});
            }
        }

        int count_all{};
        for (auto &it : dotted_key_on_keys) {
            auto val = _grouped_commands[it.second];
            auto clean_key = it.first == NOBODY_GROUP_SORTER ? it.second : it.second.substr(it.first.length() + 1);

            int wf = 0, ws = 0, wa = 0, wt = 0, w = 0, valid_count = 0;
            for (auto &x : val) {
                if (x->hidden()) continue;
                valid_count++;
                w = x->title_long().length();
                if (w > wf) wf = w;
                w = x->title_short().length();
                if (w > ws) ws = w;
                w = 0;
                for (auto const &t : x->title_aliases()) {
                    if (w > 0) w += 1;
                    w += t.length();
                }
                if (w > wa) wa = w;
            }

            wt = wf + 2 + ws + 2 + wa + 2;
            if (wt < 43) wt = 43;

            if (valid_count == 0)
                continue;

            if (it.second != UNSORTED_GROUP) {
                int i = 0;
                for (auto &x : val) {
                    if (x->hidden()) continue;
                    i++;
                }
                if (i > 0) {
                    ss << ' ' << ' ';
                    std::stringstream tmp;
                    tmp << '[' << clean_key << ']';
                    ss << c.dim().s(tmp.str()) << std::endl;
                }
            }

            for (auto &x : val) {
                if (x->hidden()) continue;
                // ss << "  " << std::setw(43) << std::left << x->title();
                ss << "  " << std::left << std::setfill(' ');
                if (!x->title_long().empty()) {
                    w = x->title_long().length();
                    ss << c.underline().s(x->title_long());

                    if (!x->title_short().empty() || !x->title_aliases().empty()) ss << ", ";
                    else
                        ss << "  ";

                    w = wf - w;
                    if (w > 0) ss << std::setw(w) << ' ';
                } else
                    ss << std::setw(wf + 2) << ' ';
                if (!x->title_short().empty()) {
                    w = x->title_short().length();
                    ss << x->title_short();
                    if (!x->title_aliases().empty()) ss << ", ";
                    else
                        ss << "  ";
                    w = ws - w;
                    if (w > 0) ss << std::setw(w) << ' ';
                } else
                    ss << std::setw(ws + 2) << ' ';
                if (!x->title_aliases().empty()) {
                    w = 0;
                    std::stringstream tmp;
                    for (auto const &t : x->title_aliases()) {
                        if (w > 0) {
                            tmp << ',';
                        } else
                            w++;
                        tmp << t;
                    }
                    ss << std::setw(wa) << tmp.str();
                } else
                    ss << std::setw(wa) << ' ';

                w = wf + 2 + ws + 2 + wa;
                ss << std::setw(wt - w) << ' ';

                ss << c.dim().s(x->descriptions())
                   // << wt << ',' << w << '|' << wf << ',' << ws << ',' << wa
                   << std::endl;

                count_all++;
            }
        }

        if (count_all == 0) {
            ss << c.dim().s("  (no sub-commands)") << '\n';
        }
    }

    inline void cmd::print_flags(std::ostream &ss, cmdr::terminal::colors::colorize &c, bool grouped) {
        unused(grouped);

        std::set<std::string> keys;
        std::map<std::string, std::string> dotted_key_on_keys;
        for (auto &it : _grouped_args) {
            keys.insert(it.first);
            auto ptr = it.first.find('.');
            if (ptr != std::string::npos) {
                auto dotted = it.first.substr(0, ptr);
                dotted_key_on_keys.insert({dotted, it.first});
            } else {
                dotted_key_on_keys.insert({NOBODY_GROUP_SORTER, it.first});
            }
        }

        int count_all{};
        for (auto &it : dotted_key_on_keys) {
            auto val = _grouped_args[it.second];
            auto clean_key = it.first == NOBODY_GROUP_SORTER ? it.second : it.second.substr(it.first.length() + 1);

            int wf = 0, ws = 0, wa = 0, wt = 0, w = 0, valid_count = 0;
            for (auto &x : val) {
                if (x->hidden()) continue;
                valid_count++;
                w = x->title_long().length() + 2;
                if (w > wf) wf = w;
                w = x->title_short().length() + 1;
                if (w > ws) ws = w;
                w = 0;
                for (auto const &t : x->title_aliases()) {
                    if (w > 0) w += 1;
                    w += t.length() + 2;
                }
                if (w > wa) wa = w;
            }

            wt = wf + 2 + ws + 2 + wa + 2;
            if (wt < 43) wt = 43;

            if (valid_count == 0)
                continue;

            if (it.second != UNSORTED_GROUP) {
                int i = 0;
                for (auto &x : val) {
                    if (x->hidden()) continue;
                    i++;
                }
                if (i > 0) {
                    ss << ' ' << ' ';
                    std::stringstream tmp;
                    tmp << '[' << clean_key << ']';
                    ss << c.dim().s(tmp.str()) << std::endl;
                }
            }

            for (auto &x : val) {
                if (x->hidden()) continue;
                ss << "  " << std::left << std::setfill(' ');
                if (!x->title_long().empty()) {
                    w = x->title_long().length();
                    ss << '-' << '-' << c.underline().s(x->title_long());

                    if (!x->title_short().empty() || !x->title_aliases().empty()) ss << ", ";
                    else
                        ss << "  ";

                    w = wf - w - 2;
                    if (w > 0) ss << std::setw(w) << ' ';
                } else
                    ss << std::setw(wf + 2) << ' ';
                if (!x->title_short().empty()) {
                    w = x->title_short().length();
                    ss << '-' << x->title_short();
                    if (!x->title_aliases().empty()) ss << ", ";
                    else
                        ss << "  ";
                    w = ws - w - 1;
                    if (w > 0) ss << std::setw(w) << ' ';
                } else
                    ss << std::setw(ws + 2) << ' ';
                if (!x->title_aliases().empty()) {
                    w = 0;
                    std::stringstream tmp;
                    for (auto const &t : x->title_aliases()) {
                        if (w > 0) {
                            tmp << ',';
                        } else
                            w++;
                        tmp << '-' << '-' << t;
                    }
                    ss << std::setw(wa) << tmp.str();
                } else
                    ss << std::setw(wa) << ' ';

                w = wf + 2 + ws + 2 + wa;
                ss << std::setw(wt - w) << ' ';

                ss << c.dim().s(x->descriptions());

                auto se = x->env_vars_get();
                if (!se.empty()) {
                    w = 0;
                    std::stringstream tmp;
                    tmp << " (ENV: ";
                    for (auto const &t : se) {
                        if (w > 0) {
                            tmp << ',';
                        } else
                            w++;
                        tmp << t;
                    }
                    tmp << ")";
                    if (w > 0)
                        ss << c.dim().s(tmp.str());
                }

                auto sd = x->defaults();
                if (!sd.empty())
                    ss << c.dim().s(sd);

                // ss << wt << ',' << w << '|' << wf << ',' << ws << ',' << wa;

                ss << std::endl;

                count_all++;
            }
        }

        if (count_all == 0) {
            ss << c.dim().s("  (no options)") << '\n';
        }
    }


    //
    //
    //


    inline app::app(const_chars name, const_chars version, const_chars author,
                    const_chars copyright, const_chars description_,
                    const_chars examples)
        : _name(name)
        , _version(version)
        , _author(author)
        , _copyright(copyright)
    // , _description(description)
    // , _examples(examples)
    {
        this->description(description_);
        this->examples(examples);
        AppHolder::instance().put(this);
    }

    inline app app::create(const_chars name, const_chars version,
                           const_chars author,
                           const_chars copyright,
                           const_chars description,
                           const_chars examples) {
        auto x = app{name, version, author, copyright, description, examples};
        x.initialize_internal_commands();
        return x;
    }

    inline void app::initialize_internal_commands() {
        add_global_options(*this);
        add_generator_menu(*this);
    }

    inline void app::add_global_options(cmdr::opt::app &cli) {
        // using namespace cmdr::opt;

        const bool hide_sys_tools = false;

        // help

        cli += cmdr::opt::subcmd{}
                       .titles("help", "h", "info", "usages")
                       .description("display this help screen")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true)
                       .on_invoke([](cmd const &hit, string_array const &remain_args) -> int {
                           unused(hit);
                           unused(remain_args);
                           std::cout << "help, !!!\n";
                           cmdr::get_app().print_usages(nullptr);
                           return 0;
                       });

        cli += cmdr::opt::opt<bool>{}
                       .titles("help", "h", "?", "info", "usages")
                       .description("display this help screen")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true)
                       .on_hit([](cmd const &hit, arg const &hit_flag, string_array const &remain_args) -> details::Action {
                           unused(hit);
                           unused(hit_flag);
                           unused(remain_args);
                           // std::cout << "help, !!!\n";
                           // cmdr::get_app().print_usages(const_cast<cmd *>(&hit));
                           return details::RequestHelpScreen;
                       });

        // version

        cli += cmdr::opt::subcmd{}
                       .titles("version", "V", "versions", "ver")
                       .description("display the version information")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true);

        cli += cmdr::opt::opt<bool>{}
                       .titles("version", "V", "versions", "ver")
                       .description("display the version information")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true);

        // build-info

        cli += cmdr::opt::opt<bool>{}
                       .titles("build-info", "#", "bdinf")
                       .description("display the building information")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true);

        // more...

        cli += cmdr::opt::opt<bool>{}
                       .titles("verbose", "v")
                       .description("verbose mode")
                       .group(SYS_MGMT_GROUP);
        cli += cmdr::opt::opt<bool>{}
                       .titles("quiet", "q")
                       .description("quiet mode")
                       .group(SYS_MGMT_GROUP);

        cli += cmdr::opt::opt<bool>{}
                       .titles("debug", "D", "debug-mode")
                       .description("enable debugging mode for more verbose outputting")
                       .group(SYS_MGMT_GROUP);
        cli += cmdr::opt::opt<bool>{}
                       .titles("trace", "tr", "trace-mode")
                       .description("enable tracing mode for developer perspective")
                       .group(SYS_MGMT_GROUP);
    }

    inline void app::add_generator_menu(cmdr::opt::app &cli) {
        // using namespace cmdr::opt;

        // generators
        cli += cmdr::opt::subcmd{}
                       .titles("generator", "g", "gen")
                       .description("generators of this app (such as manual, markdown, ...)")
                       .group(SYS_MGMT_GROUP)
                       .opt(opt_dummy{}())
                       .opt(opt_dummy{}());
        {
            auto &t1 = *cli.last_added_command();
            t1 += cmdr::opt::subcmd{}
                          .titles("doc", "d", "markdown", "docx", "pdf", "tex")
                          .description("generate a markdown document, or: pdf/TeX/...")
                          .opt(opt_dummy{}())
                          .opt(opt_dummy{}());

            t1 += cmdr::opt::opt<int>{}
                          .titles("count", "c")
                          .description("set counter value")
                          .default_value(cmdr::support_types((int16_t)(3)));

            t1 += cmdr::opt::subcmd{}
                          .titles("manual", "m", "man")
                          .description("generate linux man page.")
                          .opt(opt_dummy{}())
                          .opt(opt_dummy{}());

            t1 += cmdr::opt::subcmd{}
                          .titles("shell", "s", "sh")
                          .description("generate the bash/zsh auto-completion script or install it.")
                          .opt(opt_dummy{}())
                          .opt(opt_dummy{}());
        }
    }

    inline app &app::operator+(const arg &a) {
        cmd::operator+(a);
        return *this;
    }

    inline app &app::operator+=(const arg &a) {
        cmd::operator+=(a);
        return *this;
    }

    inline app &app::operator+(const cmd &a) {
        cmd::operator+(a);
        return *this;
    }

    inline app &app::operator+=(const cmd &a) {
        cmd::operator+=(a);
        return *this;
    }

    inline string_array app::remain_args(parsing_pkg &pp, char *argv[], int i, int argc) {
        string_array a;
        for (auto &it : pp.non_commands) {
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

    inline details::Action app::process_command(app::parsing_pkg &pp, int argc, char *argv[]) {
        auto cmr = matching_command(pp);
        if (cmr.matched) {
            cmr.obj->hit_title(pp.title.c_str());
            pp.matched_commands.push_back(cmr.obj);
            if (cmr.obj->on_command_hit()) {
                auto rc = cmr.obj->on_command_hit()(
                        pp.last_matched_cmd(),
                        remain_args(argv, pp.index + 1, argc));
                if (rc < details::OK || rc >= details::Abortion)
                    return rc;
                if (rc == details::RequestHelpScreen)
                    pp.help_requesting = true;
            }
            return details::Continue;
        }
        if (cmr.should_abort)
            return details::Abortion;
        if (treat_unknown_input_command_as_error)
            return on_unknown_command_found(pp, cmr);
        pp.unknown_commands.push_back(argv[pp.index]);
        return details::Continue;
    }

    inline details::Action app::process_long_flag(app::parsing_pkg &pp, int argc, char *argv[]) {
        pp.title = pp.title.substr(2);
        auto fmr = matching_long_flag(pp);
        if (fmr.matched) {
            fmr.obj->hit_title(pp.title.c_str());
            pp.matched_flags.push_back(fmr.obj);
            if (fmr.obj->on_flag_hit()) {
                auto rc = fmr.obj->on_flag_hit()(
                        pp.last_matched_cmd(),
                        pp.last_matched_flg(),
                        remain_args(argv, pp.index + 1, argc));
                if (rc < details::OK || rc >= details::Abortion)
                    return rc;
                if (rc == details::RequestHelpScreen)
                    pp.help_requesting = true;
            }
            return details::Continue;
        }
        if (fmr.should_abort)
            return details::Abortion;
        pp.title = argv[pp.index];
        if (treat_unknown_input_flag_as_error)
            return on_unknown_long_flag_found(pp, fmr);
        pp.unknown_flags.push_back(pp.title);
        return details::Continue;
    }

    inline details::Action app::process_short_flag(app::parsing_pkg &pp, int argc, char *argv[]) {
        pp.title = pp.title.substr(1);
        auto fmr = matching_short_flag(pp);
        if (fmr.matched) {
            fmr.obj->hit_title(pp.title.c_str());
            pp.matched_flags.push_back(fmr.obj);
            if (fmr.obj->on_flag_hit()) {
                auto rc = fmr.obj->on_flag_hit()(
                        pp.last_matched_cmd(),
                        pp.last_matched_flg(),
                        remain_args(argv, pp.index + 1, argc));
                if (rc < details::OK || rc >= details::Abortion)
                    return rc;
                if (rc == details::RequestHelpScreen)
                    pp.help_requesting = true;
            }
            return details::Continue;
        }
        if (fmr.should_abort)
            return details::Abortion;
        pp.title = argv[pp.index];
        if (treat_unknown_input_flag_as_error)
            return on_unknown_short_flag_found(pp, fmr);
        pp.unknown_flags.push_back(pp.title);
        return details::Continue;
    }

    inline app::arg_matching_result app::matching_long_flag(app::parsing_pkg &pp) {
        arg_matching_result amr;
        for (auto it = pp.matched_commands.rbegin(); it != pp.matched_commands.rend(); it++) {
            auto c = (*it);
            details::indexed_args &li = c->_long_args;
            if (auto const &itz = li.find(pp.title); itz != li.end()) {
                amr.matched = true;
                amr.obj = itz->second;
                break;
            }
        }
        return amr;
    }

    inline app::arg_matching_result app::matching_short_flag(app::parsing_pkg &pp) {
        arg_matching_result amr;
        for (auto it = pp.matched_commands.rbegin(); it != pp.matched_commands.rend(); it++) {
            auto c = (*it);
            details::indexed_args &li = c->_short_args;
            if (auto const &itz = li.find(pp.title); itz != li.end()) {
                amr.matched = true;
                amr.obj = itz->second;
                break;
            }
        }
        return amr;
    }

    inline details::Action app::on_unknown_long_flag_found(app::parsing_pkg &pp, arg_matching_result &fmr) {
        unused(pp);
        unused(fmr);
        std::cerr << "Unknown long flag: " << std::quoted(pp.title);
        auto &c = pp.last_matched_cmd();
        if (c.valid())
            std::cerr << " under matched command: " << std::quoted(c.title());
        std::cerr << '\n';
        return details::Abortion;
    }

    inline details::Action app::on_unknown_short_flag_found(app::parsing_pkg &pp, arg_matching_result &fmr) {
        unused(pp);
        unused(fmr);
        std::cerr << "Unknown short flag: " << std::quoted(pp.title);
        auto &c = pp.last_matched_cmd();
        if (c.valid())
            std::cerr << " under matched command: " << std::quoted(c.title());
        std::cerr << '\n';
        return details::Abortion;
    }

    inline app::cmd_matching_result app::matching_command(app::parsing_pkg &pp) {
        cmd_matching_result cmr;
        cmd &c = pp.curr_command();
        details::indexed_commands &li = c._long_commands;
        if (auto const &it = li.find(pp.title); it != li.end()) {
            cmr.matched = true;
            cmr.obj = it->second;
        } else {
            li = c._short_commands;
            auto const &it1 = li.find(pp.title);
            if (it1 != li.end()) {
                cmr.matched = true;
                cmr.obj = it1->second;
            }
        }
        return cmr;
    }

    inline details::Action app::on_unknown_command_found(parsing_pkg &pp, cmd_matching_result &cmr) {
        unused(pp);
        unused(cmr);
        std::cerr << "Unknown command: " << std::quoted(pp.title);
        auto &c = pp.last_matched_cmd();
        if (c.valid())
            std::cerr << " under matched command: " << std::quoted(c.title());
        std::cerr << '\n';
        return details::Abortion;
    }

    inline int app::invoke_command(cmd &c, string_array remain_args, parsing_pkg &pp) {
        unused(pp);
        unused(c);
        unused(remain_args);
        int rc{0};
        if (c.on_pre_invoke())
            rc = c.on_pre_invoke()(c, remain_args);
        if (rc == 0) {
            if (c.on_invoke())
                rc = c.on_invoke()(c, remain_args);
            else
                std::cout << "INVOKE: " << std::quoted(c.title()) << ".\n";
        }
        if (c.on_post_invoke())
            c.on_post_invoke()(c, remain_args);
        return rc;
    }

    inline void app::print_cmd(std::ostream &ss, cmdr::terminal::colors::colorize &c, cmd *cc, std::string const &app_name, std::string const &exe_name) {
        if (!cc->description().empty()) {
            ss << std::endl
               << "DESCRIPTION" << std::endl
               << string::pad_left(cc->description()) << std::endl;
        }
        if (!cc->examples().empty()) {
            ss << std::endl
               << "EXAMPLES" << std::endl
               << string::pad_left(string::reg_replace(cc->examples(), "~", exe_name)) << std::endl;
        }
        ss << std::endl
           << "USAGE" << std::endl;
        ss << string::pad_left(exe_name, 2) << " [commands] [options] [Tail Args]" << std::endl;

        ss << std::endl
           << "COMMANDS" << std::endl;
        cc->print_commands(ss, c);

        ss << std::endl
           << "OPTIONS" << std::endl;
        cc->print_flags(ss, c);

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

    inline int app::run(int argc, char *argv[]) {
        // std::cout << "Hello, World!" << std::endl;

        parsing_pkg pp{this};
        pp.matched_commands.push_back(this);

        for (int i = 1; i < argc; i++) {
            pp.title = argv[i];
            pp.index = i;

            if (pp.title[0] == '-') {
                // flag?
                if (pp.title[1] == '-') {
                    if (pp.title[2] == 0) {
                        pp.passthru_flag = true;
                        continue;
                    }

                    // long
                    auto rc = process_long_flag(pp, argc, argv);
                    if (rc < details::OK || rc >= details::Abortion)
                        return 0;
                    continue;
                }

                // short flag
                auto rc = process_short_flag(pp, argc, argv);
                if (rc < details::OK || rc >= details::Abortion)
                    return 0;
                continue;
            }

            // command
            auto rc = process_command(pp, argc, argv);
            if (rc < details::OK || rc >= details::Abortion)
                return 0;
        }

        if (pp.help_requesting) {
            print_usages(&pp.curr_command());
            return 0;
        }

        if (auto cc = pp.last_matched_cmd(); cc.valid()) {
            if (cc.no_sub_commands()) {
                // invoking cc
                return invoke_command(cc, remain_args(pp, argv, pp.index, argc), pp);
            }

            pp.help_requesting = true;
            print_usages(&pp.curr_command());
            return 0;
        }
        return 0;
    }

} // namespace cmdr::opt


#endif //CMDR_CXX11_CMDR_IMPL_HH
