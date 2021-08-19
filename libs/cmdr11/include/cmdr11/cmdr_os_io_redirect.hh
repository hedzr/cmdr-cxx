//
// Created by Hedzr Yeh on 2021/4/20.
//

#ifndef CMDR_CXX_CMDR_OS_IO_REDIRECT_HH
#define CMDR_CXX_CMDR_OS_IO_REDIRECT_HH

// #include "cmdr_process.hh"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <istream>

#include <memory>
#include <stdexcept>
#include <streambuf>
#include <string>


namespace cmdr::os {

    class pipe_stdin_to {
        std::streambuf *stream_buffer_cout;
        std::streambuf *stream_buffer_cin;

    public:
        pipe_stdin_to(std::streambuf *stream_buffer_file = nullptr) {
            stream_buffer_cout = std::cout.rdbuf();
            stream_buffer_cin = std::cin.rdbuf();
            if (stream_buffer_file)
                std::cin.rdbuf(stream_buffer_file);
        }
        virtual ~pipe_stdin_to() {
            std::cin.rdbuf(stream_buffer_cin);
            std::cout.rdbuf(stream_buffer_cout);
            // std::cout << "This line is written to screen" << std::endl;
        }
    };

    template<std::ostream *std_out_device = &std::cout>
    class pipe_std_dev_to {
        std::streambuf *stream_buffer_;

    public:
        pipe_std_dev_to(std::streambuf *stream_buffer_file = nullptr) {
            stream_buffer_ = (*std_out_device).rdbuf();
            if (stream_buffer_file)
                (*std_out_device).rdbuf(stream_buffer_file);
        }
        virtual ~pipe_std_dev_to() {
            (*std_out_device).rdbuf(stream_buffer_);
        }
    };


    typedef pipe_std_dev_to<&std::cout> pipe_stdout_to;
    typedef pipe_std_dev_to<&std::cerr> pipe_stderr_to;
    typedef pipe_std_dev_to<&std::clog> pipe_stdlog_to;


    class pipe_all_to : pipe_std_dev_to<&std::cout>
                        , pipe_std_dev_to<&std::cerr>
                        , pipe_std_dev_to<&std::clog>
                        , pipe_stdin_to {
    public:
        pipe_all_to(std::streambuf *output_stream_buffer_file = nullptr, std::streambuf *input_stream_buffer_file = nullptr)
                : pipe_std_dev_to<&std::cout>(output_stream_buffer_file)
                  , pipe_std_dev_to<&std::cerr>(output_stream_buffer_file)
                  , pipe_std_dev_to<&std::clog>(output_stream_buffer_file)
                  , pipe_stdin_to(input_stream_buffer_file) {
        }
        virtual ~pipe_all_to() {}
    };

} // namespace cmdr::os

#endif //CMDR_CXX_CMDR_OS_IO_REDIRECT_HH
