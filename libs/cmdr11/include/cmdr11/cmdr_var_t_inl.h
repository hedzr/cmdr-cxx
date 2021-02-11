#include <utility>

//
// Created by Hedzr Yeh on 2021/2/2.
//

#ifndef CMDR_CXX11_CMDR_VAR_T_INL_H
#define CMDR_CXX11_CMDR_VAR_T_INL_H

#include "cmdr_dbg.hh"

namespace cmdr::vars {


    inline variable::self_type variable::parse(std::string &s) {
        self_type v;
        std::istringstream is(s);
        is >> v;
        return v;
    }

    // template<class T, class small_string>
    // inline T &nodeT<T, small_string>::_get(std::string const &key) {
    //     std::stringstream ss;
    //     ss << DEFAULT_KEY_PREFIX << '.' << key;
    //     return _get_raw(ss.str());
    // }


    template<class T, class small_string>
    inline T &nodeT<T, small_string>::_get_raw(std::string const &key) {
        auto it = _indexes.find(key);
        if (it == _indexes.end()) {
            return null_element();
        }
        return *it->second;
    }

    template<class T, class small_string>
    inline T const &nodeT<T, small_string>::_get_raw(std::string const &key) const {
        auto it = _indexes.find(key);
        if (it == _indexes.end()) {
            return null_element();
        }
        return *it->second;
    }

    template<class T, class small_string>
    inline T &nodeT<T, small_string>::_get_raw_p(std::string const &prefix, std::string const &key) {
        std::stringstream sk;
        if (!prefix.empty()) sk << prefix << '.';
        sk << key;
        return _get_raw(sk.str());
    }

    template<class T, class small_string>
    inline T const &nodeT<T, small_string>::_get_raw_p(std::string const &prefix, std::string const &key) const {
        std::stringstream sk;
        if (!prefix.empty()) sk << prefix << '.';
        sk << key;
        return _get_raw(sk.str());
    }

    template<class T, class small_string>
    inline T &nodeT<T, small_string>::_get_raw_p(char const *prefix, char const *key) {
        std::stringstream sk;
        if (prefix && prefix[0] != 0) sk << prefix << '.';
        sk << key;
        return _get_raw(sk.str());
    }

    template<class T, class small_string>
    inline T const &nodeT<T, small_string>::_get_raw_p(char const *prefix, char const *key) const {
        std::stringstream sk;
        if (prefix && prefix[0] != 0) sk << prefix << '.';
        sk << key;
        return _get_raw(sk.str());
    }

    template<class T, class small_string>
    inline void nodeT<T, small_string>::
            dump_tree(std::ostream &os,
                      cmdr::terminal::colors::colorize *c,
                      cmdr::terminal::colors::colorize::Colors256 dim_text_fg,
                      bool dim_text_dim,
                      int level) const {
        // auto fg = get_app()._dim_text_fg;
        // auto dim = get_app()._dim_text_dim;

        for (auto const &[k, v] : _children) {
            std::stringstream ss;
            ss << *(v.get());

            for (int i = 0; i < level; i++) os << "  ";
            os << k << ": ";
            if (c)
                os << c->fg(dim_text_fg).dim(dim_text_dim).s(ss.str());
            else
                os << ss.str();
            os << '\n';

            v->dump_tree(os, c, dim_text_fg, dim_text_dim, level + 1);
        }
    }

    template<class T, class small_string>
    // template<class K, class V>
    inline void nodeT<T, small_string>::
            dump_tree_f(std::ostream &os,
                        cmdr::terminal::colors::colorize *c,
                        cmdr::terminal::colors::colorize::Colors256 dim_text_fg,
                        bool dim_text_dim,
                        std::string const &key_prefix,
                        std::function<bool(std::pair<key_type, node_pointer> const &)> const &on_filter,
                        int level) const {
        // auto fg = get_app()._dim_text_fg;
        // auto dim = get_app()._dim_text_dim;

        for (auto &[k, v] : _children) {
            std::string dotted_key;
            {
                std::ostringstream dotted_key_ss;
                if (!key_prefix.empty())
                    dotted_key_ss << key_prefix << ".";
                dotted_key_ss << k;
                dotted_key = dotted_key_ss.str();
            }
            if (on_filter) {
                auto p = std::pair<key_type, node_pointer>(dotted_key, v);
                if (!on_filter(p))
                    continue;
            }

            std::string vs;
            {
                std::stringstream ss;
                ss << *(v.get()); // unwrap the shared_ptr<T> and get vars::variable&
                if (dump_with_type_name)
                    ss << " (" << debug::demangle(v->type().name()) << ")";
                vs = ss.str();
            }

            os << std::string(level * 2, ' ') << k << ": ";
            if (c)
                os << c->fg(dim_text_fg).dim(dim_text_dim).s(vs);
            else
                os << vs;
            os << '\n';

            v->dump_tree_f(os, c, dim_text_fg, dim_text_dim, dotted_key, on_filter, level + 1);
        }
    }

    template<class T, class small_string>
    inline void nodeT<T, small_string>::
            dump_full_keys(std::ostream &os,
                           cmdr::terminal::colors::colorize *c,
                           cmdr::terminal::colors::colorize::Colors256 dim_text_fg,
                           bool dim_text_dim,
                           int level) const {
        print_sorted<small_string, nodeT::node_pointer>(os, c, dim_text_fg, dim_text_dim, _indexes);
        UNUSED(level, os);
    }

    template<class T, class small_string>
    template<class K, class V, class Comp>
    inline void nodeT<T, small_string>::
            dump_full_keys_f(std::ostream &os,
                             cmdr::terminal::colors::colorize *c,
                             cmdr::terminal::colors::colorize::Colors256 dim_text_fg,
                             bool dim_text_dim,
                             std::function<bool(std::pair<K, V> const &)> const &on_filter,
                             int level) const {
        // std::vector<std::string> keys;
        // keys.reserve(_full_key_map.size());
        // for (auto &it : _full_key_map) {
        //     keys.push_back(it.first);
        // }
        // std::sort(keys.begin(), keys.end());
        //
        // for (auto &it : keys) {
        //     os << " - " << it << " => " << _full_key_map[it] << '\n';
        // }
        print_sorted(os, c, dim_text_fg, dim_text_dim, _indexes, on_filter);
        UNUSED(level, os);
    }

    template<class T, class small_string>
    template<class K, class V, class Comp>
    inline void nodeT<T, small_string>::
            print_sorted(
                    std::ostream &os,
                    cmdr::terminal::colors::colorize *c,
                    cmdr::terminal::colors::colorize::Colors256 dim_text_fg,
                    bool dim_text_dim,
                    std::unordered_map<K, V> const &um,
                    std::function<bool(std::pair<K, V> const &)> const &on_filter,
                    Comp pred) const {
        std::map<K, V> m(um.begin(), um.end(), pred);
        map_streamer<K, V> ms{os, c, dim_text_fg, dim_text_dim, on_filter};
        std::for_each(m.begin(), m.end(), ms);
    }

    template<class T, class small_string>
    template<class K, class V, class Comp>
    inline void nodeT<T, small_string>::
            walk_sorted(
                    std::unordered_map<K, V> const &um,
                    std::function<void(std::pair<K, V> const &)> const &cb,
                    Comp comp) const {
        std::map<K, V> m(um.begin(), um.end(), comp);
        std::for_each(m.begin(), m.end(), cb);
    }


    template<class T, class small_string>
    template<class K, class V>
    inline nodeT<T, small_string>::map_streamer<K, V>::
            map_streamer(
                    std::ostream &os,
                    cmdr::terminal::colors::colorize *c_,
                    cmdr::terminal::colors::colorize::Colors256 dim_text_fg,
                    bool dim_text_dim,
                    std::function<bool(std::pair<K, V> const &)> on_filter_)
        : _os(os)
        , c(c_)
        , fg(dim_text_fg)
        , dim(dim_text_dim)
        , on_filter(std::move(on_filter_)) {
    }

    template<class T, class small_string>
    template<class K, class V>
    inline void nodeT<T, small_string>::map_streamer<K, V>::operator()(std::pair<K, V> const &val) {
        // .first is your key, .second is your value

        auto yes{true};
        if (on_filter)
            yes = on_filter(val);

        if (yes) {
            _os << " - " << val.first << " : ";
            std::stringstream ss;
            ss << *(val.second.get());
            if (c)
                _os << c->fg(fg).dim(dim).s(ss.str());
            else
                _os << ss.str();
            _os << '\n';
        }
    }


} // namespace cmdr::vars

#endif //CMDR_CXX11_CMDR_VAR_T_INL_H
