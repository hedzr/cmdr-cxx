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
#include "cmdr_terminal.hh"


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
        add_global_options(*this);
        add_generator_menu(*this);
    }

    inline void app::add_global_options(cmdr::opt::app &cli) {
        // using namespace cmdr::opt;

        const bool hide_sys_tools = false;

        // help

        cli += cmdr::opt::subcmd{}
                       .titles("help", "h", "info", "usages")
                       .description("display this help screen")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true)
                       .on_invoke([](cmd const &hit, string_array const &remain_args) -> int {
                           unused(hit);
                           unused(remain_args);
                           cmdr::get_app().print_usages(nullptr);
                           std::cout << "help, !!!\n";
                           std::cout << "args: " << cmdr::string::join(remain_args, ',', '[', ']') << std::endl;
                           return 0;
                       });

        cli += cmdr::opt::opt<bool>{}
                       .titles("help", "h", "?", "info", "usages")
                       .description("display this help screen")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true)
                       .on_hit([](cmd const &hit, arg const &hit_flag, string_array const &remain_args) -> details::Action {
                           unused(hit);
                           unused(hit_flag);
                           unused(remain_args);
                           // std::cout << "help, !!!\n";
                           // cmdr::get_app().print_usages(const_cast<cmd *>(&hit));
                           return details::RequestHelpScreen;
                       });

        // version

        cli += cmdr::opt::subcmd{}
                       .titles("version", "V", "versions", "ver")
                       .description("display the version information")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true);

        cli += cmdr::opt::opt<bool>{}
                       .titles("version", "V", "versions", "ver")
                       .description("display the version information")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true);

        // build-info

        cli += cmdr::opt::opt<bool>{}
                       .titles("build-info", "#", "bdinf")
                       .description("display the building information")
                       .group(SYS_MGMT_GROUP)
                       .hidden(hide_sys_tools | true);

        // more...

        cli += cmdr::opt::opt<bool>{}
                       .titles("verbose", "v")
                       .description("verbose mode")
                       .group(SYS_MGMT_GROUP);
        cli += cmdr::opt::opt<bool>{}
                       .titles("quiet", "q")
                       .description("quiet mode")
                       .group(SYS_MGMT_GROUP);

        // debugging and tracing

        cli += cmdr::opt::opt<bool>{}
                       .titles("tree")
                       .description("print all commands as a tree")
                       .hidden()
                       .special()
                       .no_non_special()
                       .group(SYS_MGMT_GROUP);

        cli += cmdr::opt::opt<bool>{}
                       .titles("debug", "D", "debug-mode")
                       .description("enable debugging mode for more verbose outputting")
                       .special()
                       .group(SYS_MGMT_GROUP);

        cli += cmdr::opt::opt<bool>{}
                       .titles("trace", "tr", "trace-mode")
                       .description("enable tracing mode for developer perspective")
                       .group(SYS_MGMT_GROUP);
    }

    inline void app::add_generator_menu(cmdr::opt::app &cli) {
        // using namespace cmdr::opt;

        // generators
        cli += cmdr::opt::subcmd{}
                       .titles("generator", "g", "gen")
                       .description("generators of this app (such as manual, markdown, ...)")
                       .group(SYS_MGMT_GROUP)
                       .opt(opt_dummy{}())
                       .opt(opt_dummy{}());
        {
            auto &t1 = *cli.last_added_command();
            t1 += cmdr::opt::subcmd{}
                          .titles("doc", "d", "markdown", "docx", "pdf", "tex")
                          .description("generate a markdown document, or: pdf/TeX/...")
                          .opt(opt_dummy{}())
                          .opt(opt_dummy{}());

            t1 += cmdr::opt::opt<int>{}
                          .titles("count", "c")
                          .description("set counter value")
                          .default_value(cmdr::support_types((int16_t)(3)));

            t1 += cmdr::opt::subcmd{}
                          .titles("manual", "m", "man")
                          .description("generate linux man page.")
                          .opt(opt_dummy{}())
                          .opt(opt_dummy{}());

            t1 += cmdr::opt::subcmd{}
                          .titles("shell", "s", "sh")
                          .description("generate the bash/zsh auto-completion script or install it.")
                          .opt(opt_dummy{}())
                          .opt(opt_dummy{}());
        }
    }

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


} // namespace cmdr::opt


#endif //CMDR_CXX11_CMDR_APP_INL_H
