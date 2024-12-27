//
// Created by Hedzr Yeh on 2021/1/25.
//

#ifndef CMDR_CXX11_CMDR_APP_INL_H
#define CMDR_CXX11_CMDR_APP_INL_H

#include <exception>
#include <fstream>
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

#if defined(_WIN32) || defined(_WIN64)
    {
      char* buf = nullptr;
      size_t sz = 0;
      if (_dupenv_s(&buf, &sz, "CMDR_DIM") == 0 && buf != nullptr) {
        if (*buf == '1') {
          vars::store::_dim_text_dim = true;
        }
        free(buf);
      }
    }
#else
    if (auto *p = std::getenv("CMDR_DIM"); p && p[0] == '1') {
      vars::store::_dim_text_dim = true;
    }
#endif
  }


  inline void app::register_action(opt::Action action, opt::types::on_internal_action const &fn) {
    _internal_actions.emplace(action, fn);
  }

  inline std::string compiler_name() {
    std::stringstream compiler;
    compiler
    // https://blog.kowalczyk.info/article/j/guide-to-predefined-macros-in-c-compilers-gcc-clang-msvc-etc..html
    // http://beefchunk.com/documentation/lang/c/pre-defined-c/precomp.html
    // and: https://github.com/arnemertz/online-compilers/blob/gh-pages/compiler_version.cpp
#if defined(__clang__)
        << "clang " << __clang_major__ << "." << __clang_minor__ << "." << __clang_patchlevel__
#elif defined(__ICC) || defined(__INTEL_COMPILER)
        << "Intel " << __INTEL_COMPILER
#elif defined(__GNUC__) || defined(__GNUG__)
        << "gcc " << __GNUC__ << "." << __GNUC_MINOR__ // __VERSION__
#if defined(__GNU_PATCHLEVEL__)
        << "." << __GNUC_PATCHLEVEL__
#endif
#elif defined(__HP_cc) || defined(__HP_aCC)
        << "HP " << __HP_aCC
#elif defined(__IBMC__) || defined(__IBMCPP__)
        << "IBM " << __IBMCPP__
#elif defined(_MSC_VER)
        << "MSVC " << _MSC_FULL_VER
#elif defined(__PGI)
        << "Portland PGCPP" << __VERSION__
#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
        << "Solaris Studio" << __SUNPRO_CC
#elif defined(__EMSCRIPTEN__)
        << "emscripten "
#elif defined(__MINGW32__)
        << "MinGW 32bit " << __MINGW32_MAJOR_VERSION << "." << __MINGW32_MINOR_VERSION
#elif defined(__MINGW64__)
        << "MinGW 64bit " << __MINGW64_VERSION_MAJOR << "." << __MINGW64_VERSION_MINOR
#else
        << "UNKNOWN COMPILER "
#endif

#if CHECK_BOOST_VERSION
        << " (Boost version: " << BOOST_LIB_VERSION << ')'
#endif

#if (__cplusplus == 201703L)
        << " (C++17)"
#elif (__cplusplus == 201402L)
        << " (C++14)"
#elif (__cplusplus == 201103L)
        << " (C++11)"
#elif (__cplusplus == 199711L)
        << " (C++98)"
#elif (__cplusplus == 202002L)
        << " (C++20)"
#elif (__cplusplus > 202302L)
        << " (C++26)"
#elif (__cplusplus > 202002L)
        << " (C++23)"
#else
        << " (pre-standard C++)"
#endif
        ;
    return compiler.str();
  }

  inline void app::internal_register_actions() {
    _internal_actions.emplace(opt::Action::RequestHelpScreen, [this](opt::types::parsing_context &pc, int, char *[]) -> int {
      this->print_usages(&pc.last_matched_cmd());
      return 0;
    });
    _internal_actions.emplace(opt::Action::RequestVersionsScreen, [this](opt::types::parsing_context &, int, char *[]) -> int {
      auto vs     = this->_version;
      auto &vsvar = this->get_for_cli("version-sim");
      if (!vsvar.empty()) vs = vsvar.as<const_chars>();
      if (vs.empty()) vs = this->_version;
      std::cout << vs << '\n';
      return 0;
    });
    _internal_actions.emplace(opt::Action::RequestBuildInfoScreen, [](opt::types::parsing_context &, int, char *[]) -> int {
      std::tm t{};
      std::istringstream tsi(__TIMESTAMP__);
      // tsi.imbue(std::locale("de_DE.utf-8"));
      tsi >> std::get_time(&t, "%a %b %d %H:%M:%S %Y");
      // std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
      std::ostringstream ts;
      // ts << std::put_time(&t, "%Y-%m-%dT%H:%M:%S");
      ts << std::put_time(&t, "%FT%T%z");

      auto cn   = compiler_name();
      auto &cli = cmdr::get_app();
      std::cout
          << "Built by " << cn
          << " at " << ts.str() << '\n'
          << cn << '\n'
          << ts.str() << '\n'
          << CMDR_PROJECT_NAME << '\n'
          << CMDR_ARCHIVE_NAME << '\n'
          << CMDR_GIT_BRANCH << '\n'
          << CMDR_GIT_COMMIT_HASH << '\n'
          << cli.name() << '\n'
          << cli.version() << '\n';
      return 0;
    });
    _internal_actions.emplace(opt::Action::RequestTreeScreen,
                              [this](opt::types::parsing_context &pc, int argc, char *argv[]) -> int { return print_tree_screen(pc, argc, argv); });
    _internal_actions.emplace(opt::Action::RequestManualScreen,
                              [this](opt::types::parsing_context &pc, int argc, char *argv[]) -> int { return print_manual_screen(pc, argc, argv); });
    _internal_actions.emplace(opt::Action::RequestDebugInfoScreen,
                              [this](opt::types::parsing_context &pc, int argc, char *argv[]) -> int {
                                return print_debug_info_screen(pc, argc, argv);
                              });
  }


  inline int app::on_invoking_print_cmd(opt::cmd const &hit, string_array const &remain_args) {
    UNUSED(hit, remain_args);
    using Colors256 = terminal::colors::colorize::Colors256;
    std::cout << "command " << std::quoted(hit.title()) << " hit." << '\n';
    walk_args([this](opt::arg &a) {
      if (a.hit_count() > 0) {
        auto k  = a.dotted_key();
        auto &v = _store.get_raw(k);
        std::ostringstream os1, os2, os3, os4;
        os1 << a.hit_count() << " hits: ";
        os2 << vars::store::_c.fg(Colors256::Green).s(os1.str());
        os3 << " (hit title: " << std::quoted(a.hit_title())
            << ", spec:" << a.hit_special()
            << ", long:" << a.hit_long()
            << ", env:" << a.hit_env() << ")";
        os4 << vars::store::_c.fg(Colors256::Grey).italic().s(os3.str());
        std::cout << " - " << os2.str() << std::quoted(a.title())
                  << os4.str() << ": '" << k << "' => " << v << '\n';
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

      auto &cli                       = (*this); // get_app();
      int const tilde_debug_hit_count = tilde_debug_arg.hit_count();
      bool const allow_cli_sub_key    = cli.get_for_cli("cli").as<bool>();
      auto const &cli_prefix          = cli.cli_prefix();
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
        std::cout << c.fg(colorize::Colors256::Purple3).bg(colorize::Colors256::Default).underline().bold().s("some text") << '\n';
        std::cout << c.dim().s("dim text") << '\n';
        std::cout << (int) colorize::Colors256::Purple3 << " << want 56" << '\n';
        std::cout << (int) colorize::Colors256::MediumTurquoise << " << want 80" << '\n';
        std::cout << (int) colorize::Colors256::Grey93 << " << want 255" << '\n';

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

    auto const &help_arg         = find_flag("help");
    bool const show_hidden_items = help_arg.valid() && help_arg.hit_count() > 2;
    bool const sc_mode           = get_for_cli("shell-completion").as<bool>();

    // _pr_tree(std::cout, &pc.last_matched_cmd());
    auto c                       = cmdr::terminal::colors::colorize::create();
    pc.last_matched_cmd().print_commands(std::cout, c, _minimal_tab_width, true, show_hidden_items, sc_mode, 0);
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
                 auto key                  = cmdr::string::join(remain_args, '.');
                 cmdr::opt::cmd const *ptr = &cmdr::get_app();
                 for (auto const &k: remain_args) {
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
                 return opt::Action::RequestHelpScreen;
               });

    // version

    cli += cmdr::opt::sub_cmd{}("version", "V", "versions", "ver")
               .description("display the version information")
               .group(SYS_MGMT_GROUP)
               .hidden(hide_sys_tools | true)
               .on_pre_invoke([](cmdr::opt::cmd const &hit, string_array const &remain_args) -> int {
                 UNUSED(hit, remain_args);
                 throw opt::cmdr_requests_exception(opt::Action::RequestVersionsScreen);
               });

    cli += cmdr::opt::opt{}("version", "V", "versions", "ver")
               .description("display the version information")
               .group(SYS_MGMT_GROUP)
               .hidden(hide_sys_tools | true)
               .env_vars("VERSIONS")
               .on_hit([](cmdr::opt::cmd const &hit, cmdr::opt::arg const &hit_flag, string_array const &remain_args) -> cmdr::opt::Action {
                 UNUSED(hit, hit_flag, remain_args);
                 return opt::Action::RequestVersionsScreen;
               });

    cli += cmdr::opt::opt{""}("version-sim", "vs")
               .description("simulate the version dynamically")
               .group(SYS_MGMT_GROUP)
               .hidden(hide_sys_tools | true)
               .env_vars("VERSION_SIM");

    // build-info

    cli += cmdr::opt::opt{}("build-info", "#", "bdinfo")
               .description("display the building information")
               .group(SYS_MGMT_GROUP)
               .hidden(hide_sys_tools | true)
               .on_hit([](cmdr::opt::cmd const &hit, cmdr::opt::arg const &hit_flag, string_array const &remain_args) -> cmdr::opt::Action {
                 UNUSED(hit, hit_flag, remain_args);
                 return opt::Action::RequestBuildInfoScreen;
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

    cli += cmdr::opt::opt{}("feel-like", "bet", "bet")
               .description("best choice will be invoked for unknown command")
               .group(SYS_MGMT_GROUP)
               .hidden()
               .env_vars("FEEL_LIKE");

    cli += cmdr::opt::opt{}("no-color", "nc")
               .description("disable color text in terminal")
               .group(SYS_MGMT_GROUP)
               .hidden()
               .env_vars("PLAIN", "NO_COLOR");

    cli += cmdr::opt::opt{}("shell-completion", "sc")
               .description("disable help screen skeleton for completion")
               .group(SYS_MGMT_GROUP)
               .hidden()
               .env_vars("SHELL_COMPLETION")
               .on_hit([&](cmdr::opt::cmd const &hit, cmdr::opt::arg const &hit_flag, string_array const &remain_args) -> cmdr::opt::Action {
                 UNUSED(hit, hit_flag, remain_args);
#if CMDR_ENABLE_AUTO_COMPLETION_LOGFILE == 1
                 DEBUG_ONLY({
                   bool shell_completion_mode = get_for_cli("shell-completion").as<bool>();
                   if (shell_completion_mode) {
                     this->_sa_lf = std::ofstream(path::get_executable_path().filename().string() + ".log");
                     for (auto const &v: _args_cache)
                       this->_sa_lf << v << ' ';
                     this->_sa_lf << '\n';
                   }
                 })
#endif
                 return cmdr::opt::Action::Continue;
               });

    cli += cmdr::opt::opt{}("tree")
               .description("print all commands as a tree")
               .hidden()
               .special()
               .no_non_special()
               .group(SYS_MGMT_GROUP)
               .on_hit([](cmdr::opt::cmd const &hit, cmdr::opt::arg const &hit_flag, string_array const &remain_args) -> cmdr::opt::Action {
                 UNUSED(hit, hit_flag, remain_args);
                 if (hit_flag.hit_special())
                   return cmdr::opt::Action::RequestTreeScreen;
                 return cmdr::opt::Action::RequestTreeScreen;
               });

    cli += cmdr::opt::opt{}("debug", "D", "debug-mode")
               .description("enable debugging mode for more verbose outputting")
               .special()
               .group(SYS_MGMT_GROUP)
               .env_vars("DEBUG")
               .on_hit([](cmdr::opt::cmd const &hit, cmdr::opt::arg const &hit_flag, string_array const &remain_args) -> cmdr::opt::Action {
                 UNUSED(hit, hit_flag, remain_args);
                 if (hit_flag.hit_special())
                   return cmdr::opt::Action::RequestDebugInfoScreen;
                 return cmdr::opt::Action::OK;
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

      t1 += cmdr::opt::opt{}("feel-like", "bet", "bet")
                .description("The best choice will be applied")
                .group("Generators")
                .env_vars("FEEL_LIKE");
      t1 += cmdr::opt::opt{}("dry-run", "dry", "dry")
                .description("Don't generate the result file(s)")
                .group("Generators")
                .env_vars("DRY_RUN");
      t1 += cmdr::opt::opt{}("stdout", "c")
                .description("Prints the result file(s)")
                .group("Generators")
                .env_vars("STDOUT");


      t1 += cmdr::opt::sub_cmd{}("doc", "d", "markdown", "docx", "pdf", "tex")
                .description("generate a markdown document, or: pdf/TeX/...");
      // .opt(opt_dummy{}())
      // .opt(opt_dummy{}());

      auto &c1 = *t1.last_added_command();
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
                .on_invoke([&](auto &&...args) -> int { return cli.on_generate_shell_completion(args...); });
      auto &csh = *t1.last_added_command();
      csh += cmdr::opt::opt{}("drop", "d", "remove", "uninstall")
                 .description("Uninstall/erase the installed completion file");
      csh += cmdr::opt::opt{true}("auto", "a")
                 .description("prints shell-completion scripts within current shell")
                 .toggle_group("which-shell");
      csh += cmdr::opt::opt{}("bash", "b")
                 .description("prints the bash-completion scripts for this app")
                 .toggle_group("which-shell");
      csh += cmdr::opt::opt{}("zsh", "z")
                 .description("prints the zsh-completion scripts for this app")
                 .toggle_group("which-shell");
      csh += cmdr::opt::opt{}("fish", "f")
                 .description("prints the fish-completion scripts for this app")
                 .toggle_group("which-shell");
      csh += cmdr::opt::opt{""}("name", "n")
                 .description("use this name instead of executable name")
                 .placeholder("FILENAME");
    }
  }

  inline int app::on_generate_shell_completion(opt::cmd const &hit, string_array const &remain_args) {
    UNUSED(hit, remain_args);
    auto dry_run            = hit.owner()->find_flag("dry-run")->hit_count() > 0;
    auto bet                = hit.owner()->find_flag("feel-like")->hit_count() > 0;
    auto print_to_stdout    = hit.owner()->find_flag("stdout")->hit_count() > 0;
    auto const &which_shell = hit.toggle_group("which-shell");
    std::ostringstream os;
    os << "which-shell: " << which_shell;
    std::cerr << vars::store::_c.fg(vars::store::_dim_text_fg).s(os.str()) << '\n';

    std::string ws = which_shell;
    if (which_shell == "auto") {
      ws = util::detect_shell_env();
      os.str("");
      os << "which-shell detected: " << ws;
      std::cerr << vars::store::_c.fg(vars::store::_dim_text_fg).s(os.str()) << '\n';
    }

    if (ws == "bash") {
      generate_bash_completion(dry_run, bet, print_to_stdout);
    } else {
      std::cerr << "Not implemented for '" << std::quoted(ws) << "' yet" << '\n';
    }
    return 0;
  }

  inline bool _write_bash_comp_file_to(std::filesystem::path &path, std::filesystem::path &safe_name, std::string &contents, bool drop, std::string const &ok_msg) {
    bool written{};
    try {
      // dir /= filename;
      if (drop) {
        if (std::filesystem::remove(path)) {
          std::cerr << path << " removed" << '\n';
          written = true;
        } else {
          std::cerr << "cannot remove " << path << '\n';
        }
      } else {
        std::ofstream out(path);
        if (out) {
          util::defer<bool> const dx_fn(std::bind(&std::ofstream::close, &out));
          out << contents;
          if (out) {
            written = true;
            cross::setenv("TARGET", path.string().c_str());
            cross::setenv("TARGET_DIR", path.parent_path().string().c_str());
            std::cerr << vars::store::dark_text(string::expand_env(ok_msg));
          }
        }
      }
    } catch (...) {
      UNUSED(safe_name);
    }
    return written;
  }


  // inline app &app::operator+(const opt::opt &o) {
  inline int app::generate_bash_completion(bool dry_run, bool bet, bool print_to_stdout) {
    const char *tpl  = R"SH-EOF(
# bash completion wrapper for %{EXE_NAME} v%{APP_VERSION}
# This file is generated by cmdr-cxx core.
#
# Put this file as /usr/local/etc/bash_completion.d/%{SAFE_APP_NAME}, and 
# Re-login to enable the new bash completion script.
# Or source it in your current TTY:
#   $ source %{SAFE_APP_NAME}
#

_cmdr_cmd_help_events () {
  $* --help -hhncsc|awk -F'|' '{print $1}'|awk -F',' '{ for (i=1;i<=NF;i++) {gsub(/^[ \t]+|[ \t]+$/, "", $i); print $i;} }'
}


_cmdr_cmd_%{SAFE_APP_NAME}() {
  local cmd="%{EXE_NAME}" cur prev words
  _get_comp_words_by_ref cur prev words
  if [ "$prev" != "" ]; then
    unset 'words[${#words[@]}-1]'
  fi

  COMPREPLY=()
  #pre=${COMP_WORDS[COMP_CWORD-1]}
  #cur=${COMP_WORDS[COMP_CWORD]}

  case "$prev" in
    --help|--version)
      COMPREPLY=()
      return 0
      ;;
    $cmd)
      COMPREPLY=( $(compgen -W "$(_cmdr_cmd_help_events $cmd)" -- ${cur}) )
      return 0
      ;;
    *)
      COMPREPLY=( $(compgen -W "$(_cmdr_cmd_help_events ${words[@]})" -- ${cur}) )
      return 0
      ;;
  esac

  #opts="--help --version -q --quiet -v --verbose --system --dest="
  #opts="--help upgrade version deploy undeploy log ls ps start stop restart"
  opts="--help -hhncsc"
  # grep "^  [^ \[\$\#\!/\\@\"']"|
  # -hh --no-color --shell-completion
  cmds=$($cmd --help -hhncsc|awk -F'|' '{print $1}'|awk -F',' '{ for (i=1;i<=NF;i++) {gsub(/^[ \t]+|[ \t]+$/, "", $i); print $i;} }')

  COMPREPLY=( $(compgen -W "${opts} ${cmds}" -- ${cur}) )

} # && complete -F _cmdr_cmd_%{SAFE_APP_NAME} %{EXE_NAME}

#
# [[ -r "/usr/local/etc/profile.d/bash_completion.sh" ]] && . "/usr/local/etc/profile.d/bash_completion.sh"
#
if type complete >/dev/null 2>&1; then
	# bash
	complete -F _cmdr_cmd_%{SAFE_APP_NAME} %{EXE_NAME}
elif type compdef >/dev/null 2>&1; then
	# zsh
	_cmdr_cmd_${SAFE_APP_NAME}_zsh() { compadd $(_cmdr_cmd_%{SAFE_APP_NAME}); }
	compdef _cmdr_cmd_${SAFE_APP_NAME}_zsh %{EXE_NAME}
fi

)SH-EOF";

    bool const drop  = get_for_cli("generate.shell.drop").as<bool>();

    // auto *safe_name = std::getenv("SAFE_APP_NAME");
#if defined(_WIN32) || defined(_WIN64)
    std::string szbuf;
    {
      char* buf = nullptr;
      size_t sz = 0;
      if (_dupenv_s(&buf, &sz, "EXE_NAME") == 0 && buf != nullptr)
      {
        szbuf = buf;
        free(buf);
      }
    }
    auto *exe_name = szbuf.c_str();
#else
    auto *exe_name   = std::getenv("EXE_NAME");
#endif

    const auto *name    = get_for_cli("generate.shell.name").as<const_chars>();
    const const_chars n = name && *name ? name : exe_name;
    std::string sn      = n;
    string::replace(sn, "-", "_");
    cross::setenv("SAFE_APP_NAME", sn.c_str());
    std::filesystem::path safe_name(sn);

    std::string contents = tpl;
    string::replace_all(contents, "%{SAFE_APP_NAME}", sn);
    string::replace_all(contents, "%{EXE_NAME}", exe_name);
    string::replace_all(contents, "%{APP_NAME}", n);
#if defined(_WIN32) || defined(_WIN64)
    std::string szbuf1;
    {
      char* buf1 = nullptr;
      size_t sz = 0;
      if (_dupenv_s(&buf1, &sz, "APP_VERSION") == 0 && buf1 != nullptr) {
        szbuf1 = buf1;
        free(buf1);
      }
    }
    auto* appver = szbuf1.c_str();
#else
    char *appver = std::getenv("APP_VERSION");
#endif
    string::replace_all(contents, "%{APP_VERSION}", appver);

    std::filesystem::path path{};
    if (!dry_run) {
      bool written{};
      if (bet) {
        for (auto const *dir: std::vector<const_chars>{
               "$HOME/.linuxbrew/etc/bash_completion.d",
               "/opt/homebrew/etc/bash_completion.d",
               "/usr/local/etc/bash_completion.d",
               "/etc/bash_completion.d",
             }) {
          // if (std::filesystem::exists(path))
          //     if(auto st=std::filesystem::status(path);st.permissions())
          path = string::expand_env(dir);
          path /= safe_name;
          written = _write_bash_comp_file_to(path, safe_name,
                                             contents, drop,
                                             R"("$SAFE_APP_NAME" was written to $TARGET_DIR/.
Re-login your terminal/tty to take the effects.
Or apply it in-place:
$ source $TARGET

)");
          if (!written && !drop) {
            std::cerr << "can't write to " << path << ". ignored" << '\n';
          }
          if (written)
            break;
        }
      }

      if (!written) {
        path    = safe_name;
        written = _write_bash_comp_file_to(path, safe_name, contents, drop, R"("$SAFE_APP_NAME" was written.
Put it into `/usr/local/etc/bash_completion.d` or
`/opt/homebrew/etc/bash_completion.d`, and
Re-login your terminal/tty to take the effects.
Or apply it in-place:
$ source $TARGET

)");
      }
    }

    if (print_to_stdout && !drop)
      std::cout << contents << '\n';
    else if (drop) {
      if (dry_run)
        std::cout << path << " will be removed" << '\n';
    }
    return 0;
  }

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


#endif // CMDR_CXX11_CMDR_APP_INL_H
