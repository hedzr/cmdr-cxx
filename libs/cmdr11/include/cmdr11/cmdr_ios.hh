//
// Created by Hedzr Yeh on 2021/1/21.
//

#ifndef CMDR_CXX11_CMDR_IOS_HH
#define CMDR_CXX11_CMDR_IOS_HH

#include <iomanip>
#include <iosfwd>

namespace cmdr::io {

    class ios_flags_saver {
    public:
        explicit ios_flags_saver(std::ostream &os)
            : ios(os)
            , f(os.flags()) {}
        ~ios_flags_saver() { ios.flags(f); }

        ios_flags_saver(const ios_flags_saver &rhs) = delete;
        ios_flags_saver &operator=(const ios_flags_saver &rhs) = delete;

    private:
        std::ostream &ios;
        std::ios::fmtflags f;
    };

    class ios_state_saver {
    public:
        explicit ios_state_saver(std::ostream &os)
            : ios(os) {
            oldState.copyfmt(os);
        }
        ~ios_state_saver() { ios.copyfmt(oldState); }

        ios_state_saver(const ios_state_saver &rhs) = delete;
        ios_state_saver &operator=(const ios_state_saver &rhs) = delete;

    private:
        std::ostream &ios;
        std::ios oldState{nullptr};
    };

} // namespace cmdr::io

#endif //CMDR_CXX11_CMDR_IOS_HH
