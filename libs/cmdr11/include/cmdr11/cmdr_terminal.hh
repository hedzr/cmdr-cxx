//
// Created by Hedzr Yeh on 2021/1/22.
//

#ifndef CMDR_CXX11_CMDR_TERMINAL_HH
#define CMDR_CXX11_CMDR_TERMINAL_HH

namespace cmdr::terminal::colors {

    class colorize final {
        // public:
        //     static colorize &instance();
        //
        //     colorize(const colorize &) = delete;
        //     colorize &operator=(const colorize) = delete;
        //
        // protected:
    public:
        colorize()
            : _fg{}
            , _bg{}
            , _st{} {}

    protected:
        int _fg, _bg;

        union _status {
            struct {
                bool _bold : 1;      // 1:
                bool _dim : 1;       // 2:
                bool _italic : 1;    // 3:
                bool _underline : 1; // 4:
                bool _blink : 1;     // 5:
                bool _rblink : 1;    // 6:
                bool _reverse : 1;   // 7:
                bool _hidden : 1;    // 8: conceal
                bool _crossed : 1;   // 9:

                bool _reset_bold : 1;
                bool _reset_dim : 1;
                bool _reset_italic : 1;
                bool _reset_underline : 1;
                bool _reset_blink : 1;
                bool _reset_rblink : 1;
                bool _reset_reverse : 1;
                bool _reset_hidden : 1;
                bool _reset_crossed : 1;
                bool _reset_all : 1;

                bool _auto_reset_invoked : 1;
            } _individual;
            uint _i{};
        } _st;

        bool _auto_reset{true};
        const_chars _cc{};
        std::string _ss{};

    public:
        colorize &fg(int c) {
            assert(c >= 0 && c <= 256);
            _fg = c;
            return (*this);
        }
        colorize &bg(int c) {
            assert(c >= 0 && c <= 256);
            _bg = c;
            return (*this);
        }

#define DEF_MODIFIER(what)                  \
    colorize &what(bool b = true) {         \
        _st._individual._##what = b;        \
        return (*this);                     \
    }                                       \
    colorize &reset_##what(bool b = true) { \
        _st._individual._reset_##what = b;  \
        _st._individual._##what = false;    \
        return (*this);                     \
    }
        DEF_MODIFIER(bold)
        DEF_MODIFIER(dim)
        DEF_MODIFIER(italic)
        DEF_MODIFIER(underline)
        DEF_MODIFIER(blink)
        DEF_MODIFIER(rblink)
        DEF_MODIFIER(reverse)
        DEF_MODIFIER(hidden)
        DEF_MODIFIER(crossed)
#undef DEF_MODIFIER

        colorize &reset_all(bool b = true) {
            _st._individual._reset_all = b;
            return (*this);
        }

        [[nodiscard]] std::string modifiers() const {
            std::stringstream ss;
            if (_st._individual._bold) ss << "1;";
            if (_st._individual._dim) ss << "2;";
            if (_st._individual._italic) ss << "3;";
            if (_st._individual._underline) ss << "4;";
            if (_st._individual._blink) ss << "5;";
            if (_st._individual._rblink) ss << "6;";
            if (_st._individual._reverse) ss << "7;";
            if (_st._individual._hidden) ss << "8;";
            if (_st._individual._crossed) ss << "9;";
            if (_st._individual._reset_bold) ss << "21;";
            if (_st._individual._reset_dim) ss << "22;";
            if (_st._individual._reset_italic) ss << "23;";
            if (_st._individual._reset_underline) ss << "24;";
            if (_st._individual._reset_blink) ss << "25;";
            if (_st._individual._reset_rblink) ss << "26;";
            if (_st._individual._reset_reverse) ss << "27;";
            if (_st._individual._reset_hidden) ss << "28;";
            if (_st._individual._reset_crossed) ss << "29;";
            if (_st._individual._reset_all) ss << "0;";
            return ss.str();
        }

        colorize &auto_reset(bool b = true) {
            _auto_reset = b;
            return (*this);
        }

        colorize &s(const_chars str) {
            _cc = str;
            return (*this);
        }
        colorize &s(const std::string &str) {
            _ss = str;
            return (*this);
        }

        // 256-colors, 88-colors

        enum Colors256 {
            Black,
            Maroon,
            Green,
            Olive,
            Navy,
            Purple,
            Teal,
            Sliver,

            Grey,
            Red,
            Lime,
            Yellow,
            Blue,
            Fuchsia,
            Aqua,
            White,

            Grey0,
            NavyBlue,
            DarkBlue,
            Blue3,
            Blue2,
            Blue1,
            DarkGreen,
            DeepSkyBlue4_23,

            DeepSkyBlue4_24,
            DeepSkyBlue4_25,
            DodgerBlue3,
            DodgerBlue2,
            Green4,
            SpringGreen4,
            Turquoise4,
            DeepSkyBlue3_31,

            DeepSkyBlue3_32,
            DodgerBlue1,
            Green3,
            SpringGreen3,
            DarkCyan,
            LightSeaGreen,
            DeepSkyBlue2,
            DeepSkyBlue1,

            Green3_40,
            SpringGreen3_41,
            SpringGreen2,
            Cyan3,
            DarkTurquoise,
            Turquoise2,
            Green1,
            SpringGreen2_47,

            SpringGreen1,
            MediumSpringGreen,
            Cyan2,
            Cyan1,
            DarkRed,
            DeepPink4,
            Purple4,
            Purple4_55,

            Purple3 = 56,
            BlueViolet,
            Orange4,
            Grey37,
            MediumPurple4,
            SlateBlue3,
            SlateBlue3_62,
            RoyalBlue1,

            Chartreuse4 = 64,
            DarkSeaGreen4,
            PaleTurquoise4,
            SteelBlue,
            SteelBlue3,
            CornflowerBlue,
            Chartreuse3,
            DarkSeaGreen4_71,

            CadetBlue = 72,
            CadetBlue_73,
            SkyBlue3,
            SteelBlue1,
            Chartreuse3_76,
            PaleGreen3,
            SeaGreen3,
            Aquamarine3,

            MediumTurquoise = 80,
            SteelBlue1_81,
            Chartreuse2_82,
            SeaGreen2,
            SeaGreen1,
            SeaGreen1_85,
            Aquamarine1,
            DarkSlateGray2,
            DarkRed_88 = 88,
            DeepPink4_89,
            DarkMagenta,
            DarkMagenta_91,
            DarkViolet,
            Purple_93,
            Orange4_94,
            LightPink4,

            Plum4 = 96,
            MediumPurple3,
            MediumPurple3_98,
            SlateBlue1,
            Yellow4,
            Wheat4,
            Grey53,
            LightSlateGrey,

            MediumPurple = 104,
            LightSlateBlue,
            Yellow4_106,
            DarkOliveGreen3,
            DarkSeaGreen,
            LightSkyBlue3,
            LightSkyBlue3_110,
            SkyBlue2,

            Chartreuse2 = 112,
            DarkOliveGreen3_113,
            PaleGreen3_114,
            DarkSeaGreen3,
            DarkSlateGray3,
            SkyBlue1,
            Chartreuse1,
            LightGreen_119,

            LightGreen = 120,
            PaleGreen1,
            Aquamarine1_122,
            DarkSlateGray1,
            Red3,
            DeepPink4_125,
            MediumVioletRed,
            Magenta3,

            DarkViolet_128 = 128,
            Purple_129,
            DarkOrange3,
            IndianRed,
            HotPink3,
            MediumOrchid3,
            MediumOrchid,
            MediumPurple2,

            DarkGoldenrod = 136,
            LightSalmon3,
            RosyBrown,
            Grey63,
            MediumPurple2_140,
            MediumPurple1,
            Gold3,
            DarkKhaki,

            NavajoWhite3 = 144,
            Grey69,
            LightSteelBlue3,
            LightSteelBlue,
            Yellow3,
            DarkOliveGreen3_149,
            DarkSeaGreen3_150,
            DarkSeaGreen2,

            LightCyan3 = 152,
            LightSkyBlue1,
            GreenYellow,
            DarkOliveGreen2,
            PaleGreen1_156,
            DarkSeaGreen2_157,
            DarkSeaGreen1,
            PaleTurquoise1,

            Red3_160 = 160,
            DeepPink3,
            DeepPink3_162,
            Magenta3_163,
            Magenta3_164,
            Magenta2,
            DarkOrange3_166,
            IndianRed_167,

            HotPink3_168 = 168,
            HotPink2,
            Orchid,
            MediumOrchid1,
            Orange3,
            LightSalmon3_173,
            LightPink3,
            Pink3,

            Plum3 = 176,
            Violet,
            Gold3_178,
            LightGoldenrod3,
            Tan,
            MistyRose3,
            Thistle3,
            Plum2,

            Yellow3_184 = 184,
            Khaki3,
            LightGoldenrod2,
            LightYellow3,
            Grey84,
            LightSteelBlue1,
            Yellow2,
            DarkOliveGreen1_191,

            DarkOliveGreen1 = 192,
            DarkSeaGreen1_193,
            Honeydew2,
            LightCyan1,
            Red1,
            DeepPink2,
            DeepPink1,
            DeepPink1_198,
            DeepPink1_199,

            Magenta2_200 = 200,
            Magenta1,
            OrangeRed1,
            IndianRed1,
            IndianRed1_204,
            HotPink,
            HotPink_206,
            MediumOrchid1_207,

            DarkOrange = 208,
            Salmon1,
            LightCoral,
            PaleVioletRed1,
            Orchid2,
            Orchid1,
            Orange1,
            SandyBrown,

            LightSalmon1 = 216,
            LightPink1,
            Pink1,
            Plum1,
            Gold1,
            LightGoldenrod2_221,
            LightGoldenrod2_222,
            NavajoWhite1,

            MistyRose1 = 224,
            Thistle1,
            Yellow1,
            LightGoldenrod1,
            Khaki1,
            Wheat1,
            Cornsilk1,
            Grey100,

            Grey3 = 232,
            Grey7,
            Grey11,
            Grey15,
            Grey19,
            Grey23,
            Grey27,
            Grey30,

            Grey35 = 240,
            Grey39,
            Grey42,
            Grey46,
            Grey50,
            Grey54,
            Grey58,
            Grey62,

            Grey66 = 248,
            Grey70,
            Grey74,
            Grey78,
            Grey82,
            Grey85,
            Grey89,
            Grey93,

            Default = 256,
        };

        // 16-colors

        enum class style {
            reset = 0,
            bold = 1,
            dim = 2,
            italic = 3,
            underline = 4,
            blink = 5,
            rblink = 6,
            reversed = 7,
            conceal = 8,
            crossed = 9
        };

        enum class fg {
            black = 30,
            red = 31,
            green = 32,
            yellow = 33,
            blue = 34,
            magenta = 35,
            cyan = 36,
            gray = 37,
            reset = 39
        };

        enum class bg {
            black = 40,
            red = 41,
            green = 42,
            yellow = 43,
            blue = 44,
            magenta = 45,
            cyan = 46,
            gray = 47,
            reset = 49
        };

        // light fg
        enum class fgB {
            black = 90,
            red = 91,
            green = 92,
            yellow = 93,
            blue = 94,
            magenta = 95,
            cyan = 96,
            gray = 97, // = white
        };

        // light bg
        enum class bgB {
            black = 100,
            red = 101,
            green = 102,
            yellow = 103,
            blue = 104,
            magenta = 105,
            cyan = 106,
            gray = 107, // = white
        };

        enum class reset {
            all = 0,
            bold = 1,
            dim = 2,
            italic = 3,
            underline = 4,
            blink = 5,
            rblink = 6,
            reversed = 7,
            conceal = 8,
            crossed = 9
        };

        friend std::ostream &operator<<(std::ostream &os, const enum style o) {
            os << "\033[" << (int) o << "m";
            return os;
        }
        friend std::ostream &operator<<(std::ostream &os, const enum fg o) {
            os << "\033[" << (int) o << "m";
            return os;
        }
        friend std::ostream &operator<<(std::ostream &os, const enum bg o) {
            os << "\033[" << (int) o << "m";
            return os;
        }
        friend std::ostream &operator<<(std::ostream &os, const enum fgB o) {
            os << "\033[" << (int) o << "m";
            return os;
        }
        friend std::ostream &operator<<(std::ostream &os, const enum bgB o) {
            os << "\033[" << (int) o << "m";
            return os;
        }
        friend std::ostream &operator<<(std::ostream &os, const enum reset o) {
            os << "\033[" << (int) o << ";m";
            return os;
        }

        static colorize create() {
            return colorize{};
        }
        static colorize new_colorizer() {
            return colorize{};
        }

        friend std::ostream &operator<<(std::ostream &os, const colorize &o) {
            os << "\033[" << o.modifiers();
            if (o._fg != 256) {
                os << "38;5;" << o._fg;
            }
            if (o._bg != 256) {
                os << "48;5;" << o._bg;
            }
            os << 'm';
            if (o._cc != nullptr)
                os << o._cc; // << " // " << o.modifiers();
            if (!o._ss.empty())
                os << o._ss;
            if (o._auto_reset) {
                os << "\033[0m";
                // o._st._auto_reset_invoked = true;
            }
            return os;
        }
    };

    // inline colorize &colorize::instance() {
    //     static const std::unique_ptr<colorize> instance{new colorize{}};
    //     return *instance;
    // }


} // namespace cmdr::terminal::colors

namespace cmdr::terminal {
    //
} //namespace cmdr::terminal

#endif //CMDR_CXX11_CMDR_TERMINAL_HH
