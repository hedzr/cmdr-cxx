//
// Created by Hedzr Yeh on 2021/4/20.
//

#include "cmdr11/cmdr_ios.hh" // allow ostream << istream, ...
#include "cmdr11/cmdr_os_io_redirect.hh"
#include "cmdr11/cmdr_path.hh"
#include "cmdr11/cmdr_process.hh"

#include <fstream>
#include <iostream>

void test_stdout_capture() {
  cmdr::process::exec ex("ls -la /tmp/");

  std::cout << "stdout:" << '\n';
  std::cout << ex;
  std::cout.flush();
  // std::cout.good();

  std::cout << '\n';
  std::cout << "done!" << '\n';
  std::cout << '\n';
  std::cout << '\n';
  std::cout.flush();
}

void test_stderr_capture() {
  {
    cmdr::process::exec ex("ls -la /unknown");

    std::cout << "executed: rec-code = " << ex.ret_code() << '\n';

    std::cout << "stderr:" << '\n';
    // ex.stderr_stream() >> std::noskipws;
    // std::copy(std::istream_iterator<char>(ex.stderr_stream()), std::istream_iterator<char>(), std::ostream_iterator<char>(std::cout));
    std::cout << ex.stderr_stream();

    std::cout << "stdout:" << '\n';
    std::cout << ex;
    std::cout.flush();
  }
  std::cout << '\n';
  std::cout << "done!" << '\n';
  std::cout.flush();
  std::cout << '\n';
  std::cout << '\n';
}

int main() {

  std::fstream file;
  file.open("cout.txt.log", std::ios::out);

  std::cout << "This line written to screen, pwd: ";
  std::cout << cmdr::path::get_current_dir();
  std::cout << "\n";
  {
    cmdr::os::pipe_all_to p1(file.rdbuf());
    std::cout << "This line written to file "
              << "\n";
    std::clog << "hello"
              << "\n";
  }
  std::cout << "This line written to screen too"
            << "\n";
  std::clog << "world"
            << "\n";
  test_stdout_capture();
  test_stderr_capture();
}