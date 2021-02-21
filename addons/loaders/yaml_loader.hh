//
// Created by Hedzr Yeh on 2021/1/31.
//

#ifndef CMDR_CXX11_YAML_LOADER_HH
#define CMDR_CXX11_YAML_LOADER_HH

#if __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#elif __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#elif _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251 4275 4996 26812) // needs to have dll-interface to be used by clients of class
#endif
#include <yaml-cpp/yaml.h> // https://github.com/jbeder/yaml-cpp
#if __clang__
#pragma clang diagnostic pop
#elif __GNUC__
#pragma GCC diagnostic pop
#elif _MSC_VER
#pragma warning(pop)
#endif

#include <cmdr-cxx.hh>

#include <cstdlib>
#include <fstream>
#include <optional>
#include <regex>


namespace YAML {
    template<typename T>
    struct as_if<T, std::optional<T>> {
        explicit as_if(const Node &node_)
            : node(node_) {}
        const Node &node;

        std::optional<T> operator()() const {
            std::optional<T> val;
            T t;
            if (node.m_pNode && convert<T>::decode(node, t))
                val = std::move(t);
            return val;
        }
    };

    // There is already a std::string partial specialisation, so we need a full specialisation here
    template<>
    struct as_if<std::string, std::optional<std::string>> {
        explicit as_if(const Node &node_)
            : node(node_) {}
        const Node &node;

        std::optional<std::string> operator()() const {
            std::optional<std::string> val;
            std::string t;
            if (node.m_pNode && convert<std::string>::decode(node, t))
                val = std::move(t);

            return val;
        }
    };
} // namespace YAML

namespace cmdr::addons::loaders {

    /**
     * @brief loading the external config file in YAML format
     * 
     * Usages:
     * 
     *      #include <cmdr11/addons/loaders/yaml_loader.hh>
     *      
     *      using namespace cmdr::addons::loaders;
     *      cli.set_global_on_loading_externals(yaml_loader{}());
     * 
     * CMake Preparations:
     * 
     *      include(cmdr11/addons/loaders/yaml_loader)
     */
    class yaml_loader {
    public:
        cmdr::types::on_loading_externals operator()() const {
            return [=](cmdr::app &c) {
                UNUSED(c);

                static std::vector searches = {
                    "", // for --config
#if defined(_DEBUG)
                    "./tests/etc/$APP_NAME/$APP_NAME.yml",
                    "../tests/etc/$APP_NAME/$APP_NAME.yml",
#endif
                    "/etc/$APP_NAME/$APP_NAME.yml",
                    "/usr/local/etc/$APP_NAME/$APP_NAME.yml",
                    "$HOME/.config/$APP_NAME/$APP_NAME.yml",
                    "$HOME/.$APP_NAME/$APP_NAME.yml",
                    "$APP_NAME.yml",
                    ".$APP_NAME.yml",
                };

                const char *const env_var = "CONFIG_FILE";
                c += cmdr::opt::opt{""}("config")
                             .description("the directory or the full-path of the main config file")
                             .group(SYS_MGMT_GROUP)
                             .placeholder("CONFIG_FILE")
                             .env_vars(env_var)
                             .on_hit([&](opt::cmd const &hit, opt::arg const &hit_flag, string_array const &remain_args) -> opt::Action {
                                 UNUSED(hit, hit_flag, remain_args);
                                 if (hit_flag.hit_count()) {
                                     auto file = cmdr::get_for_cli<std::string>("config");
                                     load_config_file_or_dir(file, c);
                                 }
                                 return opt::Action::Continue;
                             });

                // load from env_var
                auto ptr = std::getenv(env_var);
                if (ptr) {
                    auto file = std::string(ptr);
                    load_config_file_or_dir(file, c);
                }

                // scan the pre-defined locations
                using namespace cmdr::string;
                using namespace cmdr::path;
                //std::cout << "loading from " << get_current_dir() << '\n';
                for (auto const &sp : searches) {
                    auto p = expand_env(sp);
                    if (!p.empty() && file_exists(p)) {
                        // load it
                        load_to(p, c);
                    }
                }
            };
        }
        void load_config_file_or_dir(std::string &file, cmdr::app &c) const {
            namespace fs = std::filesystem;
            if (path::file_exists(file)) {
                if (fs::is_directory(file)) {
                    auto p = fs::path(file);
                    p /= (c.name() + ".yml");
                    file = p.u8string();
                    if (path::file_exists(file)) {
                        load_to(file, c);
                        return;
                    }
                }

                load_to(file, c);
            }
        }
        void load_to(std::string const &p, cmdr::app &c) const {
            YAML::Node config = YAML::LoadFile(p);

            cmdr::util::defer _upd([p]() {
                auto &cli = cmdr::get_app();
                cli.set("config.file.loaded", true);
                auto const *k = "config.file.files";
                if (!cli.has(k))
                    cli.set(k, std::vector<std::string>{});
                auto &vec = cli.get(k).as<std::vector<std::string> &>();
                vec.push_back(p);
                // cli.set(k, vec);
            });

            load_node_to("", config, c);
        }
        void load_node_to(std::string const &pk, YAML::Node const &node, cmdr::app &c) const {
            for (YAML::const_iterator it = node.begin(); it != node.end(); ++it) {
                auto key = it->first.as<std::string>();
                // std::cout << "... " << pk << '.' << key;
                switch (it->second.Type()) {
                    case YAML::NodeType::Null: // ...
                        break;
                    case YAML::NodeType::Scalar: // ...
                    {
                        // std::cout << " is " << it->second.as<std::string>();
                        // std::cout << '\n';

                        std::stringstream ks;
                        if (!pk.empty())
                            ks << pk << '.';
                        ks << key;

                        auto val = safe_get_value(it->second);
                        c.store().set_raw(ks.str().c_str(), val);
                        break;
                    }
                    case YAML::NodeType::Sequence: // ...
                    {
                        // std::cout << '\n';
                        std::vector<cmdr::vars::variable> vec;
                        for (YAML::const_iterator itz = it->second.begin(); itz != it->second.end(); ++itz)
                            vec.push_back(safe_get_value(*itz));

                        std::stringstream ks;
                        if (!pk.empty())
                            ks << pk << '.';
                        ks << key;

                        if (!vec.empty()) {
                            cmdr::vars::variable val{vec};
                            c.store().set_raw(ks.str().c_str(), val);
                        } else {
                            cmdr::vars::variable val{std::vector<std::string>{}};
                            c.store().set_raw(ks.str().c_str(), val);
                        }
                        break;
                    }
                    case YAML::NodeType::Map: // ...
                    {
                        // std::cout << '\n';
                        std::stringstream ks;
                        if (!pk.empty())
                            ks << pk << '.';
                        ks << key;
                        load_node_to(ks.str(), it->second, c);
                        break;
                    }
                    case YAML::NodeType::Undefined: // ...
                        break;
                }
            }
        }

        template<typename T>
        static auto extractor(YAML::Node const &node) {
            return YAML::as_if<bool, std::optional<bool>>(node)();
        }

        static cmdr::vars::variable safe_get_value(YAML::Node const &node) {
            cmdr::vars::variable val;
            if (auto as_bool = extractor<bool>(node); as_bool.has_value()) {
                val = as_bool.value();
            } else if (auto as_short = extractor<short>(node); as_short.has_value()) {
                val = as_short.value();
            } else if (auto as_ushort = extractor<unsigned short>(node); as_ushort.has_value()) {
                val = as_ushort.value();
            } else if (auto as_int = extractor<int>(node); as_int.has_value()) {
                val = as_int.value();
            } else if (auto as_uint = extractor<unsigned int>(node); as_uint.has_value()) {
                val = as_uint.value();
            } else if (auto as_long = extractor<long>(node); as_long.has_value()) {
                val = as_long.value();
            } else if (auto as_ulong = extractor<long>(node); as_ulong.has_value()) {
                val = as_ulong.value();
            } else if (auto as_longlong = extractor<long long>(node); as_longlong.has_value()) {
                val = as_longlong.value();
            } else if (auto as_ulonglong = extractor<unsigned long long>(node); as_ulonglong.has_value()) {
                val = as_ulonglong.value();

            } else if (auto as_float = extractor<float>(node); as_float.has_value()) {
                val = as_float.value();
            } else if (auto as_double = extractor<double>(node); as_double.has_value()) {
                val = as_double.value();
            } else if (auto as_longdouble = extractor<long double>(node); as_longdouble.has_value()) {
                val = as_longdouble.value();

            } else if (auto as_vecint = extractor<std::vector<int>>(node); as_vecint.has_value()) {
                val = as_vecint.value();
            } else if (auto as_veclong = extractor<std::vector<long>>(node); as_veclong.has_value()) {
                val = as_veclong.value();
            } else if (auto as_vecflt = extractor<std::vector<float>>(node); as_vecflt.has_value()) {
                val = as_vecflt.value();
            } else if (auto as_vecdbl = extractor<std::vector<double>>(node); as_vecdbl.has_value()) {
                val = as_vecdbl.value();
            } else if (auto as_vecstr = extractor<std::vector<std::string>>(node); as_vecstr.has_value()) {
                val = as_vecstr.value();

            } else if (auto as_ns = extractor<std::chrono::nanoseconds>(node); as_ns.has_value()) {
                val = as_ns.value();
            } else if (auto as_us = extractor<std::chrono::microseconds>(node); as_us.has_value()) {
                val = as_us.value();
            } else if (auto as_ms = extractor<std::chrono::milliseconds>(node); as_ms.has_value()) {
                val = as_ms.value();
            } else if (auto as_sec = extractor<std::chrono::seconds>(node); as_sec.has_value()) {
                val = as_sec.value();
            } else if (auto as_min = extractor<std::chrono::minutes>(node); as_min.has_value()) {
                val = as_min.value();
            } else if (auto as_hour = extractor<std::chrono::hours>(node); as_hour.has_value()) {
                val = as_hour.value();
            } else if (auto as_dur_ld = extractor<std::chrono::duration<long double, std::ratio<1>>>(node); as_dur_ld.has_value()) {
                val = as_dur_ld.value();
            } else if (auto as_dur_ld60 = extractor<std::chrono::duration<long double, std::ratio<60>>>(node); as_dur_ld60.has_value()) {
                val = as_dur_ld60.value();
            } else if (auto as_dur_d60 = extractor<std::chrono::duration<double, std::ratio<60>>>(node); as_dur_d60.has_value()) {
                val = as_dur_d60.value();
            } else if (auto as_dur_f60 = extractor<std::chrono::duration<float, std::ratio<60>>>(node); as_dur_f60.has_value()) {
                val = as_dur_f60.value();
            } else if (auto as_dur_f1 = extractor<std::chrono::duration<float, std::ratio<1>>>(node); as_dur_f1.has_value()) {
                val = as_dur_f1.value();

            } else if (auto as_chars = extractor<const_chars>(node); as_chars.has_value()) {
                val = as_chars.value();

            } else { // if (std::optional<std::string> as_string = YAML::as_if<std::string, std::optional<std::string>>(node)(); as_string.has_value()) {
                val = node.as<std::string>();
            }
            return val;
        }
    };

} // namespace cmdr::addons::loaders

#endif //CMDR_CXX11_YAML_LOADER_HH
