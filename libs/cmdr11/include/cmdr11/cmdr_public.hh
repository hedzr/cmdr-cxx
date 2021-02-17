//
// Created by Hedzr Yeh on 2021/2/1.
//

#ifndef CMDR_CXX11_CMDR_PUBLIC_HH
#define CMDR_CXX11_CMDR_PUBLIC_HH

#include "cmdr_internals.hh"

namespace cmdr {

    inline app &get_app() { return *app_holder::instance().get_ptr(); }
    inline auto &get_store() { return get_app().store(); }

    [[maybe_unused]] inline app &app::uniq() const { return get_app(); }

    inline app &cli(const_chars name, const_chars version,
                    const_chars author = nullptr, const_chars copyright = nullptr,
                    const_chars description = nullptr,
                    const_chars examples = nullptr) {
        if (auto *ptr = app_holder::instance().get_ptr(); ptr) {
            return get_app();
        }
        return app::create_new(name, version, author, copyright, description, examples);
    }

    inline app new_cli(const_chars name, const_chars version,
                       const_chars author = nullptr, const_chars copyright = nullptr,
                       const_chars description = nullptr,
                       const_chars examples = nullptr) {
#if !defined(CATCH_VERSION_MAJOR)
        if (auto *ptr = app_holder::instance().get_ptr(); ptr) {
            cmdr_throw_line("can't invoke create_app() multiple times.");
        }
#endif
        return app::create(name, version, author, copyright, description, examples);
    }

    /**
     * @brief retrieve the config item value from store
     * 
     * get() will wrap a prefix 'app.' to key for extracting the raw value in Option Store.
     * 
     * @tparam T 
     * @param key is a dotted string like 'server.start.foreground'
     * @return 
     */
    template<class T>
    inline T get(char const *key) { return get_app().get(key).cast_as<T>(); }
    /**
     * @brief retrieve the config item value from store
     * 
     * get() will wrap a prefix 'app.' to key for extracting the raw value in Option Store.
     * 
     * @tparam T 
     * @param key is a dotted string like 'server.start.foreground'
     * @return 
     */
    template<class T>
    inline T get(std::string const &key) { return get_app().get(key).cast_as<T>(); }
    /**
     * @brief retrieve the cli-app args/flags value item from store.
     * 
     * get_for_cli will wrap a prefix 'app.cli.' to key for extracting the raw value in Option Store.
     * 
     * @tparam T 
     * @param key is a dotted string like 'server.start.foreground'
     * @return 
     */
    template<class T>
    inline T get_for_cli(char const *key) { return get_app().get_for_cli(key).cast_as<T>(); }
    /**
     * @brief retrieve the cli-app args/flags value item from store
     * 
     * get_for_cli will wrap a prefix 'app.cli.' to key for extracting the raw value in Option Store.
     * 
     * @tparam T 
     * @param key is a dotted string like 'server.start.foreground'
     * @return 
     */
    template<class T>
    inline T get_for_cli(std::string const &key) { return get_app().get_for_cli(key).cast_as<T>(); }


    template<class A, typename... Args,
             std::enable_if_t<
                     std::is_constructible<vars::variable, A, Args...>::value &&
                             !std::is_same<std::decay_t<A>, vars::variable>::value &&
                             !std::is_same<std::decay_t<A>, app>::value,
                     int> = 0>
    inline void set(char const *key, A &&a0, Args &&...args) {
        get_app().set(key, a0, args...);
    }
    template<class A,
             std::enable_if_t<std::is_constructible<vars::variable, A>::value &&
                                      !std::is_same<std::decay_t<A>, vars::variable>::value &&
                                      !std::is_same<std::decay_t<A>, app>::value,
                              int> = 0>
    void set(char const *key, A &&a) {
        get_app().set(key, a);
    }
    inline void set(char const *key, vars::variable &&a) { get_app().set(key, a); }
    inline void set(char const *key, vars::variable const &a) { get_app().set(key, a); }


    inline bool is_debug() { return get_for_cli<bool>("debug"); }
    inline bool is_trace() { return get_for_cli<bool>("trace"); }
    inline bool is_verbose() { return get_for_cli<bool>("verbose"); }
    inline bool is_quiet() { return get_for_cli<bool>("quiet"); }
    inline bool is_no_color() { return get_for_cli<bool>("no-color"); }


    /**
     * @brief retrieve the verbose level from the hit data of a flag named as "verbose".
     * see also the initializations of the internal commands and flags.
     * @return 
     */
    inline int get_verbose_level() { return get_app()["verbose"].hit_count(); }
    inline bool is_help_hit() { return get_app()["help"].hit_count() > 0 && get_for_cli<bool>("help"); }

} // namespace cmdr

#endif //CMDR_CXX11_CMDR_PUBLIC_HH
