//
// Created by Hedzr Yeh on 2021/2/1.
//

#ifndef CMDR_CXX11_CMDR_PUBLIC_HH
#define CMDR_CXX11_CMDR_PUBLIC_HH

#include "cmdr_internals.hh"

namespace cmdr {

    inline app &get_app() { return *app_holder::instance().get_ptr(); }
    inline auto &get_store() { return get_app().store(); }

    inline app cli(const_chars name, const_chars version,
                   const_chars author = nullptr, const_chars copyright = nullptr,
                   const_chars description = nullptr,
                   const_chars examples = nullptr) {
        if (app_holder::instance().get_ptr()) {
            throw std::logic_error("can't invoke create_app() multiple times.");
        }

        return app::create(name, version, author, copyright, description, examples);
    }

    inline bool is_debug() { return get_store().get("debug").cast_as<bool>(); }
    inline bool is_trace() { return get_store().get("trace").cast_as<bool>(); }
    inline bool is_verbose() { return get_store().get("verbose").cast_as<bool>(); }
    inline bool is_quiet() { return get_store().get("quiet").cast_as<bool>(); }

    inline int get_verbose_level() { return get_app()["verbose"].hit_count(); }
    inline bool is_help_hit() { return get_app()["help"].hit_count() > 0 && get_store().get("help").cast_as<bool>(); }

}

#endif //CMDR_CXX11_CMDR_PUBLIC_HH
