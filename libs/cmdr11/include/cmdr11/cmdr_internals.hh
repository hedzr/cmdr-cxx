//
// Created by Hedzr Yeh on 2021/1/25.
//

#ifndef CMDR_CXX11_CMDR_INTERNALS_HH
#define CMDR_CXX11_CMDR_INTERNALS_HH

#include "cmdr_utils.hh"


namespace cmdr {

    class app;

    class app_holder final : public util::singleton<app_holder> {
    public:
        explicit app_holder(typename util::singleton<app_holder>::token) { /*std::cout << "AppHolder constructed" << std::endl;*/
        }
        ~app_holder() = default;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "readability-convert-member-functions-to-static"
        [[maybe_unused]] void use() const { std::cout << "in use" << std::endl; }
#pragma clang diagnostic pop

    public:
        app *get_ptr() { return _app; }
        app &operator*() { return *_app; }
        app *operator->() { return _app; }
        // explicit operator app() { return _app; }

    private:
        app *_app{};
        void put(app *ptr) { _app = ptr; }
        friend class app;
    };

} // namespace cmdr

#endif //CMDR_CXX11_CMDR_INTERNALS_HH
