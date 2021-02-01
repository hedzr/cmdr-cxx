//
// Created by Hedzr Yeh on 2021/1/29.
//

#ifndef CMDR_CXX11_CMDR_IMP1_HH
#define CMDR_CXX11_CMDR_IMP1_HH

namespace cmdr::vars {

    inline variable::self_type variable::parse(std::string &s) {
        self_type v;
        std::istringstream is(s);
        is >> v;
        return v;
    }

    template<class T, class small_string>
    inline T &nodeT<T, small_string>::_get(std::string const &key) {
        std::stringstream ss;
        ss << DEFAULT_KEY_PREFIX << '.' << key;
        return _get_raw(ss.str());
    }
    template<class T, class small_string>
    inline T &nodeT<T, small_string>::_get_raw(std::string const &key) {
        auto it = _indexes.find(key);
        if (it == _indexes.end()) {
            return null_element();
        }
        return *it->second;
    }

    template<class T, class small_string>
    inline void nodeT<T, small_string>::dump_tree(std::ostream &os, cmdr::terminal::colors::colorize *c, int level) const {
        auto fg = get_app()._dim_text_fg;
        auto dim = get_app()._dim_text_dim;

        for (auto const &[k, v] : _children) {
            for (int i = 0; i < level; i++) os << "  ";

            std::stringstream ss;
            ss << v;

            os << k << ": ";
            if (c)
                os << c->fg(fg).dim(dim).s(ss.str());
            else
                os << ss.str();
            os << std::endl;

            v.dump_tree(os, c, level + 1);
        }
        unused(level);
    }

    template<class T, class small_string>
    inline void nodeT<T, small_string>::dump_full_keys(std::ostream &os, cmdr::terminal::colors::colorize *c, int level) const {
        // std::vector<std::string> keys;
        // keys.reserve(_full_key_map.size());
        // for (auto &it : _full_key_map) {
        //     keys.push_back(it.first);
        // }
        // std::sort(keys.begin(), keys.end());
        //
        // for (auto &it : keys) {
        //     os << " - " << it << " => " << _full_key_map[it] << std::endl;
        // }
        print_sorted(os, c, _indexes);
        unused(level);
    }

    template<class T, class small_string>
    inline nodeT<T, small_string>::map_streamer::map_streamer(std::ostream &os, cmdr::terminal::colors::colorize *c)
        : _os(os)
        , c(c) {
        fg = get_app()._dim_text_fg;
        dim = get_app()._dim_text_dim;
    }

} // namespace cmdr::vars

#endif //CMDR_CXX11_CMDR_IMP1_HH
