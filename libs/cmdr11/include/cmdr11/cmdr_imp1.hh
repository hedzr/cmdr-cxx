//
// Created by Hedzr Yeh on 2021/1/29.
//

#ifndef CMDR_CXX11_CMDR_IMP1_HH
#define CMDR_CXX11_CMDR_IMP1_HH


#include "cmdr_arg.hh"


namespace cmdr::vars {


} // namespace cmdr::vars

namespace cmdr::opt {

    inline std::string arg::defaults() const {
        std::stringstream ss;
        ss << ' ' << '[' << "DEFAULT";
        if (!_placeholder.empty())
            ss << ' ' << _placeholder;
        ss << '=' << _default.get() << ']';
        // if (!std::holds_alternative<std::monostate>(_default)) {
        //     ss << ' ' << '[' << "DEFAULT=" << variant_to_string(_default) << ']';
        // }
        return ss.str();
    }
    inline const arg::var_type &arg::default_value() const { return _default; }
    inline arg::var_type &arg::default_value() { return _default; }

    inline arg &arg::default_value(const vars::variable &v) {
        if (_default.get() == nullptr)
            _default = std::make_shared<vars::variable>(v);
        else
            _default->emplace(v);
        return (*this);
    }
    inline arg &arg::default_value(const_chars v) {
        if (_default.get() == nullptr)
            _default = std::make_shared<vars::variable>(std::string(v));
        else
            _default->emplace(std::string(v));
        return (*this);
    }
    template<class T>
    inline arg &arg::default_value(T const &v) {
        if (_default.get() == nullptr)
            _default = std::make_shared<vars::variable>(v);
        else
            _default->template emplace(v);
        return (*this);
    }

} // namespace cmdr::opt


namespace cmdr::opt::types {

    inline cmd &parsing_context::curr_command() {
        if (_matched_commands.empty())
            return *_root;
        return *_matched_commands.back();
    }

    inline cmd &parsing_context::last_matched_cmd() {
        if (_matched_commands.empty())
            return cmd::null_command();
        return *_matched_commands.back();
    }

    inline arg &parsing_context::last_matched_flg() {
        if (matched_flags.empty())
            return cmd::null_arg();
        return *matched_flags.back();
    }

    //
    inline void parsing_context::add_matched_arg(arg *obj, std::shared_ptr<vars::variable> const &v) {
        matched_flags.push_back(obj);
        _values_map.emplace(std::make_pair(obj, v));
    }

} // namespace cmdr::opt::types


#endif //CMDR_CXX11_CMDR_IMP1_HH
