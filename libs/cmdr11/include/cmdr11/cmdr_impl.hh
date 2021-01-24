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
    inline const arg &cmd::operator[](const_chars long_title) const { return const_cast<cmd&>(*this).find_flag(long_title); }

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
    inline cmd const &cmd::operator()(const_chars long_title, bool extensive) const { return const_cast<cmd&>(*this).find_command(long_title, extensive); }

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

            if (valid_count == 0) continue;

            if (it.second != UNSORTED_GROUP) {
                int i = 0;
                for (auto &x : val) {
                    if (x->hidden()) continue;
                    i++;
                }
                if (i > 0)
                    ss << ' ' << ' ' << '[' << clean_key << ']' << std::endl;
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
            }
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

            if (valid_count == 0) continue;

            if (it.second != UNSORTED_GROUP) {
                int i = 0;
                for (auto &x : val) {
                    if (x->hidden()) continue;
                    i++;
                }
                if (i > 0)
                    ss << ' ' << ' ' << '[' << clean_key << ']' << std::endl;
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
            }
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
        return app{name, version, author, copyright, description, examples};
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

    inline int app::run(int argc, char *argv[]) {
        // std::cout << "Hello, World!" << std::endl;

        for (int i = 1; i < argc; i++) {
            if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
                printf("Usage: App <options>\nOptions are:\n");
                printf("Option list goes here");
                exit(0);
            } else if (!strcmp(argv[i], "-c") || !strcmp(argv[i], "--custom")) {
                printf("argument accepted");
            } else {
                if (i == argc - 1) {
                    break;
                }
            }
        }

        print_usages();

        return 0;
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
} // namespace cmdr::opt


#endif //CMDR_CXX11_CMDR_IMPL_HH
