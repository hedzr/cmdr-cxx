//
// Created by Hedzr Yeh on 2021/8/24.
//

#include "cmdr11/cmdr_defs.hh"

#include "cmdr11/cmdr_common.hh"
#include "cmdr11/cmdr_dbg.hh"
#include "cmdr11/cmdr_log.hh"
#include "cmdr11/cmdr_pipeable.hh"
#include "cmdr11/cmdr_pool.hh"
#include "cmdr11/cmdr_utils.hh"
#include "cmdr11/cmdr_x_class.hh"
#include "cmdr11/cmdr_x_test.hh"

#include <iostream>
#include <math.h>
#include <string>

cmdr::debug::X x_global_var;

namespace test {
    template<typename T>
    constexpr auto ZFN() {
        constexpr auto function = std::string_view{__FUNCTION_NAME__};
        constexpr auto prefix = std::string_view{"auto ZFN() [T = "};
        constexpr auto suffix = std::string_view{"]"};

        constexpr auto start = function.find(prefix) + prefix.size();
        constexpr auto end = function.rfind(suffix);
        std::string_view name = function.substr(start, (end - start));
        return name.substr(0, (end - start));
        // return function;
    }

    template<typename T>
    constexpr auto ZFS() {
        constexpr auto function = std::string_view{__FUNCTION_NAME__};
        constexpr auto prefix = std::string_view{"auto ZFT() [T = "};
        // constexpr auto suffix = std::string_view{"]"};

        constexpr auto start = function.find(prefix) + prefix.size();
        // constexpr auto end = function.rfind(suffix);
        return (unsigned long long) start; // function.substr(start, (end - start));
        // return function;
    }

    template<typename T>
    constexpr auto ZFT() {
        constexpr auto function = std::string_view{__FUNCTION_NAME__};
        // constexpr auto prefix = std::string_view{"auto ZFT() [T = "};
        constexpr auto suffix = std::string_view{"]"};

        // constexpr auto start = function.find(prefix) + prefix.size();
        constexpr auto end = function.rfind(suffix);
        return (unsigned long long) end; // function.substr(start, (end - start));
        // return function;
    }

    template<typename T>
    constexpr auto ZFZ() {
        // auto ZFZ() [T = std::__1::basic_string<char>]
        constexpr auto function = std::string_view{__FUNCTION_NAME__};
        return function;
    }
} // namespace test

void test_type_name() {
    printf(">>Z '%s'\n", test::ZFZ<std::string>().data());
    printf(">>Z '%s'\n", test::ZFN<std::string>().data());
    printf(">>Z %llu, %llu\n", test::ZFS<std::string>(), test::ZFT<std::string>());

#ifndef _WIN32
    printf(">>2 '%s'\n", cmdr::debug::type_name_holder<std::string>::value.data());

    printf(">>1 '%s'\n", cmdr::debug::type_name_1<cmdr::pool::conditional_wait_for_int>().data());
    printf(">>> '%s'\n", cmdr::debug::type_name<cmdr::pool::conditional_wait_for_int>().data());
#endif

    auto fn = cmdr::debug::type_name<std::string>();
    std::string str{fn};
    printf(">>> '%s'\n", str.c_str());

    std::cout << cmdr::debug::type_name<std::string>() << '\n';
    std::cout << std::string(cmdr::debug::type_name<std::string>()) << '\n';
    printf(">>> %s\n", std::string(cmdr::debug::type_name<std::string>()).c_str());
}

namespace tmp1 {
    struct Point {
        virtual ~Point() = default;
        // virtual Point *clone() const = 0;
        virtual const char *name() const = 0;
    };

    struct Point2D : Point {
        Point2D() = default;
        virtual ~Point2D() = default;
        static std::unique_ptr<Point2D> create_unique() { return std::make_unique<Point2D>(); }
        static Point2D *create() { return new Point2D(); }
        // Point *clone() const override { return new Point2D(*this); }
        DISABLE_MSVC_WARNINGS(26815)
        const char *name() const override { return cmdr::debug::type_name<std::decay_t<decltype(*this)>>().data(); }
        RESTORE_MSVC_WARNINGS
    };

    struct Point3D : Point {
        // Point3D() = default;
        virtual ~Point3D() = default;
        static std::unique_ptr<Point3D> create_unique() { return std::make_unique<Point3D>(); }
        static Point3D *create() { return new Point3D(); }
        // Point *clone() const override { return new Point3D(*this); }
        DISABLE_MSVC_WARNINGS(26815)
        const char *name() const override { return cmdr::debug::type_name<std::decay_t<decltype(*this)>>().data(); }
        RESTORE_MSVC_WARNINGS
    };

    struct factory {
        template<typename T>
        static std::unique_ptr<T> create() { return T::create_unique(); }
    };
} // namespace tmp1

void test_factory() {
    namespace fct = cmdr::util::factory;

    auto pp = tmp1::factory::create<tmp1::Point2D>();
    cmdr_print("Point2D = %p, typename: %s", pp.get(), cmdr::debug::type_name<tmp1::Point2D>().data());
    // pp = tmp1::factory::create<tmp1::Point3D>();
    // cmdr_print("Point2D = %p", pp.get());

    using shape_factory = fct::factory<tmp1::Point, tmp1::Point2D, tmp1::Point3D>;
    auto ptr = shape_factory::create("tmp1::Point2D");
    cmdr_print("shape_factory: Point2D = %p, %s", ptr.get(), ptr->name());
    ptr = shape_factory::create("tmp1::Point3D");
    cmdr_print("shape_factory: Point3D = %p, %s", ptr.get(), ptr->name());

    std::unique_ptr<tmp1::Point> smt = std::make_unique<tmp1::Point3D>();
    cmdr_print("name = %s", smt->name()); // ok
}

namespace cmdr::dp::factory::classical {

    class classical_factory_method;

    class Transport {
    public:
        virtual ~Transport() {}
        virtual void deliver() = 0;
    };

    class Trunk : public Transport {
        float x, y;

    public:
        explicit Trunk(double x_, double y_) { x = (float) x_, y = (float) y_; }
        explicit Trunk(float x_, float y_) { x = x_, y = y_; }
        ~Trunk() = default;
        friend class classical_factory_method;
        void deliver() override { printf("Trunk::deliver()\n"); }
        friend std::ostream &operator<<(std::ostream &os, const Trunk &o) { return os << "x: " << o.x << " y: " << o.y; }
    };

    class Ship : public Transport {
        float x, y;

    public:
        explicit Ship(double x_, double y_) { x = (float) x_, y = (float) y_; }
        explicit Ship(float x_, float y_) { x = x_, y = y_; }
        ~Ship() = default;
        friend class classical_factory_method;
        void deliver() override { printf("Ship::deliver()\n"); }
        friend std::ostream &operator<<(std::ostream &os, const Ship &o) { return os << "x: " << o.x << " y: " << o.y; }
    };

    class classical_factory_method {
    public:
        static Transport *create_ship(float r_, float theta_) {
            return new Ship{r_ * cos(theta_), r_ * sin(theta_)};
        }
        static Transport *create_trunk(float x_, float y_) {
            return new Trunk{x_, y_};
        }
        static std::unique_ptr<Ship> create_ship_2(float r_, float theta_) {
            return std::make_unique<Ship>(r_ * cos(theta_), r_ * sin(theta_));
        }
        static std::unique_ptr<Trunk> create_trunk_2(float x_, float y_) {
            return std::make_unique<Trunk>(x_, y_);
        }

        template<typename T, typename... Args>
        static std::unique_ptr<T> create(Args &&...args) {
            return std::make_unique<T>(args...);
        }
    };

} // namespace cmdr::dp::factory::classical

void test_factory_classical() {
    using namespace cmdr::dp::factory::classical;
    classical_factory_method f;

    auto p1 = f.create_trunk(3.1f, 4.2f);
    std::cout << p1 << '\n';
    p1->deliver();

    auto p2 = f.create_ship(3.1f, 4.2f);
    std::cout << p2 << '\n';
    p2->deliver();

    auto p3 = f.create_ship_2(3.1f, 4.2f);
    std::cout << p3.get() << '\n';
    p3->deliver();

    auto p4 = f.create<Ship>(3.1f, 4.2f);
    std::cout << p4.get() << '\n';
    p4->deliver();
}

namespace cmdr::dp::factory::inner {

    class Transport {
    public:
        virtual ~Transport() {}
        virtual void deliver() = 0;
    };

    class Trunk : public Transport {
        float x, y;

    public:
        explicit Trunk(double x_, double y_) { x = (float) x_, y = (float) y_; }
        explicit Trunk(float x_, float y_) { x = x_, y = y_; }
        ~Trunk() = default;
        void deliver() override { printf("Trunk::deliver()\n"); }
        friend std::ostream &operator<<(std::ostream &os, const Trunk &o) { return os << "x: " << o.x << " y: " << o.y; }
        static std::unique_ptr<Trunk> create(float x_, float y_) {
            return std::make_unique<Trunk>(x_, y_);
        }
    };

    class Ship : public Transport {
        float x, y;

    public:
        explicit Ship(double x_, double y_) { x = (float) x_, y = (float) y_; }
        explicit Ship(float x_, float y_) { x = x_, y = y_; }
        ~Ship() = default;
        void deliver() override { printf("Ship::deliver()\n"); }
        friend std::ostream &operator<<(std::ostream &os, const Ship &o) { return os << "x: " << o.x << " y: " << o.y; }
        static std::unique_ptr<Ship> create(float r_, float theta_) {
            return std::make_unique<Ship>(r_ * cos(theta_), r_ * sin(theta_));
        }
    };

    template<typename T, typename... Args>
    inline std::unique_ptr<T> create_transport(Args &&...args) {
        return T::create(args...);
    }

} // namespace cmdr::dp::factory::inner

void test_factory_inner() {
    using namespace cmdr::dp::factory::inner;

    auto p1 = create_transport<Trunk>(3.1f, 4.2f);
    std::cout << p1.get() << '\n';
    p1->deliver();

    auto p2 = create_transport<Ship>(3.1f, 4.2f);
    std::cout << p2.get() << '\n';
    p2->deliver();

    auto p3 = Ship::create(3.1f, 4.2f);
    std::cout << p3.get() << '\n';
    p3->deliver();
}

namespace cmdr::dp::factory::abstract {

    class Point {
    public:
        virtual ~Point() { cmdr_debug("~Point::dtor()"); }
        virtual const char *name() const = 0;
    };

    class Point2D : public Point {
    public:
        virtual ~Point2D() { cmdr_debug("~Point2D::dtor()"); }
        Point2D() = default;
        static std::unique_ptr<Point2D> create_unique() { return std::make_unique<Point2D>(); }
        static Point2D *create() { return new Point2D(); }
        // Point *clone() const override { return new Point2D(*this); }
        DISABLE_MSVC_WARNINGS(26815)
        const char *name() const override { return cmdr::debug::type_name<std::decay_t<decltype(*this)>>().data(); }
        RESTORE_MSVC_WARNINGS
    };

    class Point3D : public Point {
    public:
        virtual ~Point3D() { cmdr_debug("~Point3D::dtor()"); }
        // Point3D() = default;
        static std::unique_ptr<Point3D> create_unique() { return std::make_unique<Point3D>(); }
        static Point3D *create() { return new Point3D(); }
        // Point *clone() const override { return new Point3D(*this); }
        DISABLE_MSVC_WARNINGS(26815)
        const char *name() const override { return cmdr::debug::type_name<std::decay_t<decltype(*this)>>().data(); }
        RESTORE_MSVC_WARNINGS
    };

    struct factory {
        template<typename T>
        static std::unique_ptr<T> create() { return T::create_unique(); }
    };

} // namespace cmdr::dp::factory::abstract

void test_factory_abstract() {
    using namespace cmdr::dp::factory::abstract;
    auto p1 = factory::create<Point2D>();
    cmdr_print("factory: Point2D = %p, %s", p1.get(), p1->name());
    auto p2 = factory::create<Point3D>();
    cmdr_print("factory: Point3D = %p, %s", p2.get(), p2->name());
}

int main() {

    CMDR_TEST_FOR(test_type_name);

    CMDR_TEST_FOR(test_factory);
    CMDR_TEST_FOR(test_factory_inner);
    CMDR_TEST_FOR(test_factory_classical);
    CMDR_TEST_FOR(test_factory_abstract);

    std::cout << "Compiler: " << cmdr::cross::kCompilerName << '\n';

    return 0;
}