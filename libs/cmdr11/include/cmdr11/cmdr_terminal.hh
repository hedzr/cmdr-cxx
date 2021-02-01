//
// Created by Hedzr Yeh on 2021/1/22.
//

#ifndef CMDR_CXX11_CMDR_TERMINAL_HH
#define CMDR_CXX11_CMDR_TERMINAL_HH


#include <unistd.h>

#include <cstdlib>
// #include <stdlib.h>


namespace cmdr::terminal {

    struct terminfo {
        //
        //
        // see:
        // https://pubs.opengroup.org/onlinepubs/009695399/functions/isatty.html
        static bool isatty() { return ::isatty(0); }

        static const char *term() {
            auto str = std::getenv("TERM");
            if (!str)
                return "";
            return str;
        }

        static bool is_colorful() {
#if defined(OS_WIN)
            return true;
#else
            auto str = term();
            const char *Terms[] = {"ansi", "color", "console", "cygwin", "gnome", "konsole",
                                   "kterm", "linux", "msys", "putty", "rxvt", "screen",
                                   "vt100", "xterm", "xterm-256colors", "xterm-color"};
            return std::any_of(std::begin(Terms), std::end(Terms),
                               [&](const char *term) {
                                   return std::strstr(str, term) != nullptr;
                               });
#endif
        }

        static int support_colors() {
#if defined(OS_WIN)
            return 256;
#else
            auto str = std::getenv("COLORTERM");
            // const char *str = ::getenv("COLORTERM");

            if (!str) {
                // in embedded terminal or under clion debugging env, we assume it's a 256-colors terminal/pty
                return 256;
            }

            std::string s(str);

            if (s == "truecolor" || s == "24bit") {
                // std::cout << "COLORTERM = " << s << '\n';
                return 0x1000000;
            }

            // int count = std::stoi(str);

            std::istringstream is(s);
            int count = 8;
            is >> count;
            return count;
#endif
        }
    };

} // namespace cmdr::terminal

namespace cmdr::terminal::colors {

    /**
     * @brief colorize
     */
    class colorize final {
        // public:
        //     static colorize &instance();
        //
        //     colorize(const colorize &) = delete;
        //     colorize &operator=(const colorize) = delete;
        //
        // protected:
    public:
        explicit colorize(bool true_color_enabled = true)
            : _fg{}
            , _bg{}
            , _st{}
            , _true_color_enabled(true_color_enabled) {
            _init();
            if (_colors <= 256) _true_color_enabled = false;
            if (_true_color_enabled) _fg = _bg = -1;
        }

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
                bool _crossed : 1;   // 9: mac - strikeout

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
        bool _true_color_enabled{true}; // false to fallback to 256-color, or true to enable true-color mode.
        const_chars _cc{};
        std::string _ss{};
        std::stringstream _stocked_ss{};

        static bool _colorful;
        static int _colors;
        static void _init() {
            if (!_colorful) {
                _colorful = terminfo::is_colorful();
                _colors = terminfo::support_colors();
#if defined(_DEBUG)
                if (!_colorful) {
                    _colorful = true;
                    _colors = 256;
#endif
                }
            }
        }

        [[nodiscard]] std::string modifiers() const {
#if 0
            std::stringstream ss;
            if (_st._individual._reset_all) ss << "0;";
            if (_st._individual._reset_bold) ss << "21;";
            if (_st._individual._reset_dim) ss << "22;";
            if (_st._individual._reset_italic) ss << "23;";
            if (_st._individual._reset_underline) ss << "24;";
            if (_st._individual._reset_blink) ss << "25;";
            if (_st._individual._reset_rblink) ss << "26;";
            if (_st._individual._reset_reverse) ss << "27;";
            if (_st._individual._reset_hidden) ss << "28;";
            if (_st._individual._reset_crossed) ss << "29;";
            if (_st._individual._bold) ss << "1;";
            if (_st._individual._dim) ss << "2;";
            if (_st._individual._italic) ss << "3;";
            if (_st._individual._underline) ss << "4;";
            if (_st._individual._blink) ss << "5;";
            if (_st._individual._rblink) ss << "6;";
            if (_st._individual._reverse) ss << "7;";
            if (_st._individual._hidden) ss << "8;";
            if (_st._individual._crossed) ss << "9;";
            return ss.str();
#else
        return _stocked_ss.str();
#endif
        }

    public:
        static void debug_print() {
            std::cout << "How many colors: " << _colors << ", " << _colorful << '\n';
        }

    public:
#define DEF_MODIFIER(what, i)                   \
    colorize &what(bool b = true) {             \
        /*_st._individual._##what = b; */       \
        _stocked_ss << (b ? i : 20 + i) << ';'; \
        return (*this);                         \
    }                                           \
    colorize &reset_##what() {                  \
        /*_st._individual._reset_##what = b;*/  \
        /*_st._individual._##what = false;*/    \
        _stocked_ss << (20 + i) << ';';         \
        return (*this);                         \
    }
        DEF_MODIFIER(bold, 1)
        DEF_MODIFIER(dim, 2)
        DEF_MODIFIER(italic, 3)
        DEF_MODIFIER(underline, 4)
        DEF_MODIFIER(blink, 5)
        DEF_MODIFIER(rblink, 6)
        DEF_MODIFIER(reverse, 7)
        DEF_MODIFIER(hidden, 8)
        DEF_MODIFIER(crossed, 9)
#undef DEF_MODIFIER

        colorize &reset_all(/*bool b = true*/) {
            // _st._individual._reset_all = b;
            std::stringstream s;
            s << "0;";
            _stocked_ss.swap(s);
            _fg = _bg = -1;
            return (*this);
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

    public:
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

        colorize &fg(int r, int g, int b) {
            // assert(c >= 0 && c <= 256);
            _fg = (r << 16) | (g << 8) | b;
            return (*this);
        }
        colorize &fg(Colors256 color256) {
            // assert(color256 >= 0 && color256 <= 256);
            _fg = color256;
            return (*this);
        }
        colorize &bg(int r, int g, int b) {
            // assert(c >= 0 && c <= 256);
            _bg = (r << 16) | (g << 8) | b;
            return (*this);
        }
        colorize &bg(Colors256 color256) {
            // assert(color256 >= 0 && color256 <= 256);
            _bg = color256;
            return (*this);
        }

    public:
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
            if (_colorful) os << "\033[" << (int) o << "m";
            return os;
        }
        friend std::ostream &operator<<(std::ostream &os, const enum fg o) {
            if (_colorful) os << "\033[" << (int) o << "m";
            return os;
        }
        friend std::ostream &operator<<(std::ostream &os, const enum bg o) {
            if (_colorful) os << "\033[" << (int) o << "m";
            return os;
        }
        friend std::ostream &operator<<(std::ostream &os, const enum fgB o) {
            if (_colorful) os << "\033[" << (int) o << "m";
            return os;
        }
        friend std::ostream &operator<<(std::ostream &os, const enum bgB o) {
            if (_colorful) os << "\033[" << (int) o << "m";
            return os;
        }
        friend std::ostream &operator<<(std::ostream &os, const enum reset o) {
            if (_colorful) os << "\033[" << (int) o << ";m";
            return os;
        }


    public:
        // normal mode: 256-colors or true-colors.
        // In normal mode, the instance of colorize need to be create at first. for example:
        //
        //    using namespace cmdr::terminal::colors;
        //    auto c = colorize::create();
        //    std::cout << c.fg(c.Purple3).bg(c.Default).underline().bold().s("some text") << '\n';
        //
        //  In 16-colors mode, you could print out the colored text directly, but you have to reset fg and bg yourself.
        //  The sample is:
        //
        //    std::cout << colorize::style::underline << colorize::fg::red << "Hello, Colorful World!" << std::endl;
        //    std::cout << colorize::reset::all << "Here I'm!" << std::endl;
        //    std::cout << "END.\n\n";
        //
        //  The two modes above can be used at the same time.
        //
        static colorize create() {
            return colorize{};
        }
        static colorize new_colorizer() {
            return colorize{};
        }

        /**
         * @brief enable colored text outputting or not.
         * @param enable_colors
         */
        [[maybe_unused]] static void enable(bool enable_colors = true) {
            _colorful = enable_colors;
            if (enable_colors && _colors == 0) _colors = 256;
        }

#define __R(x) (((x) >> 16) & 0xff)
#define __G(x) (((x) >> 8) & 0xff)
#define __B(x) (((x)) & 0xff)
        std::string as_string() {
            std::stringstream os;
            os << (*this);
            return os.str();
        }
        friend std::ostream &operator<<(std::ostream &os, const colorize &o) {
            if (_colorful) {
                os << "\033[" << o.modifiers();
                if (o._true_color_enabled) {
                    if (o._fg != -1) {
                        os << "38;2;" << __R(o._fg) << ';' << __G(o._fg) << ';' << __B(o._fg);
                        if (o._bg != -1) os << ';';
                    }
                    if (o._bg != -1) {
                        os << "48;2;" << __R(o._bg) << ';' << __G(o._bg) << ';' << __B(o._bg);
                    }
                } else {
                    if (o._fg >= 0 && o._fg < 256) {
                        os << "38;5;" << o._fg;
                        if (o._bg >= 0 && o._bg < 256) os << ';';
                    }
                    if (o._bg >= 0 && o._bg < 256) {
                        os << "48;5;" << o._bg;
                    }
                }
                os << 'm';
            }

            // text
            if (o._cc != nullptr)
                os << o._cc; // << " // " << o.modifiers();
            if (!o._ss.empty())
                os << o._ss;

            if (_colorful) {
                // reset
                if (o._auto_reset) {
                    os << "\033[0m";

                    auto xo = const_cast<colorize *>(&o);
                    xo->_st._i = 0;
                    xo->_st._individual._auto_reset_invoked = true;
                    std::stringstream sz;
                    xo->_stocked_ss.swap(sz);
                    xo->_cc = nullptr;
                    xo->_ss.clear();
                    xo->_fg = xo->_bg = -1;
                }
            }
            return os;
        }
#undef __R
#undef __G
#undef __B
    };

    // inline colorize &colorize::instance() {
    //     static const std::unique_ptr<colorize> instance{new colorize{}};
    //     return *instance;
    // }

    inline bool colorize::_colorful;
    inline int colorize::_colors;


} // namespace cmdr::terminal::colors

namespace cmdr::terminal {

    const char *const fg_reset_all = "\033[0m";

    const char *const fg_default = "\033[39m";
    
    const char *const fg_black = "\033[30m";

    const char *const fg_red = "\033[31m";

    const char *const fg_green = "\033[32m";

    const char *const fg_yellow = "\033[33m";

    const char *const fg_blue = "\033[34m";

    const char *const fg_magenta = "\033[35m";

    const char *const fg_cyan = "\033[36m";

    const char *const fg_light_gray = "\033[37m";

    const char *const fg_dark_gray = "\033[90m";

    const char *const fg_light_red = "\033[91m";

    const char *const fg_light_green = "\033[92m";

    const char *const fg_light_yellow = "\033[93m";

    const char *const fg_light_blue = "\033[94m";

    const char *const fg_light_magenta = "\033[95m";

    const char *const fg_light_cyan = "\033[96m";

    const char *const fg_white = "\033[97m";

} //namespace cmdr::terminal

#endif //CMDR_CXX11_CMDR_TERMINAL_HH
