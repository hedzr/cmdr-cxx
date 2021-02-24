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


#include "cmdr_defs.hh"

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

            auto *ptr = &_all_args.back();

            if (ptr->default_value().get() == nullptr) {
                ptr->default_value(vars::variable{false});
                cmdr_verbose_debug("  -> set arg default value to false. [key: '%s'] [cmd: \"%s\"]", a.title_long().c_str(), this->title_sequences().c_str());
            }

            if (_grouped_args.find(gn) == _grouped_args.end())
                _grouped_args.emplace(gn, types::arg_pointers{});
#ifdef _DEBUG
            auto size_before = _grouped_args[gn].size();
            _grouped_args[gn].push_back(ptr);
            auto size_after = _grouped_args[gn].size();
            assert(size_after == size_before + 1);
#else
            _grouped_args[gn].push_back(ptr);
#endif
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

            if (ptr->is_toggleable()) {
                if (ptr->default_value()->type() == typeid(bool)) {
                    auto const &key = ptr->toggle_group_name();
                    if (auto it = _hit_toggle_groups.find(key);
                        it == _hit_toggle_groups.end() || ptr->default_value()->as<bool>() == true) {
                        _hit_toggle_groups[key] = ptr->title_long();
                    }
                    if (auto it = _toggle_groups.find(key); it == _toggle_groups.end())
                        _toggle_groups.emplace(key, types::arg_pointers{});
                    _toggle_groups[key].push_back(ptr);
                }
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

            auto *ptr = &_all_commands.back();

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
        std::vector<std::string> commands;
        const auto *pcc = this;
        while (pcc && pcc->owner()) {
            commands.insert(commands.begin(), pcc->hit_count() > 0 ? pcc->hit_title() : pcc->title_long());
            pcc = pcc->owner();
        }
        return string::join(commands, ' ');
    }

    inline void cmd::toggle_group_set(std::string const &key, arg *a) {
        _hit_toggle_groups[key] = a->title_long();
    }

    inline std::string const &cmd::toggle_group(std::string const &key) const {
        return _hit_toggle_groups.at(key);
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
            auto *cc = (*it).second;
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
            auto *cc = (*it).second;
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


    //#pragma clang diagnostic push
    //#pragma ide diagnostic ignored "misc-no-recursion"
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
    //#pragma clang diagnostic pop


    namespace detail {

        inline std::map<std::string, std::string> sort_keys(types::grouped_cmd_list const &grouped_commands) {
            std::set<std::string> keys;
            std::map<std::string, std::string> dotted_key_on_keys;
            auto nobody_num = std::stoi(NOBODY_GROUP_SORTER);
            for (auto &it : grouped_commands) {
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
            return dotted_key_on_keys;
        }

        inline std::map<std::string, std::string> sort_keys(types::grouped_arg_list const &grouped_args) {
            std::set<std::string> keys;
            std::map<std::string, std::string> dotted_key_on_keys;
            auto nobody_num = std::stoi(NOBODY_GROUP_SORTER);
            for (auto &it : grouped_args) {
                keys.insert(it.first);
                auto ptr = it.first.find('.');
                if (ptr != std::string::npos) {
                    auto dotted = it.first.substr(0, ptr);
                    dotted_key_on_keys.insert({dotted, it.first});
                } else {
                    dotted_key_on_keys.insert({std::to_string(nobody_num++), it.first});
                }
            }
            return dotted_key_on_keys;
        }

        inline auto _out_group_name(std::string const &group_name,
                                    std::string &clean_key,
                                    types::cmd_pointers const &val,
                                    terminal::colors::colorize &c,
                                    terminal::colors::colorize::Colors256 fg,
                                    bool dim,
                                    bool show_hidden_items,
                                    int level,
                                    int &level_pad) {
            std::ostringstream ss;
            if (group_name != UNSORTED_GROUP) {
                int i = 0;
                for (auto &x : val) {
                    if (!show_hidden_items && x->hidden()) continue;
                    i++;
                }
                if (i > 0) {
                    if (level >= 0)
                        ss << std::string((std::size_t) level * 2, ' ') << '-' << ' ';
                    else
                        ss << ' ' << ' ';
                    std::stringstream tmp;
                    tmp << '[' << clean_key << ']';
                    ss << c.fg(fg).dim(dim).s(tmp.str()) << '\n';
                    level_pad++;
                }
            }
            return ss.str();
        }

        inline auto calc_widths(bool show_hidden_items, types::cmd_pointers const &val) {
            int wf = 0, ws = 0, wa = 0, w = 0, valid_count = 0;
            for (auto &x : val) {
                if (!show_hidden_items && x->hidden()) continue;
                valid_count++;
                w = (int) x->title_long().length();
                if (w > wf) wf = w;
                w = (int) x->title_short().length();
                if (w > ws) ws = w;
                w = 0;
                for (auto const &t : x->title_aliases()) {
                    if (w > 0) w += 1;
                    w += (int) t.length();
                }
                if (w > wa) wa = w;
            }
            return std::make_tuple(wf, ws, wa, w, valid_count);
        }

        inline void populate_tab_stop_width(std::map<std::string, std::string> const &dotted_key_on_keys,
                                            types::grouped_cmd_list const &grouped_commands,
                                            bool show_hidden_items,
                                            int &wt) {
            for (auto &it : dotted_key_on_keys) {
                auto const &val_it = grouped_commands.find(it.second);
                if (val_it == grouped_commands.end()) continue;
                auto const &val = val_it->second;
                auto clean_key = string::has_prefix(it.second, it.first) ? it.second.substr(it.first.length() + 1) : it.second;

                auto [wf, ws, wa, w, valid_count] = calc_widths(show_hidden_items, val);

                if (wf > 0) {
                    auto wt_tmp = wf + 2 + ws + 2 + wa + 2;
                    if (wt < wt_tmp)
                        wt = wt_tmp;
                }
            }
        }

        inline auto _out_group_name(std::string const &group_name,
                                    std::string &clean_key,
                                    types::arg_pointers const &val,
                                    terminal::colors::colorize &c,
                                    terminal::colors::colorize::Colors256 fg,
                                    bool dim,
                                    bool show_hidden_items) {
            std::ostringstream ss;
            if (group_name != UNSORTED_GROUP) {
                int i = 0;
                for (auto &x : val) {
                    if (!show_hidden_items && x->hidden()) continue;
                    i++;
                }
                if (i > 0) {
                    ss << ' ' << ' ';
                    std::stringstream tmp;
                    tmp << '[' << clean_key << ']';
                    ss << c.fg(fg).dim(dim).s(tmp.str()) << '\n';
                }
            }
            return ss.str();
        }

        inline auto calc_widths(bool show_hidden_items, types::arg_pointers const &val) {
            int wf = 0, ws = 0, wa = 0, w = 0, valid_count = 0;
            for (auto &x : val) {
                if (!show_hidden_items && x->hidden()) continue;
                valid_count++;
                w = (int) x->title_long().length() + 2;
                if (!x->placeholder().empty())
                    w += (int) x->placeholder().length() + 1;
                if (w > wf) wf = w;
                w = (int) x->title_short().length() + 1;
                if (w > ws) ws = w;
                w = 0;
                for (auto const &t : x->title_aliases()) {
                    if (w > 0) w += 1;
                    w += (int) t.length() + 2;
                }
                if (w > wa) wa = w;
            }
            return std::make_tuple(wf, ws, wa, w, valid_count);
        }

        inline void populate_tab_stop_width(std::map<std::string, std::string> const &dotted_key_on_keys,
                                            types::grouped_arg_list const &grouped_args,
                                            bool show_hidden_items,
                                            int &wt) {
            for (auto &it : dotted_key_on_keys) {
                auto const &val_it = grouped_args.find(it.second);
                if (val_it == grouped_args.end()) continue;
                auto const &val = val_it->second;
                auto clean_key = string::has_prefix(it.second, it.first) ? it.second.substr(it.first.length() + 1) : it.second;

                auto [wf, ws, wa, w, valid_count] = calc_widths(show_hidden_items, val);

                if (wf > 0) {
                    auto wt_tmp = wf + 2 + ws + 2 + wa + 2;
                    if (wt < wt_tmp)
                        wt = wt_tmp;
                }
            }
        }

        inline std::string _os_env_vars(arg *x) {
            std::stringstream tmp;
            auto se = x->env_vars_get();
            if (!se.empty()) {
                int w = 0;
                tmp << " (ENV: ";
                for (auto const &t : se) {
                    if (w > 0) {
                        tmp << ',';
                    } else {
                        w++;
                    }
                    tmp << t;
                }
                tmp << ")";
            }
            return tmp.str();
        }

        inline std::string _out_desc(std::string const &desc,
                                     cmdr::terminal::colors::colorize &c,
                                     cmdr::terminal::colors::colorize::Colors256 fg,
                                     bool dim,
                                     int tw,
                                     int wt_real) {
            std::ostringstream ss;
            auto d = desc;
            auto rw = (std::size_t)((std::size_t)(tw <= 0 ? 1000 : tw) - wt_real - 2);
            int w = 0;
            do {
                if (w++ != 0) {
                    ss << '\n'
                       << std::string((std::size_t) wt_real + 2, ' ');
                }
                auto t = cmdr::cross::min(rw, d.length());
                auto s1 = d.substr(0, t);
                ss << c.fg(fg).dim(dim).s(d.substr(0, t));
                d = string::trim_left_space(d.substr(t));
            } while (!d.empty());
            return ss.str();
        }

        inline std::string _out_comma_space(bool title_is_empty, bas *x,
                                            cmdr::terminal::colors::colorize &c,
                                            cmdr::terminal::colors::colorize::Colors256 fg,
                                            bool dim,
                                            bool underline,
                                            int &escaped_chars) {
            if (title_is_empty) {
                return "  "; // ss << ' ' << ' ';
            } else {
                if (x->hidden()) {
                    std::ostringstream os1;
                    os1 << c.fg(fg).dim(dim).s(", ");
                    auto s1 = os1.str();
                    escaped_chars += (int) (s1.length() - 2);
                    return s1;
                } else {
                    return ", ";
                }
            }
            UNUSED(underline);
        }

        inline std::string _out_placeholder(arg *x,
                                            cmdr::terminal::colors::colorize &c,
                                            cmdr::terminal::colors::colorize::Colors256 fg,
                                            bool dim,
                                            bool underline,
                                            int &escaped_chars) {
            std::ostringstream osr;
            if (!x->placeholder().empty()) {
                if (x->hidden()) {
                    std::ostringstream os1, os2;
                    os1 << c.fg(fg).dim(dim).s("=");
                    auto s1 = os1.str();
                    os2 << c.fg(fg).dim(dim).s(x->placeholder());
                    auto s2 = os2.str();
                    escaped_chars += (int) (s1.length() - 1 + s2.length() - x->placeholder().length());
                    osr << s1 << s2;
                } else {
                    osr << '=' << x->placeholder();
                }
                UNUSED(underline);
            }
            return osr.str();
        }

        inline std::string _out_title_aliases(bas *x,
                                              cmdr::terminal::colors::colorize &c,
                                              cmdr::terminal::colors::colorize::Colors256 fg,
                                              bool dim, bool underline,
                                              bool right_align,
                                              int wa,
                                              int &escaped_chars,
                                              bool flag) {
            std::ostringstream osr;
            if (!x->title_aliases().empty()) {
                int w = 0;
                std::stringstream tmp;
                for (auto const &t : x->title_aliases()) {
                    if (w > 0) {
                        tmp << ',';
                    } else
                        w++;
                    if (flag) tmp << '-' << '-';
                    tmp << t;
                }

                if (!right_align) osr << std::left;

                if (x->hidden()) {
                    std::ostringstream os1;
                    auto v = tmp.str();
                    os1 << c.fg(fg).dim(dim).s(v);
                    auto s1 = os1.str();
                    escaped_chars += (int) (s1.length() - v.length());
                    osr << std::setw(wa + s1.length() - v.length()) << s1;
                } else
                    osr << std::setw(wa) << tmp.str();
            } else if (wa > 0)
                osr << std::setw(wa) << ' ';
            UNUSED(underline);
            return osr.str();
        }

        inline std::string _out_title_short(std::string const &title, bas *x,
                                            cmdr::terminal::colors::colorize &c,
                                            cmdr::terminal::colors::colorize::Colors256 fg,
                                            bool dim,
                                            bool underline,
                                            int &escaped_chars,
                                            bool flag) {
            std::ostringstream osr;
            if (x->hidden()) {
                std::ostringstream os1, os2;
                if (flag) {
                    os1 << c.fg(fg).dim(dim).s("-");
                    auto s1 = os1.str();
                    osr << s1;
                    escaped_chars += (int) (s1.length() - 1);
                }
                os2 << c.fg(fg).dim(dim).s(title);
                auto s2 = os2.str();
                escaped_chars += (int) (s2.length() - title.length());
                osr << s2;
            } else {
                if (flag) osr << '-';
                osr << title;
            }
            UNUSED(underline);
            return osr.str();
        }

        inline std::string _out_title_long(std::string const &title, bas *x,
                                           cmdr::terminal::colors::colorize &c,
                                           cmdr::terminal::colors::colorize::Colors256 fg,
                                           bool dim,
                                           bool underline,
                                           int &escaped_chars,
                                           bool flag) {
            std::ostringstream os1, os2, osr;
            if (x->hidden()) {
                if (flag) {
                    os1 << c.fg(fg).dim(dim).s("--");
                    auto s1 = os1.str();
                    osr << s1;
                    escaped_chars += (int) (s1.length() - 2);
                }
                os2 << c.underline(underline).fg(fg).dim(dim).s(title);
                auto s2 = os2.str();
                escaped_chars += (int) (s2.length() - title.length());
                osr << s2;
            } else {
                os1 << c.underline(underline).s(title);
                auto s1 = os1.str();
                escaped_chars += (int) (s1.length() - title.length());
                if (flag) { osr << '-' << '-'; }
                osr << s1;
            }
            return osr.str();
        }
    } // namespace detail


    //#pragma clang diagnostic push
    //#pragma ide diagnostic ignored "misc-no-recursion"
    inline void cmd::print_commands(std::ostream &ss, cmdr::terminal::colors::colorize &c, int &wt, bool grouped, bool show_hidden_items, bool shell_completion_mode, int level) const {
        UNUSED(grouped, level);
        auto fg = vars::store::_dim_text_fg;
        auto dim = vars::store::_dim_text_dim;
        auto underline = vars::store::_long_title_underline;
        auto [th, tw] = terminal::terminfo::get_win_size();
        auto is_zsh = util::detect_shell_env() == "zsh";
        if (shell_completion_mode) tw = 9999; // don't wrap the long line

        int count_all{};
        std::map<std::string, std::string> dotted_key_on_keys = detail::sort_keys(_grouped_commands);
        detail::populate_tab_stop_width(dotted_key_on_keys, _grouped_commands, show_hidden_items, wt);

        for (auto &it : dotted_key_on_keys) {
            auto const &val_it = _grouped_commands.find(it.second);
            if (val_it == _grouped_commands.end()) continue;
            auto const &val = val_it->second;
            auto clean_key = string::has_prefix(it.second, it.first) ? it.second.substr(it.first.length() + 1) : it.second;

            auto [wf, ws, wa, w, valid_count] = detail::calc_widths(show_hidden_items, val);

            if (valid_count == 0)
                continue;

            int level_pad = 0;
            if (!shell_completion_mode)
                ss << detail::_out_group_name(it.second, clean_key, val, c, fg, dim, show_hidden_items, level, level_pad);

            for (auto &x : val) {
                if (!show_hidden_items && x->hidden()) continue;

                int escaped_chars{0};

                if (shell_completion_mode && is_zsh) {
                    std::ostringstream titles;
                    // if (!x->title_long().empty() && !x->title_short().empty()) {
                    //     titles << "'" << '(' << x->title_long() << ' ' << x->title_short() << ')';
                    //     titles << "'" << '{' << x->title_long() << ',' << x->title_short() << '}';
                    //     titles << "'" << '[' << x->descriptions() << ']' << "'" << '\n';
                    // } else
                    if (!x->title_long().empty()) {
                        titles << x->title_long() << ':'
                               << string::reg_replace(x->descriptions(), "[\\[\\]\\:\\(\\)]", "\\$&")
                               << '\n';
                    } else if (!x->title_short().empty()) {
                        titles << x->title_short() << ':'
                               << string::reg_replace(x->descriptions(), "[\\[\\]\\:\\(\\)]", "\\$&")
                               << '\n';
                    }
                    ss << titles.str();
                    continue;
                }

                if (level >= 0)
                    ss << std::string(((std::size_t) level + level_pad) * 2, ' ') << '*' << ' ';
                else
                    ss << "  " << std::left << std::setfill(' ');
                if (!x->title_long().empty()) {
                    w = (int) x->title_long().length();
                    ss << detail::_out_title_long(x->title_long(), x, c, fg, dim, underline, escaped_chars, false);
                    ss << detail::_out_comma_space(x->title_short().empty() && x->title_aliases().empty(), x, c, fg, dim, underline, escaped_chars);

                    w = wf - w;
                    if (w > 0) ss << std::setw(w) << ' ';
                } else
                    ss << std::setw((std::size_t) wf + 2) << ' ';

                if (!x->title_short().empty()) {
                    w = (int) x->title_short().length();
                    ss << detail::_out_title_short(x->title_short(), x, c, fg, dim, underline, escaped_chars, false);
                    ss << detail::_out_comma_space(x->title_aliases().empty(), x, c, fg, dim, underline, escaped_chars);

                    w = ws - w;
                    if (w > 0) ss << std::setw(w) << ' ';
                } else
                    ss << std::setw((std::size_t) ws + 2) << ' ';

                ss << detail::_out_title_aliases(x, c, fg, dim, underline, _alias_right_align, wa, escaped_chars, false);

                w = wf + 2 + ws + 2 + wa;
                auto wt_real = (wt < 43 ? 43 : wt);
                ss << std::setw(((std::size_t) wt_real - w - ((level >= 0 ? level : 0) + level_pad) * 2)) << ' ';

                if (shell_completion_mode) ss << '|'; // print a delimiter char for shell completion parser

                ss << detail::_out_desc(x->descriptions(), c, fg, dim, tw, wt_real);

                //ss << '/' << wt << '(' << wt_real << ',' << escaped_chars << ')' << rw << '/' << tw;
                ss << '\n';

                count_all++;

                if (level >= 0 && x->has_sub_commands()) {
                    x->print_commands(ss, c, wt, grouped, show_hidden_items, shell_completion_mode, level + level_pad + 1);
                }
            }
        }

        if (count_all == 0 && !shell_completion_mode) {
            ss << c.fg(fg).dim(dim).s("  (no sub-commands)") << '\n';
        }
    }
    //#pragma clang diagnostic pop

    inline void cmd::print_flags(std::ostream &ss, cmdr::terminal::colors::colorize &c, int &wt, bool grouped, bool show_hidden_items, bool shell_completion_mode, int level) const {
        UNUSED(grouped, level);
        auto fg = vars::store::_dim_text_fg;
        auto dim = vars::store::_dim_text_dim;
        auto underline = vars::store::_long_title_underline;
        auto [th, tw] = terminal::terminfo::get_win_size();
        if (shell_completion_mode) tw = 9999;

        int count_all{};
        std::map<std::string, std::string> dotted_key_on_keys = detail::sort_keys(_grouped_args);
        detail::populate_tab_stop_width(dotted_key_on_keys, _grouped_args, show_hidden_items, wt);

        for (auto &it : dotted_key_on_keys) {
            auto const &val_it = _grouped_args.find(it.second);
            if (val_it == _grouped_args.end()) continue;
            auto const &val = val_it->second;
            auto clean_key = string::has_prefix(it.second, it.first) ? it.second.substr(it.first.length() + 1) : it.second;

            auto [wf, ws, wa, w, valid_count] = detail::calc_widths(show_hidden_items, val);

            if (valid_count == 0)
                continue;

            if (!shell_completion_mode)
                ss << detail::_out_group_name(it.second, clean_key, val, c, fg, dim, show_hidden_items);

            for (auto &x : val) {
                if (!show_hidden_items && x->hidden()) continue;

                int escaped_chars{0};

                ss << ' ' << ' ' << std::left << std::setfill(' ');
                if (!x->title_long().empty()) {
                    w = (int) x->title_long().length();
                    ss << detail::_out_title_long(x->title_long(), x, c, fg, dim, underline, escaped_chars, true);
                    ss << detail::_out_placeholder(x, c, fg, dim, underline, escaped_chars);
                    ss << detail::_out_comma_space(x->title_short().empty() && x->title_aliases().empty(), x, c, fg, dim, underline, escaped_chars);

                    w = wf - w - 2;
                    if (!x->placeholder().empty())
                        w -= (int) x->placeholder().length() + 1;

                    if (w > 0) ss << std::setw(w) << ' ';
                } else
                    ss << std::setw((std::size_t) wf + 2) << ' ';

                if (!x->title_short().empty()) {
                    w = (int) x->title_short().length();
                    ss << detail::_out_title_short(x->title_short(), x, c, fg, dim, underline, escaped_chars, true);
                    ss << detail::_out_comma_space(x->title_aliases().empty(), x, c, fg, dim, underline, escaped_chars);

                    w = ws - w - 1;
                    if (w > 0) ss << std::setw(w) << ' ';
                } else
                    ss << std::setw((std::size_t) ws + 2) << ' ';

                ss << detail::_out_title_aliases(x, c, fg, dim, underline, _alias_right_align, wa, escaped_chars, true);

                w = wf + 2 + ws + 2 + wa;
                auto wt_real = (wt < 43 ? 43 : wt);
                ss << std::setw(((std::size_t) wt_real - w - (level >= 0 ? level * 2 : 0))) << ' ';

                if (shell_completion_mode) ss << '|'; // print a delimiter char for shell completion parser

                std::stringstream td;
                td << x->descriptions();
                td << detail::_os_env_vars(x);
                auto sd = x->defaults();
                if (!sd.empty()) {
                    td << sd;
                }
                ss << detail::_out_desc(td.str(), c, fg, dim, tw, wt_real);

                //ss << '/' << wt << '(' << wt_real << ',' << escaped_chars << ')' << rw << '/' << tw;
                ss << '\n';

                count_all++;
            }
        }

        if (count_all == 0 && !shell_completion_mode) {
            ss << c.fg(fg).dim(dim).s("  (no options)") << '\n';
        }
    }


} // namespace cmdr::opt

#endif //CMDR_CXX11_CMDR_CMD_INL_H
