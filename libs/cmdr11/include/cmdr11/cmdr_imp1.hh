//
// Created by Hedzr Yeh on 2021/1/29.
//

#ifndef CMDR_CXX11_CMDR_IMP1_HH
#define CMDR_CXX11_CMDR_IMP1_HH

namespace cmdr::vars {
    
    inline variable::self_type variable::parse(std::string &s) {
        self_type v;
        std::istringstream is(s);
        is >> v;
        return v;
    }
    
}

#endif //CMDR_CXX11_CMDR_IMP1_HH
