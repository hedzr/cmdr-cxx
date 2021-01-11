//
// Created by Hedzr Yeh on 2021/1/12.
//

#ifndef CMDR_CXX11_CMDR_CMD_HH
#define CMDR_CXX11_CMDR_CMD_HH

#include <list>
#include <unordered_map>


namespace cmdr::opt {

    class arg;

    typedef std::list<arg> arg_list;
    typedef std::list<arg *> arg_ptrs;
    typedef std::unordered_map<std::string, arg_ptrs> grouped_arg_list;

    class cmd;

    typedef std::list<cmd> cmd_list;
    typedef std::list<cmd *> cmd_ptrs;
    typedef std::unordered_map<std::string, cmd_ptrs> grouped_cmd_list;

    typedef std::function<void(cmd &)> option; // void (*option)(app &a);

    class cmd : public obj {
    protected:
        std::string _long;
        std::string _short;
        string_array _aliases;
        std::string _desc_long;
        std::string _description;
        std::string _examples;
        std::string _group{};

        arg_list _all_args{};
        grouped_arg_list _grouped_args{{UNSORTED_GROUP, arg_ptrs{}}};
        cmd_list _all_cmds{};
        grouped_cmd_list _grouped_cmds{{UNSORTED_GROUP, cmd_ptrs{}}};

    public:
        cmd() = default;

        virtual ~cmd() = default;

        cmd(const cmd &o) { _copy(o); }

        cmd &operator=(const cmd &o) {
            _copy(o);
            return (*this);
        }

        void _copy(const cmd &o) {
            __COPY(_long);
            __COPY(_short);
            __COPY(_aliases);
            __COPY(_desc_long);
            __COPY(_description);
            __COPY(_examples);
            __COPY(_group);

            __COPY(_all_args);
            __COPY(_grouped_args);
            __COPY(_all_cmds);
            __COPY(_grouped_cmds);
        }

        [[nodiscard]] bool valid() const {
            if (_long.empty()) return false;
            return true;
        }

    public:
#undef PROP_SET
#undef PROP_SET2
#undef PROP_SET3
#define PROP_SET(mn)         \
    cmd &mn(const_chars s) { \
        if (s) _##mn = s;    \
        return (*this);      \
    }
#define PROP_SET2(mn)                \
    cmd &title_##mn(const_chars s) { \
        if (s) _##mn = s;            \
        return (*this);              \
    }
#define PROP_SET3(mn, typ)          \
    cmd &title_##mn(const typ &s) { \
        _##mn = s;                  \
        return (*this);             \
    }

        PROP_SET2(long)

        PROP_SET2(short)

        PROP_SET3(aliases, string_array)

        // PROP_SET(description)

        PROP_SET(examples)

        PROP_SET(group)

        PROP_SET(desc_long)

#undef PROP_SET
#undef PROP_SET2
#undef PROP_SET3

    public:
        [[nodiscard]] std::string titles() const {
            std::stringstream ss;
            if (!_long.empty()) {
                ss << _long;
            }
            if (!_short.empty()) {
                if (!_long.empty())
                    ss << ',' << ' ';
                ss << _short;
            }
            for (auto &x : _aliases) {
                ss << x;
            }
            return ss.str();
        }

        [[nodiscard]] std::string descriptions() const {
            std::stringstream ss;
            ss << _description;
            return ss.str();
        }

        [[nodiscard]] const std::string &group_name() const {
            return _group;
        }

    public:
        virtual cmd &description(const_chars desc, const_chars long_desc = nullptr, const_chars examples = nullptr) {
            if (desc)
                _description = desc;
            if (long_desc)
                _desc_long = long_desc;
            if (examples)
                _examples = examples;
            return (*this);
        }

        virtual cmd &titles(const_chars title_long) {
            if (title_long) this->_long = title_long;
            return (*this);
        }

        virtual cmd &titles(const_chars title_long, const_chars title_short) {
            if (title_long) this->_long = title_long;
            if (title_short) this->_short = title_short;
            return (*this);
        }

        template<typename... T>
        cmd &titles(const_chars title_long, const_chars title_short, T... title_aliases) {
            if (title_long) this->_long = title_long;
            if (title_short) this->_short = title_short;
            if (sizeof...(title_aliases) > 0) {
                this->aliases(title_aliases...);
            }
            // // must_print("%s\n", aliases...);
            // for (const_chars x : {aliases...}) {
            //     this->_aliases.push_back(x);
            // }
            return (*this);
        }

        template<typename... T>
        cmd &aliases(T... title_aliases) {
            if (sizeof...(title_aliases) > 0) {
                _aliases.push_back({title_aliases...});
            }
            //for (const_chars x : {title_aliases...}) {
            //    this->_aliases.push_back(x);
            //}
            return (*this);
        }

    public:
        virtual cmd &operator+(const arg &a);

        virtual cmd &operator+=(const arg &a);

        virtual cmd &operator+(const cmd &a);

        virtual cmd &operator+=(const cmd &a);

    public:
        virtual cmd &option(const option &opt) {
            opt(*this);
            return (*this);
        }

        virtual int run(int argc, char *argv[]);

        virtual void post_run() const {}
    };
} // namespace cmdr::opt


#endif //CMDR_CXX11_CMDR_CMD_HH
