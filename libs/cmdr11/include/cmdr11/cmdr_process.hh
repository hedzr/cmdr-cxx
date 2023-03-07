//
// Created by Hedzr Yeh on 2021/4/20.
//

#ifndef CMDR_CXX_CMDR_PROCESS_HH
#define CMDR_CXX_CMDR_PROCESS_HH

#include <cstdio>
#include <cstring>
#include <fstream>
#include <istream>
#include <memory>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <thread>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#endif
#include <array>

#include "cmdr_defs.hh"
#include "cmdr_path.hh"


namespace cmdr::process {

  namespace detail {

    namespace fs = std::filesystem;

    class execbuf : public std::streambuf {
    protected:
      std::string output;
      std::string tmpfile_stderr;

      int_type underflow() override {
        if (gptr() < egptr()) return traits_type::to_int_type(*gptr());
        return traits_type::eof();
        // return std::streambuf::underflow();
      }
      // int_type underflow(int_type character) {
      //     if (gptr() < egptr()) return traits_type::to_int_type(*gptr());
      //     return traits_type::eof();
      // }

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    private:
      int system_and_capture(
          std::string cmdline,     // Command Line
          std::string workdir,     // set to '.' for current directory
          std::string &lst_stdout, // Return List of StdOut
          std::string &lst_stderr, // Return List of StdErr
          uint32_t &ret_code)      // Return Exit Code
      {
        int success;
        SECURITY_ATTRIBUTES security_attributes;
        HANDLE stdout_rd = INVALID_HANDLE_VALUE;
        HANDLE stdout_wr = INVALID_HANDLE_VALUE;
        HANDLE stderr_rd = INVALID_HANDLE_VALUE;
        HANDLE stderr_wr = INVALID_HANDLE_VALUE;
        PROCESS_INFORMATION process_info;
        STARTUPINFO startup_info;
        std::thread stdout_thread;
        std::thread stderr_thread;

        security_attributes.nLength              = sizeof(SECURITY_ATTRIBUTES);
        security_attributes.bInheritHandle       = TRUE;
        security_attributes.lpSecurityDescriptor = nullptr;

        if (!CreatePipe(&stdout_rd, &stdout_wr, &security_attributes, 0) ||
            !SetHandleInformation(stdout_rd, HANDLE_FLAG_INHERIT, 0)) {
          return -1;
        }

        if (!CreatePipe(&stderr_rd, &stderr_wr, &security_attributes, 0) ||
            !SetHandleInformation(stderr_rd, HANDLE_FLAG_INHERIT, 0)) {
          if (stdout_rd != INVALID_HANDLE_VALUE) CloseHandle(stdout_rd);
          if (stdout_wr != INVALID_HANDLE_VALUE) CloseHandle(stdout_wr);
          return -2;
        }

        ZeroMemory(&process_info, sizeof(PROCESS_INFORMATION));
        ZeroMemory(&startup_info, sizeof(STARTUPINFO));

        startup_info.cb         = sizeof(STARTUPINFO);
        startup_info.hStdInput  = 0;
        startup_info.hStdOutput = stdout_wr;
        startup_info.hStdError  = stderr_wr;

        if (stdout_rd || stderr_rd)
          startup_info.dwFlags |= STARTF_USESTDHANDLES;

        // Make a copy because CreateProcess needs to modify string buffer
        char cmdline_full[MAX_PATH];
        strncpy(cmdline_full, cmdline.c_str(), MAX_PATH);
        cmdline_full[MAX_PATH - 1] = 0;

        success                    = CreateProcess(
                               nullptr,
                               cmdline_full,
                               nullptr,
                               nullptr,
                               TRUE,
                               CREATE_NO_WINDOW,
                               nullptr,
                               workdir.c_str(),
                               &startup_info,
                               &process_info);
        CloseHandle(stdout_wr);
        CloseHandle(stderr_wr);

        if (!success) {
          CloseHandle(process_info.hProcess);
          CloseHandle(process_info.hThread);
          CloseHandle(stdout_rd);
          CloseHandle(stderr_rd);
          return -4;
        } else {
          CloseHandle(process_info.hThread);
        }

        if (stdout_rd) {
          stdout_thread = std::thread([&]() {
            DWORD n;
            const size_t bufsize = 1000;
            char buffer[bufsize];
            for (;;) {
              n           = 0;
              int success = ReadFile(
                  stdout_rd,
                  buffer,
                  (DWORD) bufsize,
                  &n,
                  nullptr);
              // cmdr_verbose_debug("STDERR: Success:%d n:%d\n", success, (int) n);
              if (!success || n == 0)
                break;
              std::string s(buffer, n);
              // cmdr_verbose_debug("STDOUT:(%s)\n", s.c_str());
              lst_stdout += s;
            }
            // cmdr_verbose_debug("STDOUT:BREAK!\n");
          });
        }

        if (stderr_rd) {
          stderr_thread = std::thread([&]() {
            DWORD n;
            const size_t bufsize = 1000;
            char buffer[bufsize];
            for (;;) {
              n           = 0;
              int success = ReadFile(
                  stderr_rd,
                  buffer,
                  (DWORD) bufsize,
                  &n,
                  nullptr);
              // cmdr_verbose_debug("STDERR: Success:%d n:%d\n", success, (int) n);
              if (!success || n == 0)
                break;
              std::string s(buffer, n);
              // cmdr_verbose_debug("STDERR:(%s)\n", s.c_str());
              lst_stderr += s;
            }
            // cmdr_verbose_debug("STDERR:BREAK!\n");
          });
        }

        WaitForSingleObject(process_info.hProcess, INFINITE);
        if (!GetExitCodeProcess(process_info.hProcess, (DWORD *) &ret_code))
          ret_code = (uint32_t) -1;

        CloseHandle(process_info.hProcess);

        if (stdout_thread.joinable())
          stdout_thread.join();

        if (stderr_thread.joinable())
          stderr_thread.join();

        CloseHandle(stdout_rd);
        CloseHandle(stderr_rd);

        return 0;
      }
#endif

    public:
      execbuf(const char *command) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        // _rc = std::system(command); // execute the UNIX command "ls -l >test.txt"
        //                            //std::cout << std::ifstream("test.txt").rdbuf();
        //                            // std::cout << "Exit code: " << WEXITSTATUS(_rc) << '\n';
        std::string str_out, str_err;
        uint32_t rc;
        _rc = system_and_capture(command, ".", str_out, str_err, rc);
        if (_rc == 0)
          _rc = (int) rc;

        this->output += str_out.data();
        this->output += str_err.data();
        setg((char *) this->output.data(), (char *) this->output.data(), (char *) (this->output.data() + this->output.size()));

#else // try POSIX
        tmpfile_stderr = path::tmpname_for_stderr();
        std::array<char, 512> cmd;
        std::snprintf(cmd.data(), 512, "%s 2>%s", command, tmpfile_stderr.c_str());
        // std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);
        std::unique_ptr<FILE, std::function<void(FILE *)>> pipe(popen(cmd.data(), "r"), [this](FILE *f) {
          _rc = pclose(f);
          if (_rc == EXIT_SUCCESS) {        // == 0
          } else if (_rc == EXIT_FAILURE) { // EXIT_FAILURE is not used by all programs, maybe needs some adaptation.
          }
        });
        if (!pipe) {
          throw std::runtime_error("popen() failed!");
        }

        // std::array<char, 128> buffer;
        int ch;
        while ((ch = fgetc(pipe.get())) != EOF) {
          this->output += (char) ch;
        }
        // while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        //     this->output += buffer.data();
        // }
        // pipe.reset();
        setg((char *) this->output.data(), (char *) this->output.data(), (char *) (this->output.data() + this->output.size()));

        _ef = std::ifstream(tmpfile_stderr);

        // auto rc = pclose(pipe);
#endif
      }

      std::streamsize size() const { return output.length(); }

      int _rc;
      std::ifstream _ef;
    };
  } // namespace detail

  /**
   * @brief execute a shell command and capture the stdout, stderr and return code.
   * @details For example:
   * @code{c++}
   *   cmdr::process::exec dot("dot aa.dot -T png -o aa.png -v");
   *   std::cout &lt;&lt; dot;                     // for stdout
   *   std::cout &lt;&lt; ex.stderr_stream();      // for stderr & stdlog
   *   std::cout &lt;&lt; "executed: rec-code = " &lt;&lt; ex.ret_code() &lt;&lt; '\n';
   * @endcode
   * NOTE that it's not fully completed in Windows, more testing and coding needed.
   */
  class exec : public std::istream {
  protected:
    detail::execbuf buffer;

  public:
    exec(char const *command)
        : std::istream(nullptr)
        , buffer(command) {
      this->rdbuf(&buffer);
    }

    int ret_code() const { return buffer._rc / 256; }
    std::ifstream &stderr_stream() { return buffer._ef; }

    friend std::ostream &operator<<(std::ostream &os, exec const &o) {
      if (o.buffer.size())
        os << o.rdbuf();
      return os;
    }
    // inline std::streambuf* rdbuf() const{return &buffer;}
  };


  inline void wait_a_key(const char *headline = nullptr) {
    if (headline) std::cout << headline;
    else
      std::cout << "Press Enter to Continue...";
    std::cin.ignore();
  }

} // namespace cmdr::process

#endif // CMDR_CXX_CMDR_PROCESS_HH
