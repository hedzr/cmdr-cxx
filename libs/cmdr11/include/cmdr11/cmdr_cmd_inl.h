//
// Created by Hedzr Yeh on 2021/1/25.
//

#ifndef CMDR_CXX11_CMDR_CMD_INL_H
#define CMDR_CXX11_CMDR_CMD_INL_H

#include <exception>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>


#include "cmdr_cmn.hh"

#include "cmdr_arg.hh"
#include "cmdr_cmd.hh"

#include "cmdr_app.hh"

#include "cmdr_internals.hh"
#include "cmdr_terminal.hh"


namespace cmdr::opt {


    inline bas &bas::owner(cmd *o) {
        _owner = o;
        return (*this);
    }
    inline cmd const *bas::owner() const { return _owner; }
    inline cmd *bas::owner() { return _owner; }
    inline cmd const *bas::root() const {
        cmd *p = this->_owner, *last;
        do {
            last = p;
        } while ((p = p->owner()) != nullptr);
        return last;
    }
    inline cmd *bas::root() {
        cmd *p = this->_owner, *last;
        do {
            last = p;
        } while ((p = p->owner()) != nullptr);
        return last;
    }

    inline std::string bas::dotted_key() const {
        std::vector<std::string> a;
        bas const *p = this;
        do {
            a.insert(a.begin(), p->_long);
        } while ((p = p->owner()) != nullptr);
        return string::join(a, '.');
    }

    //
    //
    //


    inline cmd &cmd::operator+(arg const &a) {
        add(a);
        return (*this);
    }
    inline cmd &cmd::operator+=(arg const &a) {
        add(a);
        return (*this);
    }
    inline cmd &cmd::operator+(cmd const &a) {
        add(a);
        return (*this);
    }
    inline cmd &cmd::operator+=(cmd const &a) {
        add(a);
        return (*this);
    }

    inline void cmd::add(arg const &a) {
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

            if (ptr->default_value().get() == nullptr) {
                ptr->default_value(vars::variable{false});
                cmdr_verbose_debug("  -> set arg default value to false. [key: '%s'] [cmd: \"%s\"]", a.title_long().c_str(), this->title_sequences().c_str());
            }

            if (_grouped_args.find(gn) == _grouped_args.end())
                _grouped_args.emplace(gn, types::arg_pointers{});
            _grouped_args[gn].push_back(ptr);

            _indexed_args.insert({a.title_long(), ptr});

            if (!a.title_short().empty()) {
                if (auto const &it = _short_args.find(a.title_short()); it != _short_args.end())
                    // cmdr_throw_line(std::string("duplicated short flag found: -") + a.title_short() + std::string(" ") + a.title_long() + std::string(", parent cmd = ") + title_sequences());
                    cmdr_throw_as(dup_short_flag_found, &a, this);
                _short_args.insert({a.title_short(), ptr});
            }

            if (auto const &it = _long_args.find(a.title_long()); it != _long_args.end())
                // cmdr_throw_line(std::string("duplicated long flag found: --") + a.title_long() + std::string(", parent cmd = ") + title_sequences());
                cmdr_throw_as(dup_short_flag_found, &a, this);
            _long_args.insert({a.title_long(), ptr});

            for (auto const &itz : a.title_aliases()) {
                if (auto const &it = _long_args.find(itz); it != _long_args.end())
                    // cmdr_throw_line(std::string("duplicated alias flag found: -") + itz + std::string(", parent cmd = ") + title_sequences());
                    cmdr_throw_as(dup_alias_flag_found, itz.c_str(), &a, this);
                _long_args.insert({itz, ptr});
            }

            _last_added_arg = ptr;
            ptr->owner(this);

            // auto dk = dotted_key();
            cmdr::get_app().on_arg_added(ptr);

            // std::cout << gn << ',' << _grouped_args[gn].size() << '\n';
        }
    }

    inline void cmd::add(cmd const &a) {
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
                _grouped_commands.emplace(gn, types::cmd_pointers{});
            auto &list = _grouped_commands[gn];
#if defined(_DEBUG)
            auto size = list.size();
#endif
            list.push_back(ptr);
#if defined(_DEBUG)
            CMDR_ASSERT(list.size() == size + 1);
#endif

            _indexed_commands.insert({a.title_long(), ptr});
            if (!a.title_short().empty()) {
                if (auto const &it = _short_commands.find(a.title_short()); it != _short_commands.end())
                    // cmdr_throw_line(std::string("duplicated short command found: ") + a.title_short() + std::string(" ") + a.title_long() + std::string(", parent cmd = ") + title_sequences());
                    cmdr_throw_as(dup_short_cmd_found, &a, this);
                _short_commands.insert({a.title_short(), ptr});
            }
            if (auto const &it = _long_commands.find(a.title_long()); it != _long_commands.end())
                // cmdr_throw_line(std::string("duplicated long command found: ") + a.title_long() + std::string(", parent cmd = ") + title_sequences());
                cmdr_throw_as(dup_long_cmd_found, &a, this);
            _long_commands.insert({a.title_long(), ptr});
            for (auto const &itz : a.title_aliases()) {
                if (auto const &it = _long_commands.find(itz); it != _long_commands.end())
                    // cmdr_throw_line(std::string("duplicated alias command found: ") + itz + std::string(", parent cmd = ") + title_sequences());
                    cmdr_throw_as(dup_alias_cmd_found, itz.c_str(), &a, this);
                _long_commands.insert({itz, ptr});
            }

            _last_added_command = ptr;
            ptr->owner(this);

            cmdr::get_app().on_cmd_added(ptr);
        }
    }


    inline std::string cmd::title_sequences() const {
        std::vector<std::string> cmds;
        auto pcc = this;
        while (pcc && pcc->owner()) {
            cmds.insert(cmds.begin(), pcc->hit_count() > 0 ? pcc->hit_title() : pcc->title_long());
            pcc = pcc->owner();
        }
        return string::join(cmds, ' ');
    }


    inline arg &cmd::operator[](const_chars long_title) { return find_flag(long_title); }

    inline const arg &cmd::operator[](const_chars long_title) const { return const_cast<cmd &>(*this).find_flag(long_title); }

    inline cmd &cmd::operator()(const_chars long_title, bool extensive) { return find_command(long_title, extensive); }

    inline cmd const &cmd::operator()(const_chars long_title, bool extensive) const { return const_cast<cmd &>(*this).find_command(long_title, extensive); }


    /**
     * @brief 
     * @param long_title is a dotted string like 'app.cli.server.port',
     *     or just a key for this command level such as 'port'. NOTE that
     *     'app.cli' is DEFAULT_CLI_KEY_PREFIX.
     *     Any prefixes like '-' or '--' will be stripped automatically.
     * @param extensive  whether or not do match with short/alias titles
     * @return 
     */
    inline arg &cmd::find_flag(const_chars long_title, bool extensive) {
        std::stringstream st;
        //if (!_long.empty())
        //    st << _long << '.';
        if (is_leading_switch_char(long_title)) long_title++;
        if (is_leading_switch_char(long_title)) long_title++;
        st << long_title;
        auto s = st.str();

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

    inline arg const *cmd::find_flag(std::string const &long_title, bool extensive) const {
        auto s = long_title;
        if (is_leading_switch_char(s)) s = s.substr(1);
        if (is_leading_switch_char(s)) s = s.substr(1);

        auto it = _indexed_args.find(s);
        if (it != _indexed_args.end())
            return (*it).second;
        if (extensive) {
            for (auto itz : _indexed_args) {
                if (itz.second->title_short() == long_title)
                    return (itz).second;
                for (auto &k : itz.second->title_aliases()) {
                    if (k == long_title)
                        return (itz).second;
                }
            }
        }
        return nullptr;
    }

    /**
     * @brief 
     * @param long_title is a dotted string like 'app.cli.server.install.systemd', 
     *   it means the sub-command sequences 'server install systemd', in which
     *   'app.cli' is DEFAULT_CLI_KEY_PREFIX. Or,
     *   a long_title can be just a simple key for this command level such as 'systemd'.
     * @param extensive whether or not do match with short/alias titles
     * @return 
     */
    inline cmd &cmd::find_command(const_chars long_title, bool extensive) {
        std::stringstream st;
        //if (!_long.empty())
        //    st << _long << '.';
        st << long_title;

        auto s = st.str();
        auto it = _indexed_commands.find(s);
        if (it != _indexed_commands.end()) {
            auto cc = (*it).second;
#if defined(_DEBUG)
            bool found = false;
            for (auto const &itl : _grouped_commands[cc->group_name()]) {
                if (itl == cc) {
                    found = true;
                    break;
                }
            }
            CMDR_ASSERT(found);
#endif
            return *cc;
        }
        if (extensive) {
            for (const auto &itz : _indexed_commands) {
                if (itz.second->title_short() == s)
                    return *((itz).second);
                for (auto &k : itz.second->title_aliases()) {
                    if (k == s)
                        return *((itz).second);
                }
            }
        }
        return null_command();
    }

    inline cmd const *cmd::find_command(std::string const &long_title, bool extensive) const {
        auto it = _indexed_commands.find(long_title);
        if (it != _indexed_commands.end()) {
            auto cc = (*it).second;
            return cc;
        }
        if (extensive) {
            for (const auto &itz : _indexed_commands) {
                if (itz.second->title_short() == long_title)
                    return (itz).second;
                for (auto &k : itz.second->title_aliases()) {
                    if (k == long_title)
                        return (itz).second;
                }
            }
        }
        return nullptr;
    }

    inline int cmd::run(int argc, char *argv[]) {
        UNUSED(argc, argv);
        return 0;
    }


    inline void cmd::walk_args(std::function<void(arg &)> const &cb) {
        for (auto &it : _grouped_commands) {
            for (auto &z : it.second) {
                z->walk_args(cb);
            }
        }

        for (auto &it : this->_grouped_args) {
            for (auto &z : it.second) {
                cb(*z);
            }
        }
    }

    inline void cmd::print_commands(std::ostream &ss, cmdr::terminal::colors::colorize &c, int &wt, bool grouped, int level) const {
        UNUSED(grouped, level);
        auto fg = vars::store::_dim_text_fg;
        auto dim = vars::store::_dim_text_dim;
        auto [th, tw] = terminal::terminfo::get_win_size();

        std::set<std::string> keys;
        std::map<std::string, std::string> dotted_key_on_keys;
        auto nobody_num = std::stoi(NOBODY_GROUP_SORTER);
        for (auto &it : _grouped_commands) {
            auto &title = it.first;
            keys.insert(title);
            auto ptr = title.find('.');
            if (ptr != std::string::npos) {
                auto dotted = title.substr(0, ptr);
                dotted_key_on_keys.insert({dotted, title});
            } else {
                dotted_key_on_keys.insert({std::to_string(nobody_num++), title});
            }
        }

        int count_all{};
        // if (wt <= 0)
        {
            for (auto &it : dotted_key_on_keys) {
                auto const &val_it = _grouped_commands.find(it.second);
                if (val_it == _grouped_commands.end()) continue;
                auto const &val = val_it->second;
                auto clean_key = string::has_prefix(it.second, it.first) ? it.second.substr(it.first.length() + 1) : it.second;

                int wf = 0, ws = 0, wa = 0, w = 0, valid_count = 0;
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

                if (wf > 0) {
                    auto wt_tmp = wf + 2 + ws + 2 + wa + 2;
                    if (wt < wt_tmp)
                        wt = wt_tmp;
                }
            }
        }

        for (auto &it : dotted_key_on_keys) {
            auto const &val_it = _grouped_commands.find(it.second);
            if (val_it == _grouped_commands.end()) continue;
            auto const &val = val_it->second;
            auto clean_key = string::has_prefix(it.second, it.first) ? it.second.substr(it.first.length() + 1) : it.second;

            int wf = 0, ws = 0, wa = 0, w = 0, valid_count = 0;
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

            if (valid_count == 0)
                continue;

            int level_pad = 0;
            if (it.second != UNSORTED_GROUP) {
                int i = 0;
                for (auto &x : val) {
                    if (x->hidden()) continue;
                    i++;
                }
                if (i > 0) {
                    if (level >= 0)
                        ss << std::string(level * 2, ' ') << '-' << ' ';
                    else
                        ss << ' ' << ' ';
                    std::stringstream tmp;
                    tmp << '[' << clean_key << ']';
                    ss << c.fg(fg).dim(dim).s(tmp.str()) << '\n';
                    level_pad++;
                }
            }

            for (auto &x : val) {
                if (x->hidden()) continue;

                if (level >= 0)
                    ss << std::string((level + level_pad) * 2, ' ') << '*' << ' ';
                else
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
                } else if (wa > 0)
                    ss << std::setw(wa) << ' ';

                w = wf + 2 + ws + 2 + wa;
                ss << std::setw((wt < 43 ? 43 : wt) - w - (level >= 0 ? level : 0)) << ' ';

                auto d = x->descriptions();
                auto rw = (std::size_t)((tw <= 0 ? 1000 : tw) - wt - 2);
                w = 0;
                do {
                    if (w++ != 0)
                        ss << std::string(wt + 2, ' ');
                    auto t = std::min(rw, d.length());
                    ss << c.fg(fg).dim(dim).s(d.substr(0, t));
                    d = d.substr(t);
                } while (!d.empty());

                // ss // << wt << ',' << level << ','
                //         << c.fg(fg).dim(dim).s(x->descriptions())
                //         // << wt << ',' << w << '|' << wf << ',' << ws << ',' << wa
                //         << '\n';

                // ss << wt << ',' << w << '|' << wf << ',' << ws << ',' << wa;

                ss << '\n';

                count_all++;

                if (level >= 0 && !x->no_sub_commands()) {
                    x->print_commands(ss, c, wt, grouped, level + level_pad + 1);
                }
            }
        }

        if (count_all == 0) {
            ss << c.fg(fg).dim(dim).s("  (no sub-commands)") << '\n';
        }
    }


    inline void cmd::print_flags(std::ostream &ss, cmdr::terminal::colors::colorize &c, int &wt, bool grouped, int level) const {
        UNUSED(grouped, level);
        auto fg = vars::store::_dim_text_fg;
        auto dim = vars::store::_dim_text_dim;
        auto [th, tw] = terminal::terminfo::get_win_size();

        std::set<std::string> keys;
        std::map<std::string, std::string> dotted_key_on_keys;
        auto nobody_num = std::stoi(NOBODY_GROUP_SORTER);
        for (auto &it : _grouped_args) {
            keys.insert(it.first);
            auto ptr = it.first.find('.');
            if (ptr != std::string::npos) {
                auto dotted = it.first.substr(0, ptr);
                dotted_key_on_keys.insert({dotted, it.first});
            } else {
                dotted_key_on_keys.insert({std::to_string(nobody_num++), it.first});
            }
        }

        int count_all{};
        // if (wt <= 0)
        {
            for (auto &it : dotted_key_on_keys) {
                auto const &val_it = _grouped_args.find(it.second);
                if (val_it == _grouped_args.end()) continue;
                auto const &val = val_it->second;
                auto clean_key = string::has_prefix(it.second, it.first) ? it.second.substr(it.first.length() + 1) : it.second;

                int wf = 0, ws = 0, wa = 0, w = 0, valid_count = 0;
                for (auto &x : val) {
                    if (x->hidden()) continue;
                    valid_count++;
                    w = x->title_long().length() + 2;
                    if (!x->placeholder().empty())
                        w += x->placeholder().length() + 1;
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

                if (wf > 0) {
                    auto wt_tmp = wf + 2 + ws + 2 + wa + 2;
                    if (wt < wt_tmp)
                        wt = wt_tmp;
                }
            }
        }

        for (auto &it : dotted_key_on_keys) {
            auto const &val_it = _grouped_args.find(it.second);
            if (val_it == _grouped_args.end()) continue;
            auto const &val = val_it->second;
            auto clean_key = string::has_prefix(it.second, it.first) ? it.second.substr(it.first.length() + 1) : it.second;

            int wf = 0, ws = 0, wa = 0, w = 0, valid_count = 0;
            for (auto &x : val) {
                if (x->hidden()) continue;
                valid_count++;
                w = x->title_long().length() + 2;
                if (!x->placeholder().empty())
                    w += x->placeholder().length() + 1;
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
                    ss << c.fg(fg).dim(dim).s(tmp.str()) << '\n';
                }
            }

            for (auto &x : val) {
                if (x->hidden()) continue;
                ss << ' ' << ' ' << std::left << std::setfill(' ');
                if (!x->title_long().empty()) {
                    w = x->title_long().length();
                    ss << '-' << '-' << c.underline().s(x->title_long());
                    if (!x->placeholder().empty())
                        ss << '=' << x->placeholder();

                    if (!x->title_short().empty() || !x->title_aliases().empty()) ss << ", ";
                    else
                        ss << ' ' << ' ';

                    w = wf - w - 2;
                    if (!x->placeholder().empty())
                        w -= x->placeholder().length() + 1;

                    if (w > 0) ss << std::setw(w) << ' ';
                } else
                    ss << std::setw(wf + 2) << ' ';
                if (!x->title_short().empty()) {
                    w = x->title_short().length();
                    ss << '-' << x->title_short();
                    if (!x->title_aliases().empty()) ss << ", ";
                    else
                        ss << ' ' << ' ';
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
                } else if (wa > 0)
                    ss << std::setw(wa) << ' ';

                w = wf + 2 + ws + 2 + wa;
                ss << std::setw((wt < 43 ? 43 : wt) - w - (level >= 0 ? level * 2 : 0)) << ' ';

                auto d = x->descriptions();
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
                        d += tmp.str();
                }
                auto sd = x->defaults();
                if (!sd.empty())
                    d += sd;
                auto rw = (std::size_t)((tw <= 0 ? 1000 : tw) - wt - 2);
                w = 0;
                do {
                    if (w++ != 0)
                        ss << std::string(wt + 2, ' ');
                    auto t = std::min(rw, d.length());
                    ss << c.fg(fg).dim(dim).s(d.substr(0, t));
                    d = d.substr(t);
                } while (!d.empty());
                // ss // << w << ',' << wt << ',' << c.fg(fg).dim(dim).s(x->descriptions());

                // ss << wt << ',' << w << '|' << wf << ',' << ws << ',' << wa;

                ss << '\n';

                count_all++;
            }
        }

        if (count_all == 0) {
            ss << c.fg(fg).dim(dim).s("  (no options)") << '\n';
        }
    }


} // namespace cmdr::opt

#endif //CMDR_CXX11_CMDR_CMD_INL_H
