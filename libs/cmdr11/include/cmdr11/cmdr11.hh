/**
 * cmdr -- C++17 Command Line Arguments Parser
 *
 * @file
 * @brief Utilities for string operations.
 *
 * @ , copyright © 2016 - 2021 Hedzr Yeh.
 * @ , license {This project is released under the MIT License.}
 *
 * This file is part of cmdr (cmdr-c17 for C++ version).
 *
 * cmdr is free software: you can redistribute it and/or modify
 * it under the terms of the MIT License..
 *
 * cmdr is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the MIT License
 * along with cmdr.  If not, see <https://github.com/hedzr/cmdr-c17/blob/master/LICENSE>.
 *
 * CHANGELOG
 *   and additional copyright holders.
 *
 *   2021/1/8
 *   - Initial version, written by hedzr @ CQ
 *
 */

#ifndef CMDR_CXX11_CMDR11_HH
#define CMDR_CXX11_CMDR11_HH

#pragma once


// #define CMDR_CAST_CONST_CHARS_AS_STD_STRING     1
// To cast a const char* string as a std::string instance in extracting
// arg default value, uncomment the above line.

// #define CMDR_ENABLE_VERBOSE_LOG    1
// To enable internal very verbose logging, uncomment the above line.
// see also cmdr_verbose_debug(...) in cmdr_log.hh

// ## version info

#include "cmdr-version.hh"

// ## all headers ...

#include "cmdr_defs.hh"
#include "cmdr_types.hh"
#include "cmdr_types_check.hh"

#include "cmdr_chrono.hh"
#include "cmdr_common.hh"

#include "cmdr_dbg.hh"


#include "cmdr_log.hh"

#include "cmdr_ios.hh"
#include "cmdr_os_io_redirect.hh"
#include "cmdr_path.hh"
#include "cmdr_string.hh"
#include "cmdr_terminal.hh"
#include "cmdr_utils.hh"

#include "cmdr_mmap.hh"
#include "cmdr_pipeable.hh"
#include "cmdr_pool.hh"
#include "cmdr_priority_queue.hh"
#include "cmdr_process.hh"
#include "cmdr_small_string.hh"


//


#if defined(CMDR_CXX11_UNIT_TEST) && CMDR_CXX11_UNIT_TEST == 1
#include "cmdr_x_test.hh"
#endif


//

#include "cmdr_var_t.hh"

#include "cmdr_cmn.hh"

#include "cmdr_arg.hh"
#include "cmdr_cmd.hh"

#include "cmdr_app.hh"

#include "cmdr_internals.hh"
#include "cmdr_public.hh"

#include "cmdr_opts.hh"

#include "cmdr_app_inl.h"
#include "cmdr_cmd_inl.h"
#include "cmdr_imp1.hh" // vars / variable / parse
#include "cmdr_impl.hh"


#endif //CMDR_CXX11_CMDR11_HH
