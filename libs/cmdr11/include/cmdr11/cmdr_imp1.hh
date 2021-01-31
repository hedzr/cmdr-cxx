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

} // namespace cmdr::vars

#endif //CMDR_CXX11_CMDR_IMP1_HH
