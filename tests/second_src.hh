//
// Created by Hedzr Yeh on 2021/2/10.
//

#ifndef CMDR_CXX11_SECOND_SRC_HH
#define CMDR_CXX11_SECOND_SRC_HH

void second_test();

void fatal_exit(const std::string &msg);

void add_sub1_menu(cmdr::app &cli, cmdr::opt::cmd &t1, const_chars title);
void add_test_menu(cmdr::app &cli);
void add_main_menu(cmdr::app &cli);
void add_server_menu(cmdr::app &cli);


#endif // CMDR_CXX11_SECOND_SRC_HH
