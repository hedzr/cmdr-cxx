#!/usr/bin/env python3

#  @copy Copyright © 2016 - 2024 Hedzr Yeh.
#
#  ask - C++17/C++20 Text Difference Utilities Library
#
#  This file is part of ask-lang/ask1x.
#
#  ask-lang toolset is free software: you can redistribute it and/or modify
#  it under the terms of the Apache 2.0 License.
#  Read /LICENSE for more information.
#
#  hedzr-cxx-cmake-submodule - a set of useful cmake scripts
#
#  This file is part of hedzr-cxx-cmake-submodule.
#
#  hedzr-cxx-cmake-submodule is free software: you can
#  redistribute it and/or modify it under the terms of
#  the Apache 2.0 License.
#  Read /LICENSE for more information.

import os
import re
import glob
import sys
import argparse


class VersionExtractor:
    def __init__(self, **kwargs):
        self.args = kwargs
        self.base_path = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
        self.build_serial = 1
        self.build_serial_file = os.path.join(self.base_path, ".build-serial")
        self.data = {"MAJOR": 0, "MINOR": 0, "PATCH": 1, "RELEASE": self.build_serial}
        # self.reg = re.compile(r"^\s*#define\s+DIFF_CXX_VERSION_STRING\s+([A-Z]+\(\")?([0-9]+).*$")
        self.reg = re.compile(
            r"^\s*#define\s+{mname}_([A-Z]+)_(VERSION|NUMBER)\s+([0-9]+).*$".format(
                **self.args
            )
        )
        self.re_ver = re.compile(
            r"^set\(VERSION ([0-9]+)(\.[0-9]+)(\.[0-9]+)(\.[0-9]+)\)$"
        )
        self.ok = False

    def read_build_serial(self):
        if os.path.isfile(self.build_serial_file):
            with open(self.build_serial_file, "r", encoding="utf-8") as fp:
                self.build_serial = int(fp.read().rstrip())
                self.build_serial += 1

    def update_build_serial(self):
        updated = False
        with open(self.build_serial_file, "w", encoding="utf-8") as fp:
            fp.seek(0)
            fp.truncate()
            fp.write(str(self.build_serial))
            updated = True
        if updated and self.args["refresh"]:
            # os.system("date")
            basic_list = ["./build", "./cmake-build-*"]
            user_build_dirs = [self.args["build_dir"]]
            in_first = set(basic_list)
            in_second = set(user_build_dirs)
            in_second_but_not_in_first = in_second - in_first
            result_list = basic_list + list(in_second_but_not_in_first)
            print("# Do cmake reconfigure at: {}".format(result_list))
            for dn in result_list:
                cf = os.path.join(self.base_path, dn, "CMakeCache.txt")
                if os.path.isfile(cf):
                    os.system("cmake --fresh -B {}".format(dn))

    def load_version_cmake(self):
        data_keys = list(self.data.keys()).copy()
        for dn in [".version.local.cmake", ".version.cmake.sample"]:
            filename = os.path.join(self.base_path, dn)
            need_update = False
            if not self.ok and os.path.isfile(filename):
                with open(filename, "r") as fp:
                    # print("checking {}".format(filename))
                    for l in fp:
                        m = self.re_ver.match(l)
                        if m:
                            # print(
                            #     "groups: {} | group(0): {} | group(2): {}".format(
                            #         m.groups(), m.group(0), m.group(2).lstrip(".")
                            #     )
                            # )
                            for ix, n in enumerate(data_keys, start=1):
                                self.data[n] = int(m.group(ix).lstrip("."))
                                # print("got from {}, {} <- {}".format(filename, n, data[n]))
                            print(
                                "serial: {}, RELEASE: {}".format(
                                    self.build_serial, self.data["RELEASE"]
                                )
                            )
                            if self.build_serial < self.data["RELEASE"] + 1:
                                need_update = True
                                self.data["RELEASE"] += 1
                                self.build_serial = self.data["RELEASE"]
                            elif self.build_serial > self.data["RELEASE"] + 1:
                                need_update = True
                            self.data["RELEASE"] = self.build_serial
                            self.ok = True
            if self.ok and os.path.isfile(filename) and need_update:
                print("write back to {}".format(filename))
                with open(filename, "w") as fp:
                    fp.seek(0)
                    fp.truncate()
                    fp.write(
                        "set(VERSION {MAJOR}.{MINOR}.{PATCH}.{RELEASE})".format(
                            **self.data
                        )
                    )

    def load_version_hh(self):
        # lookup xxx-version.hh in build/, cmake-build-debug/, ...
        basic_list = ["./build", "./cmake-build-*"]
        user_build_dirs = [self.args["build_dir"]]
        in_first = set(basic_list)
        in_second = set(user_build_dirs)
        in_second_but_not_in_first = in_second - in_first
        result_list = basic_list + list(in_second_but_not_in_first)
        print("# looking up version.hh at: {}".format(result_list))
        for dn in result_list:
            if not self.ok:
                config_h = os.path.join(
                    self.base_path, dn, "{name}-version.hh".format(**self.args)
                )
                cfg_list = glob.glob(config_h)
                for filename in cfg_list:
                    with open(filename, "r") as fp:
                        for l in fp:
                            m = self.reg.match(l)
                            if m:
                                self.data[m.group(1)] = int(m.group(3))
                                ok = True
                                self.data["RELEASE"] = self.build_serial
                                # print(
                                #     "got from {}, {} <- {}".format(
                                #         filename, m.group(1), m.group(3)
                                #     )
                                # )

    def print_it(self):
        print("{MAJOR}.{MINOR}.{PATCH}".format(**self.data))
        # print("{MAJOR}.{MINOR}.{PATCH}.{RELEASE}".format(**self.data))
        print("+{}".format(self.data["RELEASE"]))  # print build serial number too
        print("# post-build-run: {}".format(os.path.basename(__file__)))
        print(
            """#   project name: {name}, short: {sname}
#   c-style name: {cname}
#     macro name: {mname}
#      build dir: {build_dir}
""".format(
                **self.args
            )
        )


# base_path = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
#
# build_serial = 1
# build_serial_file = os.path.join(base_path, ".build-serial")
# with open(build_serial_file, "r+", encoding="utf-8") as fp:
#     build_sn = int(fp.read().rstrip())
#     build_sn += 1
#     # with open(build_serial, "w") as fp:
#     fp.seek(0)
#     fp.truncate()
#     fp.write(str(build_sn))
#
# data = {"MAJOR": 0, "MINOR": 0, "PATCH": 0, "RELEASE": build_sn}
# # reg = re.compile(r"^\s*#define\s+DIFF_CXX_VERSION_STRING\s+([A-Z]+\(\")?([0-9]+).*$")
# reg = re.compile(r"^\s*#define\s+DIFF_CXX_([A-Z]+)_(VERSION|NUMBER)\s+([0-9]+).*$")
# re_ver = re.compile(r"^set\(VERSION ([0-9]+)(\.[0-9]+)(\.[0-9]+)(\.[0-9]+)\)$")
#
# ok = False
# data_keys = list(data.keys()).copy()
# for dn in [".version.local.cmake", ".version.cmake"]:
#     if not ok:
#         filename = os.path.join(base_path, dn)
#         if os.path.isfile(filename):
#             with open(filename, "r+") as fp:
#                 # print("checking {}".format(filename))
#                 for l in fp:
#                     m = re_ver.match(l)
#                     if m:
#                         # print(
#                         #     "groups: {} | group(0): {} | group(2): {}".format(
#                         #         m.groups(), m.group(0), m.group(2).lstrip(".")
#                         #     )
#                         # )
#                         for ix, n in enumerate(data_keys, start=1):
#                             data[n] = int(m.group(ix).lstrip("."))
#                             # print("got from {}, {} <- {}".format(filename, n, data[n]))
#                         data["RELEASE"] = build_sn
#                         ok = True
#                 if ok:
#                     fp.seek(0)
#                     fp.truncate()
#                     fp.write(
#                         "set(VERSION {MAJOR}.{MINOR}.{PATCH}.{RELEASE})".format(**data)
#                     )
# # lookup xxx-version.hh in build/, cmake-build-debug/, ...
# for dn in ["build", "cmake-build-*"]:
#     if not ok:
#         config_h = os.path.join(base_path, dn, "proj-cxx-version.hh")
#         cfg_list = glob.glob(config_h)
#         for filename in cfg_list:
#             with open(filename, "r") as fp:
#                 for l in fp:
#                     m = reg.match(l)
#                     if m:
#                         data[m.group(1)] = int(m.group(3))
#                         ok = True
#                         data["RELEASE"] = build_sn
#                         # print(
#                         #     "got from {}, {} <- {}".format(
#                         #         filename, m.group(1), m.group(3)
#                         #     )
#                         # )

# print("{}.{}.{}".format(data["MAJOR"], data["MINOR"], data["PATCH"]))
#
# # print("{}.{}.{}.{}".format(data["MAJOR"], data["MINOR"], data["PATCH"], data["RELEASE"]))
#
# # print("+{}".format(data["RELEASE"])) # print build serial number too


def main(argv=sys.argv[1:]):
    parser = argparse.ArgumentParser(
        prog="versions-extractor",
        description="What the program does",
        epilog="Text at the bottom of help",
    )
    parser.add_argument(
        "-n", "--name", nargs="?", help="project name, eg 'proj-cxx', required"
    )
    parser.add_argument("-c", "--cname", nargs="?", help="c-style name, eg 'proj_cxx'")
    parser.add_argument(
        "-m", "--mname", nargs="?", help="c-macro-style name, eg 'PROJ_CXX'"
    )
    parser.add_argument("-s", "--sname", nargs="?", help="short name, eg 'prj'")
    parser.add_argument(
        "-b", "--build-dir", nargs="?", help="build directory such as ./build."
    )
    parser.add_argument(
        "-r",
        "--refresh",
        action=argparse.BooleanOptionalAction,
        default=None,
        help="refresh cmake configure?",
    )
    args = parser.parse_args()
    # print(args)
    if not args.name:
        print("FAIL: --name(-n) is required. Try: versions-extractor.py -n ask-cxx")
        exit(-1)
    if not args.cname:
        args.cname = args.name.replace("-", "_").lower()
    if not args.mname:
        args.mname = args.name.replace("-", "_").upper()
    if not args.sname:
        args.sname = args.name.replace("-cxx", "").replace("-cc", "")
    if not re.match(r"^[\/\.].*", args.build_dir):
        args.build_dir = "./" + args.build_dir
    kwargs = vars(args)

    ve = VersionExtractor(**kwargs)
    ve.read_build_serial()
    ve.load_version_cmake()
    ve.update_build_serial()
    ve.load_version_hh()
    ve.print_it()
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
