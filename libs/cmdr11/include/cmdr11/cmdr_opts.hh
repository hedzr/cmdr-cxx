//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_OPT_HH
#define CMDR_CXX11_CMDR_OPT_HH

#include "cmdr_var_t.hh"

#include "cmdr_cmn.hh"

#include "cmdr_arg.hh"
#include "cmdr_cmd.hh"


namespace cmdr::opt {


    class sub_cmd {
    public:
        sub_cmd() = default;
        ~sub_cmd() = default;

        types::option get() {
            return [=](cmd &c) {
                c += _cmd;
            };
        }

    public:
        sub_cmd &operator()(const char *long_, const char *short_ = nullptr) {
            _cmd.template titles(long_, short_);
            return (*this);
        }

        template<class... Args>
        sub_cmd &operator()(const char *long_, const char *short_, Args... args) {
            _cmd.template titles(long_, short_, args...);
            return (*this);
        }

        sub_cmd &group(const char *s) {
            _cmd.group(s);
            return (*this);
        }

        sub_cmd &hidden(bool b = true) {
            _cmd.hidden(b);
            return (*this);
        }

        sub_cmd &description(const char *desc, const char *long_desc = nullptr) {
            _cmd.description(desc, long_desc);
            return (*this);
        }

        sub_cmd &examples(const char *s) {
            _cmd.examples(s);
            return (*this);
        }

        sub_cmd &special(bool b = true) {
            _cmd.special(b);
            return (*this);
        }

        sub_cmd &no_non_special(bool b = true) {
            _cmd.no_non_special(b);
            return (*this);
        }

        sub_cmd &on_hit(types::on_command_hit const &cb) {
            _cmd.on_command_hit(cb);
            return (*this);
        }

        sub_cmd &on_pre_invoke(types::on_pre_invoke const &cb) {
            _cmd.on_pre_invoke(cb);
            return (*this);
        }

        sub_cmd &on_post_invoke(types::on_post_invoke const &cb) {
            _cmd.on_post_invoke(cb);
            return (*this);
        }

        sub_cmd &on_invoke(types::on_invoke const &cb) {
            _cmd.on_invoke(cb);
            return (*this);
        }


        cmd &underlying() { return _cmd; }
        [[nodiscard]] cmd const &underlying() const { return _cmd; }

    private:
        cmd _cmd;
    }; // class cmd


    class opt {
    public:
        opt() = default;
        // opt(opt const *) = delete;
        opt(opt &&o)
            : _arg(std::move(o._arg)) {}
        template<class A, typename... Args,
                 std::enable_if_t<
                         std::is_constructible<vars::variable, A, Args...>::value &&
                                 !std::is_same<std::decay_t<A>, opt>::value,
                         int> = 0>
        explicit opt(A &&a0, Args &&...args)
            : _arg(std::forward<A>(a0), std::forward<Args>(args)...) {
        }
        template<class A,
                 std::enable_if_t<std::is_constructible<vars::variable, A>::value &&
                                          !std::is_same<std::decay_t<A>, opt>::value,
                                  int> = 0>
        explicit opt(A &&a)
            : _arg(std::forward<A>(a)) {}
#if defined(CMDR_CAST_CONST_CHARS_AS_STD_STRING)
        explicit opt(char const *&&a)
            : _arg(std::string(a)) {}
#endif
        ~opt() = default;

        types::option get() {
            return [=](cmd &c) {
                c += _arg;
            };
        }

    public:
        opt &operator()(const char *long_, const char *short_ = nullptr) {
            _arg.titles(long_, short_);
            return (*this);
        }

        template<class... Args>
        opt &operator()(const char *long_, const char *short_, Args... args) {
            _arg.titles(long_, short_, args...);
            return (*this);
        }

        opt &operator()(const std::string &long_, const std::string &short_) {
            _arg.titles(long_.c_str(), short_.empty() ? nullptr : short_.c_str());
            return (*this);
        }

        template<class... Args>
        opt &operator()(const std::string &long_, const std::string &short_, Args... args) {
            _arg.titles(long_, short_, args...);
            return (*this);
        }

        opt &group(const char *s) {
            _arg.group(s);
            return (*this);
        }

        opt &hidden(bool b = true) {
            _arg.hidden(b);
            return (*this);
        }

        opt &description(const char *desc, const char *long_desc = nullptr) {
            _arg.description(desc, long_desc);
            return (*this);
        }

        opt &examples(const char *s) {
            _arg.examples(s);
            return (*this);
        }

        opt &required(bool b = true) {
            _arg.required(b);
            return (*this);
        }

        opt &special(bool b = true) {
            _arg.special(b);
            return (*this);
        }

        opt &no_non_special(bool b = true) {
            _arg.no_non_special(b);
            return (*this);
        }

        opt &on_hit(types::on_flag_hit const &cb) {
            _arg.on_flag_hit(cb);
            return (*this);
        }

        opt &placeholder(const char *s) {
            _arg.placeholder(s);
            return (*this);
        }

        opt &toggle_group(const char *s) {
            _arg.toggle_group(s);
            return (*this);
        }

        template<typename... Args>
        opt &env_vars(Args... args) {
            _arg.template env_vars(args...);
            return (*this);
        }

        arg &underlying() { return _arg; }
        [[nodiscard]] const arg &underlying() const { return _arg; }

    private:
        arg _arg;
        // vars::variable _value{};
    }; // class opt
    

} // namespace cmdr::opt

#endif //CMDR_CXX11_CMDR_OPT_HH
