# cmdr-cxx {#mainpage}

![CMake Build Matrix](https://github.com/hedzr/cmdr-cxx/workflows/CMake%20Build%20Matrix/badge.svg) <!-- 
![CMake Build Matrix](https://github.com/hedzr/cmdr-cxx/workflows/CMake%20Build%20Matrix/badge.svg?event=release) 
--> [![GitHub tag (latest SemVer)](https://img.shields.io/github/tag/hedzr/cmdr-cxx.svg?label=release)](https://github.com/hedzr/cmdr-cxx/releases)

`cmdr-cxx` ^pre-release^ is a *header-only* command-line arguments parser in C++17 and higher. As a member of #cmdr series, it provides a fully-functional `Option Store` for your hierarchical configuration data.

<img width="1003" alt="image-20210211180854495" src="https://user-images.githubusercontent.com/12786150/107623507-64fa0a80-6c94-11eb-9b9e-a273cfbca9a5.png">

## Features

- POSIX-Compliant command-line argument parser

  - supports long flag (REQUIRED, `--help`, short flag (`-h`), and aliases (`--usage`, `--info`, ...)
  - supports multi-level sub-commands
  - supports short flag compat: `-vab` == `-v -a -b`, `-r3ap1zq` == `-r 3 -ap 1 -z -q`
  - supports passthrough flag: `--` will terminate the parsing
  - supports lots of data types for a flag: bool, int, uint, float, string, array, chrono duration, ...
    - allows user-custom data types
  - automated help-screen printing 

- Robust Interfaces

  - hooks or actions:

    - global: pre/post-invoke 
    - flags: on_hit
    - commands: on_hit, pre/post-invoke, invoke

  - supports non-single-char short flag: `-ap 1`

  - supports for `-D+`, `-D-` to enable/disable a bool option

  - supports sortable command/flag groups

  - free style flags arrangements: `$ app main sub4 bug-bug2 zero-sub3 -vqb2r1798b2r 234 --sub4-retry1 913 --bug-bug2-shell-name=fish ~~debug --int 67 -DDD --string 'must-be'`

  - Smart suggestions for wrong command and flags

    based on [Jaro-Winkler distance](https://en.wikipedia.org/wiki/Jaro–Winkler_distance). See [Snapshot](https://github.com/hedzr/cmdr-cxx/issues/1)

  - builtin commands and flags

    - Help: `-h`, `-?`, `--help`, `--info`, `--usage`, ...
      - `help` command: `app help server pause` == `app server pause --help`.
    - Version & Build Info: `--version`/`--ver`/`-V`, `--build-info`/`-#`
    - `version`/`versions` command available.
    - Simulating version at runtime with `—-version-sim 1.9.1`
  - `~~tree`: list all commands and sub-commands.
    - `~~debug`: print the debugging info
    - `--no-color`: disable terminal color in outputting
    - `--config <location>`: specify the location of the root config file. [only for yaml-loader]
    - Verbose & Debug: `—verbose`/`-v`, `—debug`/`-D`, `—quiet`/`-q`

  - Supports `-I/usr/include -I=/usr/include` `-I /usr/include -I:/usr` option argument specifications Automatically allows those formats (applied to long option too):

    - `-I file`, `-Ifile`, and `-I=files`
    - `-I 'file'`, `-I'file'`, and `-I='files'`
    - `-I "file"`, `-I"file"`, and `-I="files"`

  - envvars overrides: `HELP=1 ./bin/test-app2-c2 server pause` is the equivalent of `./bin/test-app2-c2 server pause --help`

  - Extending internal actions for special operations auch as printing help screen...

- Hierarchical Data Manager - `Option Store`

  - various data types supports
  - getting and setting by a dotted path key
  - See also 
  - 



## Status

WIP, pre-released now.



## Build

> gcc 10+: passed
>
> clang 12+: passed

```bash
# configure
cmake -DENABLE_AUTOMATE_TESTS=OFF -S . -B build/
# build
cmake --build build/
# install
cmake --build build/ --target install
```



### Prerequisites

To run all automated tests, or, you're trying to use `yaml-loader` add-on, some dependencies need to prepared at first.

#### Linux

```bash
sudo apt install -y libyaml-cpp-dev
```

> For CentOS or RedHat, maybe the compiling from source codes is necessary.

#### macOS

```bash
brew install yaml-cpp
```







### Run the examples

The example executables can be found in `./bin`. For example:

```bash
./bin/app2
```



### Snapshots

`cmdr-cxx` prints an evident, clear, and logical help-screen. Please proceed the more snapshots at [#1 - Gallery](https://github.com/hedzr/cmdr-cxx/issues/1).





## Usages



### Integrated to your cmake script

After installed at local, `cmdr-cxx` can be integrated as your CMake module. So we might find and use it:

```cmake
find_library(cmdr11)

add_executable(my-app)
target_link_libraries(my-app PRIVATE cmdr11::cmdr11)
```



### Short example

```cpp
#include <cmdr11/cmdr11.hh>
#include "version.h" // xVERSION_STRING

int main(int argc, char *argv[]) {

    auto cli = cmdr::cli("app2", xVERSION_STRING, "hedzr",
                         "Copyright © 2021 by hedzr, All Rights Reserved.",
                         "A demo app for cmdr-c11 library.",
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

This is a simple program.



### Lookup a command and its flags

The operator `()` (`cli("cmd1.sub-cmd2.sub-sub-cmd")` ) could be used for retrieving a command (`cmdr::opt::cmd& cc`) from `cli`:

```cpp
auto &cc = cli("server");
CMDR_ASSERT(cc.valid());
CMDR_ASSERT(cc["count"].valid());  // the flag of 'server'
CMDR_ASSERT(cc["host"].valid());
CMDR_ASSERT(cc("status").valid()); // the sub-command of 'server'
CMDR_ASSERT(cc("start").valid());
CMDR_ASSERT(cc("run", true).valid());
```

Once `cc` is valid, use `[]` to extract its flags.



### Extract the matched information of a flag

While a flag given from command-line is matched ok, it holds some hit info. Such as:

```cpp
auto &cc = cli("server");
CMDR_ASSERT(cc.valid());

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

In `Option Store`, the flag value will be prefixed by `app.cli`, and get_for_cli wraps transparently.

To extract the normal configuration data, `cmdr::set` and `cmdr::get` are best choice. They will wrap and unwrap the prefix `app` transparently.

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

A config item is free data type dynamically. That is saying, you could change the data type of a item at runtime. Such as setting one entry to integer array, from integer originally.



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

`cmdr-cxx` provides stream in and out on lots of types via `cmdr::vars::variable`, take a look for further.





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

For a command without binding to `on_invoke`, `cmdr-cxx` will invoke a default one, For example:

```cpp
t1 += sub_cmd{}("pause", "p")
  .description("pause the daemon service");
```

While the end-user is typing and will get:

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



#### `-DDD`

Triple `D` means `--debug --debug --debug`. In `~~debug` mode,  triple `D` can dump more underlying value structure inside `Option Store`.

> The duplicated-flag exception there among others, is expecting because we're in testing.

<img width="1009" alt="image-20210211184120423" src="https://user-images.githubusercontent.com/12786150/107626849-5a8e3f80-6c99-11eb-8e7b-278a50702c8c.png">



#### `~~debug --cli -DDD`

The values of CLI flags are ignored but `~~cli` can force them dumped. See the snapshot at [#1 - Gallary](https://github.com/hedzr/cmdr-cxx/issues/1).





### `~~tree`

This flag will print the command hierarchical structure:

<img width="919" alt="image-20210211175837313" src="https://user-images.githubusercontent.com/12786150/107622691-29127580-6c93-11eb-9b80-38c73b85eb6b.png">





## External Loaders

There is a pre-built addon `yaml-loader` for loading the external config files in the pre-defined directory locations. `test-app-c1` demonstrates how to use it:

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



## Optimizations

Inside `cmdr-cxx`, there are many optimizable points in working.

- [x] enable dim text in terminal

  ```bash
  CMDR_DIM=1 ./bin/test-app2-c2 main sub4 bug-bug2
  ```

- [x] `--no-color`

- [x] enable very verbose debugging

  ```cpp
  #define CMDR_ENABLE_VERBOSE_LOG
  #include <cmdr11/cmdr11.hh>
  ```

- [x] enable unhandled exception handler

  ```cpp
  cmdr::debug::UnhandledExceptionHookInstaller _ueh{}; // for c++ exceptions
  cmdr::debug::SigSegVInstaller _ssi{};                // for SIGSEGV ...
  return cli.run(argc, argv);
  ```

- [ ] 





## Thanks to JODL

Thanks to [JetBrains](https://www.jetbrains.com/?from=cmdr-cxx) for donating product licenses to help develop **cmdr-cxx** [![jetbrains](https://gist.githubusercontent.com/hedzr/447849cb44138885e75fe46f1e35b4a0/raw/bedfe6923510405ade4c034c5c5085487532dee4/jetbrains-variant-4.svg)](https://www.jetbrains.com/?from=hedzr/cmdr-cxx)







## LICENSE

MIT
