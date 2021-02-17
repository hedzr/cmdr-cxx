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


#include "cmdr-version.hh"


#include "cmdr_cmn.hh"

#include "cmdr_arg.hh"
#include "cmdr_cmd.hh"

#include "cmdr_app.hh"

#include "cmdr_internals.hh"
#include "cmdr_public.hh"

#include "cmdr_terminal.hh"


namespace cmdr {

    inline app::app(const_chars name, const_chars version, const_chars author,
                    const_chars copyright, const_chars description_,
                    const_chars examples_)
        : _name(name)
        , _version(version)
        , _author(author)
        , _copyright(copyright)
    // , _description(description)
    // , _examples(examples)
    {
        opt::bas::description(description_);
        opt::bas::examples(examples_);
        app_holder::instance().put(this);
    }

    inline app &app::create_new(const_chars name, const_chars version,
                                const_chars author,
                                const_chars copyright,
                                const_chars description,
                                const_chars examples) {
        auto *x = new app{name, version, author, copyright, description, examples};
        x->initialize_internal_commands();
        return *app_holder::instance();
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
        _long = DEFAULT_CLI_KEY_PREFIX;

        internal_register_actions();
        add_global_options(*this);
        add_generator_menu(*this);

        _on_command_not_hooked = [&](opt::cmd const &c, string_array const &remain_args) {
            std::cout << "INVOKING: " << std::quoted(c.title()) << ", remains: " << string::join(remain_args, ',') << ".\n";
            on_invoking_print_cmd(c, remain_args);
            return 0;
        };

        if (auto *p = std::getenv("CMDR_DIM"); p && p[0] == '1') {
            vars::store::_dim_text_dim = true;
        }
    }

    inline void app::register_action(opt::Action action, opt::types::on_internal_action const &fn) {
        _internal_actions.emplace(action, fn);
    }

    inline void app::internal_register_actions() {
        _internal_actions.emplace(opt::RequestHelpScreen, [=](opt::types::parsing_context &pc, int, char *[]) -> int {
            print_usages(&pc.last_matched_cmd());
            return 0;
        });
        _internal_actions.emplace(opt::RequestVersionsScreen, [=](opt::types::parsing_context &, int, char *[]) -> int {
            auto vs = this->_version;
            auto &vsvar = this->get_for_cli("version-sim");
            if (!vsvar.empty()) vs = vsvar.as<std::string>();
            if (vs.empty()) vs = this->_version;
            std::cout << vs << '\n';
            return 0;
        });
        _internal_actions.emplace(opt::RequestBuildInfoScreen, [=](opt::types::parsing_context &, int, char *[]) -> int {
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
                    << " at " << ts.str() << '\n'
                    << compiler.str() << '\n'
                    << ts.str() << '\n'
                    << CMDR_PROJECT_NAME << '\n'
                    << CMDR_ARCHIVE_NAME << '\n'
                    << CMDR_GIT_BRANCH << '\n'
                    << CMDR_GIT_COMMIT_HASH << '\n';
            return 0;
        });
        _internal_actions.emplace(opt::RequestTreeScreen,
                                  [this](opt::types::parsing_context &pc, int argc, char *argv[]) -> int { return print_tree_screen(pc, argc, argv); });
        _internal_actions.emplace(opt::RequestManualScreen, print_manual_screen);
        _internal_actions.emplace(opt::RequestDebugInfoScreen,
                                  [=](opt::types::parsing_context &pc, int argc, char *argv[]) -> int {
                                      return print_debug_info_screen(pc, argc, argv);
                                  });
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
        UNUSED(hit, remain_args);
        std::cout << "command " << std::quoted(hit.title()) << " hit." << '\n';
        walk_args([=](opt::arg &a) {
            if (a.hit_count() > 0) {
                auto k = a.dotted_key();
                auto &v = _store.get_raw(k);
                std::cout << " - " << a.hit_count() << " hits: " << std::quoted(a.title())
                          << " (hit title: " << std::quoted(a.hit_title())
                          << ", spec:" << a.hit_special()
                          << ", long:" << a.hit_long()
                          << ", env:" << a.hit_env() << ") => " << v << '\n';
            }
        });
        return 0;
    }

    inline int app::print_debug_info_screen(opt::types::parsing_context &pc, int argc, char *argv[]) {
        UNUSED(pc, argc, argv);

        // auto &store = cli.store();
        // auto &store = cmdr::get_store();

#if defined(_DEBUG) && 0
        using namespace std::chrono_literals;

        _store.set("app.server.tls.enabled", true);
        _store.set("app.server.tls.ca.cert", "ca.cer");
        _store.set("app.server.tls.server.cert", "server.cer");
        _store.set("app.server.tls.server.key", "server.key");
        _store.set("app.server.tls.client-auth", true);
        _store.set("app.server.tls.handshake.timeout", 10s);
        _store.set("app.server.tls.handshake.max-idle-time", 45min);
        _store.set("app.server.tls.domains", std::vector{"example.com", "example.org"});
        // _store.set("app.server.tls.fixed-list", std::array{"item1", "item2"});
#endif

        // auto tilde_debug_arg = pc._root->operator[]("debug");
        auto tilde_debug_arg = (*pc._root)["debug"];
        if (tilde_debug_arg.valid()) {

            auto &cli = (*this); // get_app();
            int tilde_debug_hit_count = tilde_debug_arg.hit_count();
            bool allow_cli_sub_key = cli.get_for_cli("cli").as<bool>();
            auto const &cli_prefix = cli.cli_prefix();
            std::function<bool(std::pair<vars::store::key_type, vars::store::node_pointer> const &)>
                    filter{};
            if (!allow_cli_sub_key) filter = [&](std::pair<vars::store::key_type, vars::store::node_pointer> const &p) -> bool {
                return !string::has_prefix(p.first, cli_prefix);
            };

            if (tilde_debug_hit_count > 1) {
                dump_full_keys_f(std::cout, filter, "Dumping the Option Store ...");
                // store.dump_full_keys_f(cli._dim_text_fg, cli._dim_text_dim, std::cout,
                //                        [](std::pair<vars::store::key_type, vars::store::node_pointer> const &) -> bool {
                //                            return false;
                //                        });
            }

            if (tilde_debug_hit_count > 2) {
                dump_tree_f(std::cout, filter, "Dumping the Option Store as Tree ...");
                // dump_tree_f(std::cout, [](std::pair<vars::store::key_type, vars::store::node_pointer> const &) -> bool {
                //     return false;
                // });
                // dump_tree_f();
            }

            if (tilde_debug_hit_count > 5) {

                // auto vv = store.get("server.tls.handshake.max-idle-time");
                // (void) (vv);
                // std::cout << "max-idle-time: " << vv << '\n';
                // if (vv.as_string() != "45m")
                //    fatal_exit("  ^-- ERR: expect '45m'.");

                using namespace cmdr::terminal::colors;
                // auto &c = colorize::instance();
                auto c = colorize::create();
                std::cout << c.fg(c.Purple3).bg(c.Default).underline().bold().s("some text") << '\n';
                std::cout << c.dim().s("dim text") << '\n';
                std::cout << c.Purple3 << " << want 56" << '\n';
                std::cout << c.MediumTurquoise << " << want 80" << '\n';
                std::cout << c.Grey93 << " << want 255" << '\n';

                using cmdr::terminal::colors::colorize;
                std::cout << colorize::style::underline << colorize::fg::red << "Hello, Colorful World!" << '\n';
                std::cout << colorize::reset::all << "Here I'm!" << '\n';
                std::cout << "END." << '\n'
                          << '\n';

                std::cout << "How many colors: " << cmdr::terminal::terminfo::support_colors() << '\n';
                std::cout << "Terminal: " << cmdr::terminal::terminfo::term() << '\n'
                          << '\n';
            }
        }

        on_invoking_print_cmd(pc.curr_command(), pc.remain_args());
        return 0;
    }

    inline int app::print_manual_screen(opt::types::parsing_context &pc, int argc, char *argv[]) {
        UNUSED(pc, argc, argv);
        // todo print_manual_screen
        return 0;
    }

    // inline void _pr_tree(std::ostream &os, cmd *const cc) {}

    inline int app::print_tree_screen(opt::types::parsing_context &pc, int argc, char *argv[]) {
        UNUSED(pc, argc, argv);
        std::cout << "All Commands:\n";
        
        auto const &help_arg = find_flag("help");
        bool show_hidden_items = help_arg.valid() && help_arg.hit_count() > 2;
        
        // _pr_tree(std::cout, &pc.last_matched_cmd());
        auto c = cmdr::terminal::colors::colorize::create();
        pc.last_matched_cmd().print_commands(std::cout, c, _minimal_tab_width, true, show_hidden_items, 0);
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
                       .on_invoke([](cmdr::opt::cmd const &hit, string_array const &remain_args) -> int {
                           UNUSED(hit, remain_args);
                           auto key = cmdr::string::join(remain_args, '.');
                           cmdr::opt::cmd const *ptr = &cmdr::get_app();
                           for (auto const &k : remain_args) {
                               ptr = ptr->find_command(k, true);
                               if (!ptr) break;
                           }
                           cmdr::get_app().print_usages(ptr);
                           std::cout << "help, !!!\n";
                           std::cout << "args: " << cmdr::string::join(remain_args, ',', '[', ']') << '\n';
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
                       .on_hit([](cmdr::opt::cmd const &hit, cmdr::opt::arg const &hit_flag, string_array const &remain_args) -> cmdr::opt::Action {
                           UNUSED(hit, hit_flag, remain_args);
                           return cmdr::opt::RequestHelpScreen;
                       });

        // version

        cli += cmdr::opt::sub_cmd{}("version", "V", "versions", "ver")
                       .description("display the version information")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true)
                       .on_pre_invoke([](cmdr::opt::cmd const &hit, string_array const &remain_args) -> int {
                           UNUSED(hit, remain_args);
                           throw opt::cmdr_requests_exception(opt::RequestVersionsScreen);
                       });

        cli += cmdr::opt::opt{}("version", "V", "versions", "ver")
                       .description("display the version information")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true)
                       .env_vars("VERSIONS")
                       .on_hit([](cmdr::opt::cmd const &hit, cmdr::opt::arg const &hit_flag, string_array const &remain_args) -> cmdr::opt::Action {
                           UNUSED(hit, hit_flag, remain_args);
                           return cmdr::opt::RequestVersionsScreen;
                       });

        cli += cmdr::opt::opt{""}("version-sim", "vs")
                       .description("simulate the version dynamically")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true)
                       .env_vars("VERSION_SIM");

        // build-info

        cli += cmdr::opt::opt{}("build-info", "#", "bdinf")
                       .description("display the building information")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true)
                       .on_hit([](cmdr::opt::cmd const &hit, cmdr::opt::arg const &hit_flag, string_array const &remain_args) -> cmdr::opt::Action {
                           UNUSED(hit, hit_flag, remain_args);
                           return cmdr::opt::RequestBuildInfoScreen;
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

        cli += cmdr::opt::opt{}("no-color")
                       .description("disable color text in terminal")
                       .group(SYS_MGMT_GROUP)
                       .hidden()
                       .env_vars("PLAIN", "NO_COLOR");

        cli += cmdr::opt::opt{}("tree")
                       .description("print all commands as a tree")
                       .hidden()
                       .special()
                       .no_non_special()
                       .group(SYS_MGMT_GROUP)
                       .on_hit([](cmdr::opt::cmd const &hit, cmdr::opt::arg const &hit_flag, string_array const &remain_args) -> cmdr::opt::Action {
                           UNUSED(hit, hit_flag, remain_args);
                           if (hit_flag.hit_special())
                               return cmdr::opt::RequestTreeScreen;
                           return cmdr::opt::RequestTreeScreen;
                       });

        cli += cmdr::opt::opt{}("debug", "D", "debug-mode")
                       .description("enable debugging mode for more verbose outputting")
                       .special()
                       .group(SYS_MGMT_GROUP)
                       .env_vars("DEBUG")
                       .on_hit([](cmdr::opt::cmd const &hit, cmdr::opt::arg const &hit_flag, string_array const &remain_args) -> cmdr::opt::Action {
                           UNUSED(hit, hit_flag, remain_args);
                           if (hit_flag.hit_special())
                               return cmdr::opt::RequestDebugInfoScreen;
                           return cmdr::opt::OK;
                       });

        cli += cmdr::opt::opt{}("cli")
                       .description("enable 'app.cli.xxx' in ~~debug output")
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
                       .description("generators of this app (such as manual, markdown, ...)")
                       .group(SYS_MGMT_GROUP)
                       .hidden()
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

            t1 += cmdr::opt::sub_cmd{}("shell", "s", "sh", "comp", "completion")
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
