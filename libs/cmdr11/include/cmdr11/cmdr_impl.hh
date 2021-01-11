//
// Created by Hedzr Yeh on 2021/1/11.
//

#ifndef CMDR_CXX11_CMDR_IMPL_HH
#define CMDR_CXX11_CMDR_IMPL_HH

#include "cmdr_app.hh"

#include <exception>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>


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

    inline opt::app create_app(const_chars name, const_chars version,
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

            if (_grouped_args.find(gn) == _grouped_args.end())
                _grouped_args.emplace(gn, arg_ptrs{});
            // auto ptr = &_all_args.back();
            // auto it = _grouped_args.find(gn);
            // (*it).second.push_back(ptr);
            _grouped_args[gn].push_back(&_all_args.back());

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
                _all_cmds.push_back(ac);
            } else
                _all_cmds.push_back(a);

            if (_grouped_cmds.find(gn) == _grouped_cmds.end())
                _grouped_cmds.emplace(gn, cmd_ptrs{});
            _grouped_cmds[gn].push_back(&_all_cmds.back());
        }
        return *this;
    }

    inline cmd &cmd::operator+=(const cmd &a) {
        this->operator+(a);
        return *this;
    }

    inline int cmd::run(int argc, char *argv[]) {
        unused(argc);
        unused(argv);
        return 0;
    }

    //
    //
    //

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

    inline void app::print_usages() {
        std::string app_name = path::executable_name();

        std::stringstream ss;
        ss << _name << ' ' << 'v' << _version;
        if (_header.empty())
            ss << " by " << _author << '.' << ' ' << _copyright << std::endl;
        else
            ss << _header << std::endl;
        if (!_description.empty()) {
            ss << std::endl
               << "DESCRIPTION" << std::endl
               << string::pad_left(_description) << std::endl;
        }
        if (!_examples.empty()) {
            ss << std::endl
               << "EXAMPLES" << std::endl
               << string::pad_left(string::reg_replace(_examples, "~", app_name)) << std::endl;
        }
        ss << std::endl
           << "USAGE" << std::endl;
        ss << string::pad_left(app_name, 2) << " [commands] [options] [Tail Args]" << std::endl;

        ss << std::endl
           << "COMMANDS" << std::endl;

        ss << std::endl
           << "OPTIONS" << std::endl;
        {
            std::set<std::string> keys;
            std::map<std::string, std::string> dotted_key_on_keys;
            for (auto &it : _grouped_args) {
                keys.insert(it.first);
                auto ptr = it.first.find('.');
                if (ptr > 0) {
                    auto dotted = it.first.substr(0, ptr);
                    dotted_key_on_keys.insert({dotted, it.first});
                } else {
                    dotted_key_on_keys.insert({"", it.first});
                }
            }
            for (auto &it : dotted_key_on_keys) {
                auto val = _grouped_args[it.second];
                auto clean_key = it.second.substr(it.first.length());
                if (it.second != UNSORTED_GROUP)
                    ss << '[' << clean_key << ']' << std::endl;
                for (auto &x : val) {
                    ss << "  " << std::setw(43) << std::left << x->titles()
                       << x->descriptions() << x->defaults()
                       << std::endl;
                }
            }
        }

        if (!_tail_line.empty()) {
            ss << std::endl
               << _tail_line;
        } else {
            ss << std::endl
               << "Type `" << app_name << " --help` to get help information." << std::endl;
        }

        std::cout << ss.str();
    }
} // namespace cmdr::opt


#endif //CMDR_CXX11_CMDR_IMPL_HH
