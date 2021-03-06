//
// Created by Hedzr Yeh on 2021/1/20.
//

#ifndef CMDR_CXX11_CMDR_TYPES_CHECK_HH
#define CMDR_CXX11_CMDR_TYPES_CHECK_HH

#include <chrono>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <cstdlib>
#include <memory>
#include <string>

namespace cmdr::checks {


    // template<class T, typename=void>
    // struct is_duration :std::false_type {
    // };
    //
    // template<class T,
    //          typename std::enable_if<
    //                  std::is_same<std::decay<T>::type,
    //                                               std::duration<typename std::decay<T>::type::value_type,typename std::decay<T>::type::value_type>::type > >
    // struct is_duration<T> :std::true_type {
    // };

#if 0
    template<class T, int = 0>
    struct is_duration : std::false_type {};

    template<class Rep, class Period>
    struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type {};

    template<class Rep, class Period>
    struct is_duration<const std::chrono::duration<Rep, Period>> : std::true_type {};

    template<class Rep, class Period>
    struct is_duration<volatile std::chrono::duration<Rep, Period>> : std::true_type {};

    template<class Rep, class Period>
    struct is_duration<const volatile std::chrono::duration<Rep, Period>> : std::true_type {};

    //template<class T, std::enable_if<std::is_same<std::decay<T>, std::chrono::seconds>::value, int>>
    //struct is_duration<T, 1> : std::true_type {};

    template<>
    struct is_duration<std::chrono::seconds> : std::true_type {};
#else

    template<typename T, typename _ = void>
    struct is_duration : std::false_type {};

    template<typename... Ts>
    struct is_duration_helper {};

    template<typename T>
    struct is_duration<
            T,
            std::conditional_t<
                    false,
                    is_duration_helper<
                            typename T::rep,
                            typename T::period,
                            decltype(std::declval<T>().count()),
                            decltype(std::declval<T>().zero()),
                            decltype(std::declval<T>().min()),
                            decltype(std::declval<T>().max())>,
                    void>> : public std::true_type {};

#endif

    //


    template<class T>
    struct is_vector {
        using type = T;
        constexpr static bool value = false;
    };

    template<class T>
    struct is_vector<std::vector<T>> {
        using type = std::vector<std::decay<T>>;
        constexpr static bool value = true;
    };

    // and the two "olbigatory" aliases
    //
    // Usages:
    //     std::cout << std::boolalpha;
    //     std::cout << is_vector_v<std::vector<int>> << '\n' ;
    //     std::cout << is_vector_v<int> << '\n'
    template<typename T>
    inline constexpr bool is_vector_v = is_vector<T>::value;

    template<typename T>
    using is_vector_t = typename is_vector<T>::type;


#if defined(ANOTHER_IS_VECTOR)
    template<typename T, typename _ = void>
    struct isVector : std::false_type {
    };

    template<typename T>
    struct isVector<T,
                    typename std::enable_if<
                            std::is_same<typename std::decay<T>::type,
                                         std::vector<typename std::decay<T>::type::value_type, typename std::decay<T>::type::allocator_type>>::value>::type> : std::true_type {
    };
#endif


    template<typename T, typename = void>
    struct is_container : std::false_type {};

    template<typename T>
    struct is_container<T, std::void_t<decltype(std::declval<T>().data()), decltype(std::declval<T>().size())>> : std::true_type {};

    //specialize a type for all of the STL containers.
    //
    // better than is_container & is_vector, see also:
    //   https://stackoverflow.com/questions/9407367/determine-if-a-type-is-an-stl-container-at-compile-time/31105859#31105859
    namespace is_stl_container_impl {
        template<typename T>
        struct is_stl_container : std::false_type {};
        template<typename T, std::size_t N>
        struct is_stl_container<std::array<T, N>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::vector<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::deque<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::list<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::forward_list<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::set<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::multiset<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::map<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::multimap<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::unordered_set<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::unordered_multiset<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::unordered_map<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::unordered_multimap<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::stack<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::queue<Args...>> : std::true_type {};
        template<typename... Args>
        struct is_stl_container<std::priority_queue<Args...>> : std::true_type {};
    } // namespace is_stl_container_impl

    //type trait to utilize the implementation type traits as well as decay the type
    template<typename T>
    struct is_stl_container {
        static constexpr bool const value = is_stl_container_impl::is_stl_container<T>::value;
    };


    // https://stackoverflow.com/questions/12042824/how-to-write-a-type-trait-is-container-or-is-vector
    template<typename T, typename _ = void>
    struct is_generic_container : std::false_type {};

    template<typename... Ts>
    struct is_generic_container_helper {};

    template<typename T>
    struct is_generic_container<
            T,
            std::conditional_t<
                    false,
                    is_generic_container_helper<
                            typename T::value_type,
                            typename T::size_type,
                            typename T::allocator_type,
                            typename T::iterator,
                            typename T::const_iterator,
                            decltype(std::declval<T>().size()),
                            decltype(std::declval<T>().begin()),
                            decltype(std::declval<T>().end()),
                            decltype(std::declval<T>().cbegin()),
                            decltype(std::declval<T>().cend())>,
                    void>> : public std::true_type {};


    // SFINAE assertion class here: is_streamable
    // original: https://stackoverflow.com/questions/22758291/how-can-i-detect-if-a-type-can-be-streamed-to-an-stdostream
    template<typename T, typename S = std::ostream>
    class is_streamable {
        template<typename SS, typename TT>
        static auto test(int)
                -> decltype(std::declval<SS &>() << std::declval<TT>(), std::true_type());

        template<typename, typename>
        static auto test(...) -> std::false_type;

    public:
        static const bool value = decltype(test<S, T>(0))::value;
    };


} // namespace cmdr


#if 0
//
// http://coliru.stacked-crooked.com/a/ff9a456aaee6ec47
// https://stackoverflow.com/questions/40439909/c98-03-stdis-constructible-implementation
// https://godbolt.org/#g:!((g:!((g:!((h:codeEditor,i:(fontScale:14,j:1,lang:c%2B%2B,selection:(endColumn:2,endLineNumber:20,positionColumn:1,positionLineNumber:2,selectionStartColumn:2,selectionStartLineNumber:20,startColumn:1,startLineNumber:2),source:%27%23define+_LIBCPP_TESTING_FALLBACK_IS_CONSTRUCTIBLE%0A%23include+%3Ctype_traits%3E%0A%0Astruct+D1%3B%0Astruct+D2%3B%0Astruct+Base+%7B%0A++++explicit+operator+D1%26()%3B%0A%7D%3B%0A%0Astruct+D1+:+Base+%7B%0A++++D1(const+D1%26)+%3D+delete%3B%0A%7D%3B%0Astruct+D2+:+Base+%7B%7D%3B%0A%0Aint+BtoD1()+%7B%0A++++return+std::is_constructible%3CD1%26,+Base%26%3E::value%3B%0A%7D%0Aint+BtoD2()+%7B%0A++++return+std::is_constructible%3CD2%26,+Base%26%3E::value%3B%0A%7D%0A%0A%23if+defined(__clang__)+%7C%7C+(__GNUC__+%3E%3D+8)%0Aint+BtoD1_builtin()+%7B%0A++++return+__is_constructible(D1%26,+Base%26)%3B%0A%7D%0Aint+BtoD2_builtin()+%7B%0A++++return+__is_constructible(D2%26,+Base%26)%3B%0A%7D%0A%23endif%0A%0A%23ifdef+_LIBCPP_VERSION%0Aint+BtoD1_lib()+%7B%0A++++return+std::__libcpp_is_constructible%3CD1%26,+Base%26%3E::value%3B%0A%7D%0Aint+BtoD2_lib()+%7B%0A++++return+std::__libcpp_is_constructible%3CD2%26,+Base%26%3E::value%3B%0A%7D%0A%23endif%27),l:%275%27,n:%270%27,o:%27C%2B%2B+source+%231%27,t:%270%27)),k:50,l:%274%27,n:%270%27,o:%27%27,s:0,t:%270%27),(g:!((g:!((h:compiler,i:(compiler:clang400,filters:(b:%270%27,binary:%271%27,commentOnly:%270%27,demangle:%270%27,directives:%270%27,execute:%271%27,intel:%270%27,libraryCode:%271%27,trim:%271%27),fontScale:14,j:1,lang:c%2B%2B,libs:!(),options:%27-O2+-std%3Dc%2B%2B11+-stdlib%3Dlibc%2B%2B%27,selection:(endColumn:1,endLineNumber:1,positionColumn:1,positionLineNumber:1,selectionStartColumn:1,selectionStartLineNumber:1,startColumn:1,startLineNumber:1),source:1),l:%275%27,n:%270%27,o:%27x86-64+clang+4.0.0+(Editor+%231,+Compiler+%231)+C%2B%2B%27,t:%270%27)),k:50,l:%274%27,m:68.14159292035397,n:%270%27,o:%27%27,s:0,t:%270%27),(g:!((h:output,i:(compiler:1,editor:1,fontScale:14,wrap:%271%27),l:%275%27,n:%270%27,o:%27%231+with+x86-64+clang+4.0.0%27,t:%270%27)),header:(),l:%274%27,m:31.85840707964602,n:%270%27,o:%27%27,s:0,t:%270%27)),k:50,l:%273%27,n:%270%27,o:%27%27,t:%270%27)),l:%272%27,n:%270%27,o:%27%27,t:%270%27)),version:4
//
namespace cmdr::types {

#if __GNUC__ < 10

    template<typename T, T Val>
    struct integral_constant {
        typedef integral_constant type;
        typedef T value_type;
        enum {
            value = Val
        };
    };

    typedef integral_constant<bool, true> true_type;
    typedef integral_constant<bool, false> false_type;

    template<typename T>
    struct remove_ref {
        typedef T type;
    };

    template<typename T>
    struct remove_ref<T &> {
        typedef T type;
    };

    // is_base_of from https://stackoverflow.com/questions/2910979/how-does-is-base-of-work
    namespace aux {
        typedef char yes[1];
        typedef char no[2];

        template<typename B, typename D>
        struct Host {
            operator B *() const;
            operator D *();
        };
    } // namespace aux
    template<typename B, typename D>
    struct is_base_of {
        template<typename T>
        static aux::yes &check(D *, T);
        static aux::no &check(B *, int);

        static const bool value = sizeof(check(aux::Host<B, D>(), int())) == sizeof(aux::yes);
    };

    template<typename T>
    struct remove_cv {
        typedef T type;
    };
    template<typename T>
    struct remove_cv<const T> {
        typedef T type;
    };
    template<typename T>
    struct remove_cv<volatile T> {
        typedef T type;
    };
    template<typename T>
    struct remove_cv<const volatile T> {
        typedef T type;
    };

    template<typename T>
    struct is_void : integral_constant<bool, false> {};
    template<>
    struct is_void<void> : integral_constant<bool, true> {};

    template<bool, typename T, typename>
    struct conditional {
        typedef T type;
    };
    template<typename T, typename U>
    struct conditional<false, T, U> {
        typedef U type;
    };


    namespace aux {

        template<typename T, typename U>
        struct is_more_const : integral_constant<bool, false> {};

        template<typename T, typename U>
        struct is_more_const<const T, U> : integral_constant<bool, true> {};

        template<typename T, typename U>
        struct is_more_const<const T, const U> : integral_constant<bool, false> {};

        template<typename T, typename U>
        struct is_more_volatile : integral_constant<bool, false> {};

        template<typename T, typename U>
        struct is_more_volatile<volatile T, U> : integral_constant<bool, true> {};

        template<typename T, typename U>
        struct is_more_volatile<volatile T, volatile U> : integral_constant<bool, false> {};

        template<typename T, typename U>
        struct is_more_cv : integral_constant<bool, is_more_const<T, U>::value && is_more_volatile<T, U>::value> {};


        template<typename T>
        struct is_default_constructible {
            template<typename U>
            static yes &test(int (*)[sizeof(new U)]);
            template<typename U>
            static no &test(...);
            enum {
                value = sizeof(test<T>(0)) == sizeof(yes)
            };
        };

        template<typename T, typename Arg>
        struct is_constructible_1 {
            template<typename U, typename Arg_>
            static yes &test(int (*)[sizeof(U(static_cast<Arg_>(*((typename remove_ref<Arg_>::type *) 0))))]);
            template<typename U, typename Arg_>
            static no &test(...);
            enum {
                value = sizeof(test<T, Arg>(0)) == sizeof(yes)
            };
        };

        // Base pointer construct from Derived Pointer
        template<typename T, typename U>
        struct is_constructible_1<T *, U *>
            : conditional<
                      is_void<typename remove_cv<T>::type>::value,
                      integral_constant<bool, true>,
                      typename conditional<
                              is_void<typename remove_cv<U>::type>::value,
                              integral_constant<bool, false>,
                              typename conditional<
                                      is_more_cv<T, U>::value,
                                      integral_constant<bool, false>,
                                      is_base_of<T, U>>::type>::type>::type {};

        // Base pointer construct from Derived Pointer
        template<typename T, typename U>
        struct is_constructible_1<T &, U &>
            : conditional<
                      is_more_cv<T, U>::value,
                      integral_constant<bool, false>,
                      is_base_of<T, U>>::type {};


        template<typename T, typename Arg1, typename Arg2>
        struct is_constructible_2 {
            template<typename U, typename Arg1_, typename Arg2_>
            static yes &test(int (*)[sizeof(U(
                    static_cast<Arg1_>(*((typename remove_ref<Arg1_>::type *) 0)),
                    static_cast<Arg2_>(*((typename remove_ref<Arg2_>::type *) 0))))]);
            template<typename U, typename Arg1_, typename Arg2_>
            static no &test(...);
            enum {
                value = sizeof(test<T, Arg1, Arg2>(0)) == sizeof(yes)
            };
        };
    } // namespace aux

    template<typename T, typename Arg1 = void, typename Arg2 = void>
    struct is_constructible : integral_constant<bool, aux::is_constructible_2<T, Arg1, Arg2>::value> {
    };

    template<typename T, typename Arg>
    struct is_constructible<T, Arg> : integral_constant<bool, aux::is_constructible_1<T, Arg>::value> {
    };
    template<typename T>
    struct is_constructible<T> : integral_constant<bool, aux::is_default_constructible<T>::value> {
    };

#else
    using is_constructible = std::is_constructible;
#endif

} // namespace cmdr::types

#endif // 0


#endif //CMDR_CXX11_CMDR_TYPES_CHECK_HH
