//
// Created by Hedzr Yeh on 2021/1/25.
//

#ifndef CMDR_CXX11_CMDR_APP_INL_H
#define CMDR_CXX11_CMDR_APP_INL_H

#include <exception>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>


#include "cmdr_app.hh"
#include "cmdr_arg.hh"
#include "cmdr_cmd.hh"
#include "cmdr_cmn.hh"
#include "cmdr_internals.hh"
#include "cmdr_public.hh"
#include "cmdr_terminal.hh"


namespace cmdr {

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
        app_holder::instance().put(this);
    }

    inline app app::create(const_chars name, const_chars version,
                           const_chars author,
                           const_chars copyright,
                           const_chars description,
                           const_chars examples) {
        auto x = app{name, version, author, copyright, description, examples};
        x.initialize_internal_commands();
        return x;
    }

    inline void app::initialize_internal_commands() {
        _long = DEFAULT_KEY_PREFIX;
        register_actions();
        add_global_options(*this);
        add_generator_menu(*this);
        _on_command_not_hooked = [&](opt::cmd const &c, string_array const &remain_args) {
            std::cout << "INVOKING: " << std::quoted(c.title()) << ", remains: " << string::join(remain_args, ',') << ".\n";
            on_invoking_print_cmd(c,remain_args);
            return 0;
        };
    }

    inline void app::register_actions() {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
        _internal_actions.emplace(opt::RequestHelpScreen, [=](parsing_context &pc, int argc, char *argv[]) -> int {
            print_usages(&pc.last_matched_cmd());
            return 0;
        });
        _internal_actions.emplace(opt::RequestVersionsScreen, [=](parsing_context &pc, int argc, char *argv[]) -> int {
            std::cout << this->_version << std::endl;
            return 0;
        });
        _internal_actions.emplace(opt::RequestBuildInfoScreen, [=](parsing_context &pc, int argc, char *argv[]) -> int {
            std::stringstream compiler;
            compiler <<
            // https://blog.kowalczyk.info/article/j/guide-to-predefined-macros-in-c-compilers-gcc-clang-msvc-etc..html
            // http://beefchunk.com/documentation/lang/c/pre-defined-c/precomp.html
#if defined(__clang__)
                    "clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__
#elif defined(__GNUC__)
                            "gcc " <<  __GNUC__ <<"."<<__GNUC_MINOR__ // __VERSION__
#if defined(__GNU_PATCHLEVEL__)
                                                                 <<"."<<__GNUC_PATCHLEVEL__
#endif
#elif defined(_MSC_VER)
                            << "MSVC " << _MSC_FULL_VER
#elif defined(__EMSCRIPTEN__)
                            << "emscripten "
#elif defined(__MINGW32__)
                            << "MinGW 32bit " << __MINGW32_MAJOR_VERSION << "." << __MINGW32_MINOR_VERSION
#elif defined(__MINGW64__)
                            << "MinGW 64bit " << __MINGW64_VERSION_MAJOR << "." << __MINGW64_VERSION_MINOR
#else
                            << "UNKNOWN COMPILER "
#endif
                    ;

            std::tm t{};
            std::istringstream tsi(__TIMESTAMP__);
            // tsi.imbue(std::locale("de_DE.utf-8"));
            tsi >> std::get_time(&t, "%a %b %d %H:%M:%S %Y");
            // std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
            std::ostringstream ts;
            // ts << std::put_time(&t, "%Y-%m-%dT%H:%M:%S");
            ts << std::put_time(&t, "%FT%T%z");

            std::cout
                    << "Built by " << compiler.str()
                    << " at " << ts.str() << std::endl
                    << compiler.str() << std::endl
                    << ts.str() << std::endl;
            return 0;
        });
        _internal_actions.emplace(opt::RequestTreeScreen, [this](parsing_context &pc, int argc, char *argv[]) -> int { return print_tree_screen(pc, argc, argv); });
        _internal_actions.emplace(opt::RequestManualScreen, print_manual_screen);
        _internal_actions.emplace(opt::RequestDebugInfoScreen, print_debug_info_screen);
#pragma clang diagnostic pop
    }

    // inline void app::fatal_exit(std::string const &msg) {
    //     std::cerr << msg << '\n';
    //     exit(-1);
    // }

    // inline void app::fatal_exit(const std::string &msg) {
    //     std::cerr << msg << '\n';
    //     exit(-1);
    // }

    inline int app::on_invoking_print_cmd(opt::cmd const &hit, string_array const &remain_args) {
        unused(hit);
        unused(remain_args);
        std::cout << "command " << std::quoted(hit.title()) << " hit." << '\n';
        walk_args([](opt::arg &a) {
            if (a.hit_count() > 0) {
                std::cout << " - " << a.hit_count() << " hits: " << std::quoted(a.title())
                          << " (hit title: " << std::quoted(a.hit_title())
                          << ", spec:" << a.hit_special()
                          << ", long:" << a.hit_long()
                          << ", env:" << a.hit_env() << ")" << '\n';
            }
        });
        return 0;
    }

    inline int app::print_debug_info_screen(parsing_context &pc, int argc, char *argv[]) {
        unused(pc);
        unused(argc);
        unused(argv);
#if defined(_DEBUG) || 1
        // auto &store = cli.store();
        auto &store = cmdr::get_store();

        using namespace std::chrono_literals;

        store.set("app.server.tls.enabled", true);
        store.set("app.server.tls.ca.cert", "ca.cer");
        store.set("app.server.tls.server.cert", "server.cer");
        store.set("app.server.tls.server.key", "server.key");
        store.set("app.server.tls.client-auth", true);
        store.set("app.server.tls.handshake.timeout", 10s);
        store.set("app.server.tls.handshake.max-idle-time", 45min);
        store.set("app.server.tls.domains", std::vector{"example.com", "example.org"});
        // store.set("app.server.tls.fixed-list", std::array{"item1", "item2"});

#if defined(_DEBUG) || 1
        store.dump_full_keys(std::cout);
        store.dump_tree(std::cout);
#endif

        auto vv = store.get("server.tls.handshake.max-idle-time");
        (void) (vv);
        // std::cout << "max-idle-time: " << vv << '\n';
        //if (vv.as_string() != "45m")
        //    fatal_exit("  ^-- ERR: expect '45m'.");
#endif

#if defined(_DEBUG) || 1
        using namespace cmdr::terminal::colors;
        // auto &c = colorize::instance();
        auto c = colorize::create();
        std::cout << c.fg(c.Purple3).bg(c.Default).underline().bold().s("some text") << '\n';
        std::cout << c.dim().s("dim text") << '\n';
        std::cout << c.Purple3 << " << want 56\n";
        std::cout << c.MediumTurquoise << " << want 80\n";
        std::cout << c.Grey93 << " << want 255\n";

        using cmdr::terminal::colors::colorize;
        std::cout << colorize::style::underline << colorize::fg::red << "Hello, Colorful World!" << std::endl;
        std::cout << colorize::reset::all << "Here I'm!" << std::endl;
        std::cout << "END.\n\n";

        std::cout << "How many colors: " << cmdr::terminal::terminfo::support_colors() << '\n';
        std::cout << "Terminal: " << cmdr::terminal::terminfo::term() << '\n';
#endif
        return 0;
    }

    inline int app::print_manual_screen(parsing_context &pc, int argc, char *argv[]) {
        unused(pc);
        unused(argc);
        unused(argv);
        // todo print_manual_screen
        return 0;
    }

    // inline void _pr_tree(std::ostream &os, cmd *const cc) {}

    inline int app::print_tree_screen(parsing_context &pc, int argc, char *argv[]) {
        unused(pc);
        unused(argc);
        unused(argv);
        std::cout << "All Commands:\n";

        // _pr_tree(std::cout, &pc.last_matched_cmd());
        auto c = cmdr::terminal::colors::colorize::create();
        pc.last_matched_cmd().print_commands(std::cout, c, _minimal_tab_width, true, 0);
        return 0;
    }

    inline void app::add_global_options(app &cli) {
        // using namespace cmdr::opt;

        const bool hide_sys_tools = false;

        // help

        cli += cmdr::opt::sub_cmd{}("help", "h", "info", "usages")
                       .description("display this help screen")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true)
                       .on_invoke([](opt::cmd const &hit, string_array const &remain_args) -> int {
                           unused(hit);
                           unused(remain_args);
                           cmdr::get_app().print_usages(nullptr);
                           std::cout << "help, !!!\n";
                           std::cout << "args: " << cmdr::string::join(remain_args, ',', '[', ']') << std::endl;
                           // todo expand 'help ...' to a embedded help sub-system.
                           // print the help screen if remain_args is a valid command sequence.
                           // or enter an interactive question-and-answer system.
                           return 0;
                       });

        cli += cmdr::opt::opt{}("help", "h", "?", "info", "usages")
                       .description("display this help screen")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true)
                       .env_vars("HELP")
                       .on_hit([](opt::cmd const &hit, opt::arg const &hit_flag, string_array const &remain_args) -> opt::Action {
                           unused(hit);
                           unused(hit_flag);
                           unused(remain_args);
                           return opt::RequestHelpScreen;
                       });

        // version

        cli += cmdr::opt::sub_cmd{}("version", "V", "versions", "ver")
                       .description("display the version information")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true)
                       .on_invoke([](opt::cmd const &hit, string_array const &remain_args) -> int {
                           unused(hit);
                           unused(remain_args);
                           return opt::RequestVersionsScreen;
                       });

        cli += cmdr::opt::opt{}("version", "V", "versions", "ver")
                       .description("display the version information")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true)
                       .env_vars("VERSIONS")
                       .on_hit([](opt::cmd const &hit, opt::arg const &hit_flag, string_array const &remain_args) -> opt::Action {
                           unused(hit);
                           unused(hit_flag);
                           unused(remain_args);
                           return opt::RequestVersionsScreen;
                       });

        // build-info

        cli += cmdr::opt::opt{}("build-info", "#", "bdinf")
                       .description("display the building information")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true)
                       .on_hit([](opt::cmd const &hit, opt::arg const &hit_flag, string_array const &remain_args) -> opt::Action {
                           unused(hit);
                           unused(hit_flag);
                           unused(remain_args);
                           return opt::RequestBuildInfoScreen;
                       });

        // more...

        cli += cmdr::opt::opt{}("verbose", "v")
                       .description("verbose mode")
                       .env_vars("VERBOSE")
                       .group(SYS_MGMT_GROUP);
        cli += cmdr::opt::opt{}("quiet", "q")
                       .description("quiet mode")
                       .env_vars("QUIET")
                       .group(SYS_MGMT_GROUP);

        // debugging and tracing

        cli += cmdr::opt::sub_cmd{}("print", "pr", "display")
                       .description("print cmdr internal information")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true)
                       .on_invoke([&](auto &&...args) -> int { return cli.on_invoking_print_cmd(args...); })
                // .on_invoke(cli.on_invoking_print_cmd)
                ;

        cli += cmdr::opt::opt{}("tree")
                       .description("print all commands as a tree")
                       .hidden()
                       .special()
                       .no_non_special()
                       .group(SYS_MGMT_GROUP)
                       .on_hit([](opt::cmd const &hit, opt::arg const &hit_flag, string_array const &remain_args) -> opt::Action {
                           unused(hit);
                           unused(hit_flag);
                           unused(remain_args);
                           if (hit_flag.hit_special())
                               return opt::RequestTreeScreen;
                           return opt::RequestTreeScreen;
                       });

        cli += cmdr::opt::opt{}("debug", "D", "debug-mode")
                       .description("enable debugging mode for more verbose outputting")
                       .special()
                       .group(SYS_MGMT_GROUP)
                       .env_vars("DEBUG")
                       .on_hit([](opt::cmd const &hit, opt::arg const &hit_flag, string_array const &remain_args) -> opt::Action {
                           unused(hit);
                           unused(hit_flag);
                           unused(remain_args);
                           if (hit_flag.hit_special())
                               return opt::RequestDebugInfoScreen;
                           return opt::OK;
                       });

        cli += cmdr::opt::opt{}("short")
                       .description("enable shorter ~~debug")
                       .special()
                       .no_non_special()
                       .hidden()
                       .group(SYS_MGMT_GROUP);

        cli += cmdr::opt::opt{}("trace", "tr", "trace-mode")
                       .description("enable tracing mode for developer perspective")
                       .env_vars("TRACE")
                       .group(SYS_MGMT_GROUP);
    }

    inline void app::add_generator_menu(app &cli) {
        // using namespace cmdr::opt;

        // generators
        cli += cmdr::opt::sub_cmd{}("generate", "g", "gen")
                       .description("generators of this appT (such as manual, markdown, ...)")
                       .group(SYS_MGMT_GROUP)
                // .opt(opt_dummy{}())
                // .opt(opt_dummy{}());
                ;
        {
            auto &t1 = *cli.last_added_command();
            t1 += cmdr::opt::sub_cmd{}("doc", "d", "markdown", "docx", "pdf", "tex")
                          .description("generate a markdown document, or: pdf/TeX/...");
            // .opt(opt_dummy{}())
            // .opt(opt_dummy{}());

            auto c1 = *t1.last_added_command();
            c1 += cmdr::opt::opt{}("pages", "pg")
                          .description("set pdf pages")
                          .group("PDF");
            c1 += cmdr::opt::opt{1}("start-number", "sn", "start")
                          .description("set the start of auto-numbering in markdown export")
                          .group("Markdown");

            // t1 += cmdr::opt::opt<int>{}
            //               .titles("count", "c")
            //               .description("set counter value")
            //               .default_value(cmdr::vars::streamer_any((int16_t)(3)));

            t1 += cmdr::opt::sub_cmd{}("manual", "m", "man")
                          .description("generate linux man page.")
                    // .opt(opt_dummy{}())
                    // .opt(opt_dummy{}())
                    ;

            t1 += cmdr::opt::sub_cmd{}("shell", "s", "sh")
                          .description("generate the bash/zsh auto-completion script or install it.")
                    // .opt(opt_dummy{}())
                    // .opt(opt_dummy{}())
                    ;
        }
    }

    // inline app &app::operator+(const opt::opt &o) {
    //     cmd::operator+(a);
    //     return *this;
    // }
    //
    // inline app &app::operator+=(const opt::opt &o) {
    //     operator+(a);
    //     return *this;
    // }
    //
    // inline app &app::operator+(const opt::sub_cmd &o) {
    //     opt::cmd::operator+(a);
    //     return *this;
    // }
    //
    // inline app &app::operator+=(const opt::sub_cmd &o) {
    //     operator+=(a);
    //     return *this;
    // }


} // namespace cmdr


#endif //CMDR_CXX11_CMDR_APP_INL_H
