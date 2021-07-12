# cmdr-cxx {#mainpage}

![CMake Build Matrix](https://github.com/hedzr/cmdr-cxx/workflows/CMake%20Build%20Matrix/badge.svg) <!-- 
![CMake Build Matrix](https://github.com/hedzr/cmdr-cxx/workflows/CMake%20Build%20Matrix/badge.svg?event=release) 
--> [![GitHub tag (latest SemVer)](https://img.shields.io/github/tag/hedzr/cmdr-cxx.svg?label=release)](https://github.com/hedzr/cmdr-cxx/releases)

`cmdr-cxx` ^pre-release^ is a *header-only* command-line arguments parser in C++17 and higher. As a member of #cmdr series, it provides a fully-functional `Option Store` for your hierarchical configuration data.

<img width="914" alt="image-20210221210442215" src="https://user-images.githubusercontent.com/12786150/108625941-8fb04400-7488-11eb-8403-afc79825745c.png">

## Features

- POSIX-Compliant command-line argument parser

  - supports long flag (REQUIRED, `--help`, short flag (`-h`), and aliases (`--usage`, `--info`, ...)
  - supports multi-level sub-commands
  - supports short flag compat: `-vab` == `-v -a -b`, `-r3ap1zq` == `-r 3 -ap 1 -z -q`
  - supports passthrough flag: `--` will terminate the parsing
  - supports lots of data types for a flag: bool, int, uint, float, string, array, chrono duration, ...
    - allows user-custom data types
  - automated help-screen printing (*`-hhh` to print the hidden items*)

- Robust Interfaces

  - Hooks or Actions:

    - global: pre/post-invoke 
    - flags: on_hit
    - commands: on_hit, pre/post-invoke, invoke

  - Supports non-single-char short flag: `-ap 1`

  - Supports for `-D+`, `-D-` to enable/disable a bool option

  - Supports sortable command/flag groups

  - Supports toggleable flags - just like a radio button group

  - Free style flags arrangements: `$ app main sub4 bug-bug2 zero-sub3 -vqb2r1798b2r 234 --sub4-retry1 913 --bug-bug2-shell-name=fish ~~debug --int 67 -DDD --string 'must-be' --long 789`

  - Smart suggestions for wrong command and flags

    based on [Jaro-Winkler distance](https://en.wikipedia.org/wiki/Jaro–Winkler_distance). See [Snapshot](https://github.com/hedzr/cmdr-cxx/issues/1)

  - Builtin commands and flags

    - Help: `-h`, `-?`, `--help`, `--info`, `--usage`, ...
      - `help` command: `app help server pause` == `app server pause --help`.
    - Version & Build Info: `--version`/`--ver`/`-V`, `--build-info`/`-#`
    - `version`/`versions` command available.
    - Simulating version at runtime with `—-version-sim 1.9.1`
    
  - `~~tree`: lists all commands and sub-commands.
    
    - `~~debug`: print the debugging info
    - `--no-color`: disable terminal color in outputting
    - `--config <location>`: specify the location of the root config file. [only for yaml-loader]
    
  - Verbose & Debug: `—verbose`/`-v`, `—debug`/`-D`, `—quiet`/`-q`
    
  - Supports `-I/usr/include -I=/usr/include` `-I /usr/include -I:/usr` option argument specifications Automatically allows those formats (applied to long option too):

    - `-I file`, `-Ifile`, and `-I=files`
    - `-I 'file'`, `-I'file'`, and `-I='files'`
    - `-I "file"`, `-I"file"`, and `-I="files"`

  - Envvars overrides: `HELP=1 ./bin/test-app2-c2 server pause` is the equivalent of `./bin/test-app2-c2 server pause --help`

  - Extensible external loaders: `cli.set_global_on_loading_externals(...);`

  - Extending internal actions for special operations auch as printing help screen...

- Hierarchical Data Manager - `Option Store`

  - various data types supports
  - accusing the item with its dotted path key (such as `server.tls.certs.cert-bundle`)
  - See also [Fast Doc](#fast-document) section.





## Status

WIP, pre-released now.


- v0.2.15 - WIP, ...
- v0.2.13 - any kind of fixes; added priority-queue; added `ASSERTIONS_ONLY`, `NO_ASSERTIONS_ONLY`; added `is_iterable`, added vector_to_string(vec); `defer<T>` & `defer<bool>`; many improvements merged;
- v0.2.11 - maintained
- v0.2.10 - MSVC (Build Tool 16.7.2+, VS2019 passed), and others improvements (bash completion, ...)
- v0.2.9 - various fixes, improvements
- v0.2.8 - fixed cmdr11Config.cmake for importing transparently
- v0.2.7 - `auto &cli = cmdr::create(...)`
- v0.2.5 - public release starts

CXX 17 Compilers:

- gcc 10+: passed

- clang 12+: passed

- msvc build tool 16.7.2, 16.8.5 (VS2019 or Build Tool) passed



### Snapshots

`cmdr-cxx` prints an evident, clear, and logical help-screen. Please proceed the more snapshots at [#1 - Gallery](https://github.com/hedzr/cmdr-cxx/issues/1).



### Bonus

- golang: [cmdr](https:://github.com/hedzr/cmdr)
- .netCore: [Cmdr.Core](https://github.com/hedzr/Cmdr.Core)





## Usages



### Integrate to your cmake script

After installed at local cmake repository (Modules), `cmdr-cxx` can be integrated as your CMake module. So we might find and use it:

```cmake
find_package(cmdr11 REQUIRED)

add_executable(my-app)
target_link_libraries(my-app PRIVATE cmdr11::cmdr11)
```

Or you can download [deps-cmdr11.cmake](https://github.com/hedzr/cmdr-cxx/blob/master/cmake/deps-cmdr11.cmake) and include it:

```cmake
add_executable(my-app)

include(deps-cmdr11)     # put deps-cmdr11.cmake into your cmake module path at first
add_cmdr_cxx_to(my-app)
```





### Short example

```cpp
#include <cmdr11/cmdr11.hh>
#include "version.h" // xVERSION_STRING

int main(int argc, char *argv[]) {

  auto &cli = cmdr::cli("app2", xVERSION_STRING, "hedzr",
                        "Copyright © 2021 by hedzr, All Rights Reserved.",
                        "A demo app for cmdr-cxx library.",
                        "$ ~ --help");

  try {
    using namespace cmdr::opt;

    cli += sub_cmd{}("server", "s", "svr")
    .description("server operations for listening")
      .group("TCP/UDP/Unix");
    {
      auto &t1 = *cli.last_added_command();

      t1 += opt{(int16_t)(8)}("retry", "r")
      .description("set the retry times");

      t1 += opt{(uint64_t) 2}("count", "c")
      .description("set counter value");

      t1 += opt{"localhost"}("host", "H", "hostname", "server-name")
      .description("hostname or ip address")
        .group("TCP")
        .placeholder("HOST[:IP]")
        .env_vars("HOST");

      t1 += opt{(int16_t) 4567}("port", "p")
      .description("listening port number")
        .group("TCP")
        .placeholder("PORT")
        .env_vars("PORT", "SERVER_PORT");

      t1 += sub_cmd{}("start", "s", "startup", "run")
      .description("start the server as a daemon service, or run it at foreground")
        .on_invoke([](cmdr::opt::cmd const &c, string_array const &remain_args) -> int {
          UNUSED(c, remain_args);
          std::cout << c.title() << " invoked.\n";
          return 0;
        });
      auto &s1 = *t1.last_added_command();
      s1 += cmdr::opt::opt{}("foreground", "f")
      .description("run at fg");

      t1 += sub_cmd{}("stop", "t", "shutdown")
      .description("stop the daemon service, or stop the server");

      t1 += sub_cmd{}("pause", "p")
      .description("pause the daemon service");

      t1 += sub_cmd{}("resume", "re")
      .description("resume the paused daemon service");
      t1 += sub_cmd{}("reload", "r")
      .description("reload the daemon service");
      t1 += sub_cmd{}("hot-reload", "hr")
      .description("hot-reload the daemon service without stopping the process");
      t1 += sub_cmd{}("status", "st", "info", "details")
      .description("display the running status of the daemon service");
    }

  } catch (std::exception &e) {
    std::cerr << "Exception caught for duplicated cmds/args: " << e.what() << '\n';
    CMDR_DUMP_STACK_TRACE(e);
  }

  return cli.run(argc, argv);
}
```

It is a simple program.



## Fast Document



### Lookup a command and its flags

The operator `()` (`cli("cmd1.sub-cmd2.sub-sub-cmd")` ) could be used for retrieving a command (`cmdr::opt::cmd& cc`) from `cli`:

```cpp
auto &cc = cli("server");
CMDR_ASSERT(cc.valid());
CMDR_ASSERT(cc["count"].valid());     // the flag of 'server'
CMDR_ASSERT(cc["host"].valid());
CMDR_ASSERT(cc("status").valid());    // the sub-command of 'server'
CMDR_ASSERT(cc("start").valid());     // sub-command: 'start'
CMDR_ASSERT(cc("run", true).valid()); // or alias: 'run'
```

Once `cc` is valid, use `[]` to extract its flags.

The dotted key is allowed. For example: `cc["start.port"].valid()`.

```cpp
CMDR_ASSERT(cli("server.start").valid());
CMDR_ASSERT(cli("server.start.port").valid());

// get flag 'port' of command 'server.start':
CMDR_ASSERT(cc["start.port"].valid());
```





### Extract the matched information of a flag

While a flag given from command-line is matched ok, it holds some hit info. Such as:

```cpp
auto &cc = cli("server");  // get 'server' command object
CMDR_ASSERT(cc.valid());   // got ok?

CMDR_ASSERT(cc["count"].valid());
CMDR_ASSERT(cc["count"].hit_long()); 	          // if `--count` given
CMDR_ASSERT(cc["count"].hit_long() == false); 	// if `-c` given
CMDR_ASSERT(cc["count"].hit_count() == 1);     	// if `--count` given
CMDR_ASSERT(cc["count"].hit_title() == "c");   	// if `-c` given

CMDR_ASSERT(cli["verbose"].hit_count() == 3);   // if `-vvv` given

// hit_xxx are available for a command too
CMDR_ASSERT(cc.hit_title() == "server");        // if 'server' command given

```

The value of a flag from command-line will be saved into `Option Store`, and extracted by shortcut `cmdr::get_for_cli()` . For example:

```cpp
auto verbose = cmdr::get_for_cli<bool>("verbose");
auto hostname = cmdr::get_for_cli<std::string>("server.host");
```

In `Option Store`, the flag value will be prefixed by `"app.cli."`, and get_for_cli wraps transparently.

> The normal entries in `Options Store` are prefixed by string `"app."`. You could define another one of course.

To extract the normal configuration data, `cmdr::set` and `cmdr::get` are best choices. They will wrap and unwrap the prefix `app` transparently.

```cpp
auto verbose = cmdr::get<bool>("cli.verbose");
auto hostname = cmdr::get<std::string>("cli.server.host");
```

If you wanna extract them directly:

```cpp
auto verbose = cmdr::get_store().get_raw<bool>("app.cli.verbose");
auto hostname = cmdr::get_store().get_raw<std::string>("app.cli.server.host");

auto verbose = cmdr::get_store().get_raw_p<bool>("app.cli", "verbose");
auto hostname = cmdr::get_store().get_raw_p<std::string>("app.cli", "server.host");
```



### Set the value of a config item

Every entry in `Option Store` that we call it a config item. The entries are hierarchical. So we locate it with a dotted key path string.

A config item is free for data type dynamically. That is saying, you could change the data type of a item at runtime. Such as setting one entry to integer array, from integer originally.

> But it is hard for coding while you're working for a c++ program.

```cpp
cmdr::set("wudao.count", 1);
cmdr::set("wudao.string", "str");
cmdr::set("wudao.float", 3.14f);
cmdr::set("wudao.double", 2.7183);
cmdr::set("wudao.array", std::vector{"a", "b", "c"});
cmdr::set("wudao.bool", false);

std::cout << cmdr::get<int>("wudao.count") << '\n';
auto const &aa = cmdr::get< std::vector<char const*> >("wudao.array");
std::cout << cmdr::string::join(aa, ", ", "[", "]") << '\n';

// Or: maybe you will like to stream out a `variable` with standard format.
cmdr::vars::variable& ab = cmdr::get_app().get("wudao.array");
std::cout << ab << '\n';
```



### `cmdr::vars::variable`

`cmdr-cxx` provides stream-io on lots of types via `cmdr::vars::variable`, take a look for further.





## Features to improve your app arch

`cmdr-cxx` provides some debugging features or top view to improve you design at CLI-side.

### Default Action

We've been told that we can bind an action (via `on_invoke`) to a (sub-)command:

```cpp
t1 += sub_cmd{}("start", "s", "startup", "run")
  .description("start the server as a daemon service, or run it at foreground")
  .on_invoke([](cmdr::opt::cmd const &c, string_array const &remain_args) -> int {
    UNUSED(c, remain_args);
    std::cout << c.title() << " invoked.\n";
    return 0;
  });
```

For those commands without binding to `on_invoke`, `cmdr-cxx` will invoke a default one, For example:

```cpp
t1 += sub_cmd{}("pause", "p")
  .description("pause the daemon service");
```

While the end-user is typing and they will got:

```bash
❯ ./bin/test-app2-c2 server pause
INVOKING: "pause, p", remains: .
command "pause, p" hit.
```

Simple naive? Dislike it? The non-hooked action can be customized.

#### User-custom non-hooked action

Yes you can:

```cpp
#if CMDR_TEST_ON_COMMAND_NOT_HOOKED
cli.set_global_on_command_not_hooked([](cmdr::opt::cmd const &, string_array const &) {
  cmdr::get_store().dump_full_keys(std::cout);
  cmdr::get_store().dump_tree(std::cout);
  return 0;
});
#endif
```



### `~~debug`

Special flag has the leading sequence chars `~~`.

`~~debug` can disable the command action and print the internal hitting information.

```bash
❯ ./bin/test-app2-c2 server pause -r 5 -c 3 -p 1357 -vvv ~~debug
command "pause, p" hit.
 - 1 hits: "--port=PORT, -p" (hit title: "p", spec:0, long:0, env:0) => 1357
 - 1 hits: "--retry, -r" (hit title: "r", spec:0, long:0, env:0) => 5
 - 1 hits: "--count, -c" (hit title: "c", spec:0, long:0, env:0) => 3
 - 3 hits: "--verbose, -v" (hit title: "v", spec:0, long:0, env:0) => true
 - 1 hits: "--debug, -D, --debug-mode" (hit title: "debug", spec:true, long:true, env:false) => true

```

Another one in [Gallary](https://github.com/hedzr/cmdr-cxx/issues/1):

<img width="912" alt="image-20210217161825139" src="https://user-images.githubusercontent.com/12786150/108175867-39679c00-713c-11eb-8ef2-4d363d37755e.png">



#### `-DDD`

Triple `D` means `--debug --debug --debug`. In `~~debug` mode,  triple `D` can dump more underlying value structure inside `Option Store`.

> The duplicated-flag exception there among others, is expecting because we're in testing.

<img width="1009" alt="image-20210211184120423" src="https://user-images.githubusercontent.com/12786150/107626849-5a8e3f80-6c99-11eb-8e7b-278a50702c8c.png">



#### `~~debug --cli -DDD`

The values of CLI flags are ignored but `~~cli` can make them raised when dumping. See the snapshot at [#1 - Gallary](https://github.com/hedzr/cmdr-cxx/issues/1).





### `~~tree`

This flag will print the command hierarchical structure:

<img width="895" alt="image-20210222130046734" src="https://user-images.githubusercontent.com/12786150/108664649-2246f680-750e-11eb-8e6d-0437f9797e63.png">



### Remove the cmdr-cxx tail line

By default a citation line(s) will be printed at the ends of help screen:

<img width="684" alt="image-20210215100547030" src="https://user-images.githubusercontent.com/12786150/107898103-da582a80-6f75-11eb-9ffc-02cdd2af249d.png">

I knew this option is what you want:

```cpp
    auto &cli = cmdr::cli("app2", CMDR_VERSION_STRING, "hedzr",
                         "Copyright © 2021 by hedzr, All Rights Reserved.",
                         "A demo app for cmdr-c11 library.",
                         "$ ~ --help")
            // remove "Powered by cmdr-cxx" line
            .set_no_cmdr_endings(true)
            // customize the last line except cmdr endings
            // .set_tail_line("")
            .set_no_tail_line(true);
```

The "Type `...` ..." line could be customized by `set_tail_line(str)`, so called `tail line`,. Or, you can disable the `tail line`  by `set_no_tail_line(bool)`.

The `Powered by ...` line can be disabled by `set_no_cmdr_ending`, so-called `cmdr-ending` line.

## External Loaders

There is a builtin addon `yaml-loader` for loading the external config files in the pre-defined directory locations. As a sample to show you how to write a external loader, `yaml-loader` will load and parse the yaml config file and merge it into `Option Store`.

> TODO: `conf.d` not processed now.

`test-app-c1` demonstrates how to use it:

```cpp
{
  using namespace cmdr::addons::loaders;
  cli.set_global_on_loading_externals(yaml_loader{}());
}
```

The coresponding cmake fragment might be:

```cmake
#
# For test-app-c1, loading the dependency to yaml-cpp
#
include(loaders/yaml_loader)
add_yaml_loader(test-app2-c1)
```

> This add-on needs a third-part library,`yaml-cpp`, presented.

## Specials

Inside `cmdr-cxx`, there are many optimizable points and some of them in working.

- [x] enable dim text in terminal

  ```bash
  CMDR_DIM=1 ./bin/test-app2-c2 main sub4 bug-bug2
  ```

- [x] `--no-color`: do NOT print colorful text with [Terminal Escaped Sequences](https://en.wikipedia.org/wiki/ANSI_escape_code), envvars `PLAIN` or `NO_COLOR` available too.

  ```bash
  ./bin/test-app2-c2 --no-color
  PLAIN=1 ./bin/test-app-c2
  ```

- [x] enable very verbose debugging

  ```cpp
  #define CMDR_ENABLE_VERBOSE_LOG 1
  #include <cmdr11/cmdr11.hh>
  ```

- [x] enable unhandled exception handler

  ```cpp
  cmdr::debug::UnhandledExceptionHookInstaller _ueh{}; // for c++ exceptions
  cmdr::debug::SigSegVInstaller _ssi{};                // for SIGSEGV ...
  return cli.run(argc, argv);
  ```

- [x] `-hhh` (i.e. `--help --help --help`) will print the help screen with those invisible items (the hidden commands and flags).

- [x] Tab-stop position is adjustable based the options automatically

- [x] The right-side of a line, in the help screen, command/flag decriptions usually, can be wrapped and aligned along the tab-stop width.

- [ ] More...




## For Developer



### Build

> gcc 10+: passed
>
> clang 12+: passed
>
> msvc build tool 16.7.2, 16.8.5 (VS2019 or Build Tool) passed

```bash
# configure
cmake -DENABLE_AUTOMATE_TESTS=OFF -S . -B build/
# build
cmake --build build/
# install
cmake --build build/ --target install
# sometimes maybe sudo: sudo cmake --build build/ --target install
```

### ninja, [Optional]

We used ninja for faster building.


### Other Options

1. `BUILD_DOCUMENTATION`=OFF
2. `ENABLE_TESTS`=OFF





### Prerequisites

To run all automated tests, or, you're trying to use `yaml-loader` add-on, some dependencies need to prepared at first, by youself, maybe.



#### Catch2

If the tests are enabled, [ `Catch2`](https://github.com/catchorg/Catch2) will be downloaded while cmake configuring and building automatically. If you have a local cmake-findable Catch2 copy, more attentions would be appreciated.



#### Others

In our tests, `test-app2-c1` and `yaml-loader` will request [`yaml-cpp`](https://github.com/jbeder/yaml-cpp) is present.

> Optional

##### Linux

```bash
sudo apt install -y libyaml-cpp-dev
```

> For CentOS or RedHat: `sudo dnf install yaml-cpp yaml-cpp-devel yaml-cpp-static`

##### macOS

```bash
brew install yaml-cpp
```

##### Windows

```bash
vcpkg install yaml-cpp
```

> **NOTE** that [vcpkg](https://github.com/microsoft/vcpkg) want to inject the control file for cmake building, see also [Using vcpkg with CMake](https://github.com/microsoft/vcpkg#using-vcpkg-with-cmake)





### Run the examples

The example executables can be found in `./bin` after built. For example:

```bash
# print command tree (with hidden commands)
./bin/cmdr11-cli -hhh ~~tree
```

1. ~~You will get them from release page~~.
2. TODO: we will build a docker release later.
3. Run me from a online CXX IDE.



### Hooks in cmdr-cxx



1. `auto & cli = cmdr::get_app()`

2. Register actions:

   `void register_action(opt::Action action, opt::types::on_internal_action const &fn);`

   In your pre_invoke handler, some actions called `internal actions` could by triggered via the returned `Action` code.

   The `Action` codes is extensible, followed by a `on_internal_action` handler user-customized.

3. Hooks

   > `xxx_handlers` or `s`(`_externals`) means you can specify it multiple times.

   1. `set_global_on_arg_added_handlers`, `set_global_on_cmd_added_handlers`

   2. `set_global_on_arg_matched_handlers`, `set_global_on_cmd_matched_handlers`

   3. `set_global_on_loading_externals`

   4. `set_global_on_command_not_hooked`

      cmdr prints some hitting info for a sub-command while no `on_invoke` handler associated with it.

      Or, you can specify one yours via `set_global_on_command_not_hooked`.

   5. `set_global_on_post_run_handlers`

   6. `set_on_handle_exception_ptr`

   7. `set_global_pre_invoke_handler`, `set_global_post_invoke_handler`

      

   8. 





## Thanks to JODL

Thanks to [JetBrains](https://www.jetbrains.com/?from=cmdr-cxx) for donating product licenses to help develop **cmdr-cxx** [![jetbrains](https://gist.githubusercontent.com/hedzr/447849cb44138885e75fe46f1e35b4a0/raw/bedfe6923510405ade4c034c5c5085487532dee4/jetbrains-variant-4.svg)](https://www.jetbrains.com/?from=hedzr/cmdr-cxx)







## LICENSE

MIT
