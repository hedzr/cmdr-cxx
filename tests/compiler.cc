
// #include "cmdr11/cmdr-all.hh"

#include "cmdr11/cmdr_common.hh"
#include "cmdr11/cmdr_string.hh" // to_string<>()

#include <iomanip>
#include <iostream>

int main() {
  std::cout << "Hello, World!" << '\n'
            << "I was built by " << cmdr::cross::compiler_name() << '\n'
            << "__cplusplus = 0x" << std::hex << std::setfill('0') << std::setw(8) << __cplusplus << ' ' << '(' << std::dec << __cplusplus << ')' << '\n';
}