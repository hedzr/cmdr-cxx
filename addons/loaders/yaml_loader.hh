//
// Created by Hedzr Yeh on 2021/1/31.
//

#ifndef CMDR_CXX11_YAML_LOADER_HH
#define CMDR_CXX11_YAML_LOADER_HH

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#include <yaml-cpp/yaml.h> // https://github.com/jbeder/yaml-cpp
#pragma clang diagnostic pop

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
                unused(c);

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
                                 unused(hit);
                                 unused(hit_flag);
                                 unused(remain_args);
                                 if (hit_flag.hit_count()) {
                                     auto file = cmdr::get_for_cli<std::string>("config");
                                     load_config_file_or_dir(file, c);
                                 }
                                 return opt::Continue;
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
                    file = p;
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
                        c.store().set(ks.str().c_str(), val);
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
                            c.store().set(ks.str().c_str(), val);
                        } else {
                            cmdr::vars::variable val{std::vector<std::string>{}};
                            c.store().set(ks.str().c_str(), val);
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

        static cmdr::vars::variable safe_get_value(YAML::Node const &node) {
            cmdr::vars::variable val;
            if (std::optional<bool> as_bool = YAML::as_if<bool, std::optional<bool>>(node)(); as_bool.has_value()) {
                val = as_bool.value();
            } else if (std::optional<int> as_int = YAML::as_if<int, std::optional<int>>(node)(); as_int.has_value()) {
                val = as_int.value();
            } else if (std::optional<double> as_double = YAML::as_if<double, std::optional<double>>(node)(); as_double.has_value()) {
                val = as_double.value();
            } else { // if (std::optional<std::string> as_string = YAML::as_if<std::string, std::optional<std::string>>(node)(); as_string.has_value()) {
                val = node.as<std::string>();
            }
            return val;
        }
    };

} // namespace cmdr::addons::loaders

#endif //CMDR_CXX11_YAML_LOADER_HH
