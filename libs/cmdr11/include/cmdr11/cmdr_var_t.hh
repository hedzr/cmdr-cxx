//
// Created by Hedzr Yeh on 2021/1/16.
//

#ifndef CMDR_CXX11_CMDR_VAR_T_HH
#define CMDR_CXX11_CMDR_VAR_T_HH

#include <any>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <optional>
#include <sstream>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>


#include "cmdr_chrono.hh"
#include "cmdr_defs.hh"
#include "cmdr_types.hh"


namespace cmdr::opt::vars {

#if defined(NEVER_USED)
    template<typename T>
    class variable {
    public:
        T &value() { return _value; }

        [[nodiscard]] const T &value() const { return _value; }

        friend std::ostream &operator<<(std::ostream &output, const variable &v) {
            output << v._value;
            return output;
        }

        friend std::istream &operator>>(std::istream &input, variable &v) {
            input >> v._value;
            return input;
        }

        // friend std::ostream &operator<<(std::ostream &output, const T &v) {
        //     output << v;
        //     return output;
        // }
        //
        // friend std::istream &operator>>(std::istream &input, T &v) {
        //     input >> v;
        //     return input;
        // }

        static void test() {
            // variable<std::string> v;
            variable<std::string> v("a string");
            std::cout << v << std::endl;
            std::istringstream is("abc-string");
            is >> v;
            std::cout << v << " | expect 'abc-string' after stream inputting" << std::endl;

#if BETTER
            variable<std::string> v2(8, ' ');
#else
            variable<std::string> v2(std::in_place, 8, ' ');
#endif
            std::cout << std::quoted(v2.value())
                      << " | expect '        ' (8-spaces) after in-place construction"
                      << std::endl;

            variable<std::string> v3 = v2;
            std::cout << std::quoted(v3.value()) << std::endl;

            variable<std::string> v4(v2);
            std::cout << std::quoted(v4.value()) << std::endl;
        }

    public:
        variable() = default;
        ~variable() = default;

#if BETTER

        variable(variable const &) = default;
        variable(variable &&) noexcept = default;

        template<typename A = T, typename... Args,
                 std::enable_if_t<
                         std::is_constructible<T, A, Args...>::value &&
                                 !std::is_same<std::decay_t<A>, variable>::value,
                         int> = 0>
        explicit variable(A &&a0, Args &&...args)
            : _value(std::forward<A>(a0), std::forward<Args>(args)...) {}

        explicit variable(T &&v)
            : _value(std::move(v)) {}

#else

        variable(const variable &o)
            : _value(o._value) {}
        variable(variable &&o) noexcept
            : _value(o._value) {}
        explicit variable(const T &t)
            : _value(t) {}
        explicit variable(T &&t)
            : _value(std::forward<T>(t)) {}

        template<class... ARGS>
        explicit variable(std::in_place_t, ARGS &&...args)
            : _value(T(std::forward<ARGS>(args)...)) {}

#endif

    private:
        T _value;
    };


    class extractor {
    public:
        // app:
        //   server:
        //     tls:
        //       enabled: true
        //
        //  app *vt{}
        //  app.server *vt{}
        //  app.server.tls  *vt{}
        //  app.server.tls.enabled  *vt{}
        //
        //  app vt{
        //    server vt {
        //      tls {
        //        enabled: vt{enabled => true}
        //        ca {
        //          file: vt {file => 'ca.cer'}
        //        }
        //      }
        //      statics {}
        //    }
        //    client vt {
        //    }
        //  }
        //
        //
        // ['app', []
        // ]
    };

#endif // defined(NEVER_USED)


    struct streamable_any : std::any {
        void (*streamer)(std::ostream &, streamable_any const &); //= nullptr;
        friend std::ostream &operator<<(std::ostream &os, streamable_any const &a) {
            a.streamer(os, a);
            return os;
        }

        // template<class T, std::enable_if_t<!is_duration<T>::value, int> = 0>
        // static void _format(std::ostream &os, T const &v) {
        //     os << v;
        // }
        // template<class T>
        // static void _format<T,
        //                     std::enable_if_t<is_duration<T>::value, int>>(std::ostream &os, T const &v) {
        //     format_duration(os, v);
        // }

        template<class U>
        explicit streamable_any(U &&t, typename std::enable_if<is_duration<std::decay_t<U>>::value>::type * = nullptr)
            : std::any(std::forward<U>(t))
            , streamer([](std::ostream &os, streamable_any const &self) {
                static_assert(is_duration<std::decay_t<U>>::value);
                // os << "[**duration**] ";
                chrono::format_duration(os, std::any_cast<std::decay_t<U>>(self));
            }) {}

        template<class V>
        explicit streamable_any(V &&t, typename std::enable_if<is_stl_container<std::decay_t<V>>::value>::type * = nullptr)
            : std::any(std::forward<V>(t))
            , streamer([](std::ostream &os, streamable_any const &self) {
                os << '[';
                for (auto const v : std::any_cast<std::decay_t<V>>(self)) {
                    os << v << ',';
                }
                os << ']';
            }) {}

        template<class T,
                 typename std::enable_if<
                         !is_duration<std::decay_t<T>>::value &&
                         !is_stl_container<std::decay_t<T>>::value &&
                         !std::is_same<std::decay_t<T>, streamable_any>{}>::type * = nullptr>
        explicit streamable_any(T &&t)
            : std::any(std::forward<T>(t))
            , streamer([](std::ostream &os, streamable_any const &self) {
                static_assert(!is_duration<std::decay_t<T>>::value);
                static_assert(!is_stl_container<T>::value);
                static_assert(!std::is_same<std::decay_t<T>, streamable_any>{});
                if constexpr (!std::is_void_v<T>) {
                    //if constexpr (is_duration<std::decay_t<T>>::value)
                    //    os << "norm - ";
                    os << std::any_cast<std::decay_t<T>>(self);
                    //auto const &vv = std::any_cast<std::decay_t<T>>(self);
                    //os << typeid(vv).name() << " (T=" << typeid(T).name() << ")";

                    // (void)(os);(void)(self);
                    //_format(os, std::any_cast<std::decay_t<T>>(self));
                    // if constexpr (is_duration<T>::value)
                    //     format_duration(os, std::any_cast<std::decay_t<T>>(self));
                    // else
                    //     os << std::any_cast<std::decay_t<T>>(self);
                }
            }) {}

        // specialize for bool streaming output
        explicit streamable_any(bool t)
            : std::any(std::forward<bool>(t))
            , streamer([](std::ostream &os, streamable_any const &self) {
                os << (std::any_cast<std::decay_t<bool>>(self) ? "true" : "false");
            }) {}

        streamable_any()
            : streamer([](std::ostream &, streamable_any const &) {}) {}
        ~streamable_any() {}
    };

    /**
     * @brief a node at a hierarchical data structure
     * @tparam holderT the real value type stored in this node
     *
     * A node holds a key-part and the underlying value.
     * We can access a node by its full dotted-separated path, for
     * example, 'app.server' and 'app.server.tls' are two nodes.
     */
    template<typename holderT = streamable_any>
    class var_t {
    public:
        typedef var_t self_type;
        typedef std::list<self_type> var_t_array;
        typedef std::unordered_map<std::string, var_t> children_container;
        typedef std::unordered_map<std::string, var_t *> child_pointers;

        const int NODE_DEFAULT_VALUE{0};

    public:
        var_t() = default;
        ~var_t() = default;
        var_t(var_t const &) = default;
        var_t(var_t &&) noexcept = default;

        template<typename A = holderT, typename... Args,
                 std::enable_if_t<
                         std::is_constructible<holderT, A, Args...>::value &&
                                 !std::is_same<std::decay_t<A>, var_t>::value,
                         int> = 0>
        explicit var_t(A &&a0, Args &&...args)
            : _value(std::forward<A>(a0), std::forward<Args>(args)...) {}

        explicit var_t(holderT &&v)
            : _value(std::move(v)) {}

        bool operator==(const var_t &r) {
            return _value == r._value;
        }

    public:
        var_t &get(const_chars key) { return _get(key); }
        var_t &get(const std::string &key) { return _get(key); }
        const var_t &get(const_chars key) const { return _get(key); }
        const var_t &get(const std::string &key) const { return _get(key); }

    private:
        var_t &_get(const std::string &key) {
            auto &it = _full_key_map.find(key);
            if (it == _full_key_map.end())
                return null_elem();
            return (*(it.second));
        }

    public:
        // void set(const_chars key, const holderT &val) {
        //     std::string ks(key);
        //     set(ks, val);
        // }

        void set(const std::string &key, holderT &&val, self_type *parent) { _put_no_parent(key, val, parent); }
        void set(const_chars key, holderT &&val, self_type *parent) { _put_no_parent(key, val, parent); }
        void set(const_chars key, const holderT &val, self_type *parent) { _put_no_parent(key, val, parent); }

    private:
        void _put_no_parent(const std::string &key, const holderT &val, self_type *parent) {
            _put(key, val, parent, [this, key](const std::string &prefix, self_type *ptr) {
                // auto sk = key;
                // sk += '.';
                // sk += prefix;
                // if (auto it = _full_key_map.find(prefix); it == _full_key_map.end()) {
                //     _full_key_map.template emplace(sk, ptr);
                // }
                (void) (prefix);
                (void) (ptr);
                // std::cout << "this: " << this << ", key: " << key << ", matching key: " << prefix << std::endl;
                (void) (this);
                (void) (key);
            });
        }

        void _put(const std::string &key, const holderT &val, self_type *parent,
                  std::function<void(const std::string &, self_type *)> const &on_picked) {
            std::string prefix1, part1, remains;
            std::string::size_type pos{0}, old{0}, length = key.length();
            var_t *ptr;
            if (pos < length) {
                pos = key.find('.', old);
                if (pos != std::string::npos) {
                    part1 = key.substr(old, pos);
                    old = pos + 1;
                    remains = key.substr(old);
                } else {
                    part1 = key;
                    pos = length;
                    remains.clear();
                }

                // TODO optimize part1 with a small string pointer PTR, using PTR as the key of the hash_map (_children) and _key_part

                typename children_container::iterator itc = _children.find(part1);
                if (itc == _children.end()) {
                    if (remains.empty())
                        _children.template emplace(part1, val);
                    else
                        _children.template emplace(part1, NODE_DEFAULT_VALUE == 0 ? holderT() : holderT(NODE_DEFAULT_VALUE));
                    itc = _children.find(part1);
                }
                ptr = &((*itc).second);
                ptr->_parent = parent;

                prefix1 = key.substr(0, pos);
                if (auto it = _full_key_map.find(prefix1); it == _full_key_map.end()) {
                    _full_key_map.template emplace(prefix1, ptr);
                }
                if (on_picked)
                    on_picked(prefix1, this);

                // auto p = this;
                // while (p._parent != nullptr) {
                //     p = p._parent;
                //     // add this ptr to the full_key_map of parents
                //     prefix1 = p
                // }

                if (!remains.empty()) {
                    ptr->_put(remains, val, ptr, [this, on_picked, prefix1](const std::string &prefix, self_type *ptr) {
                        auto sk = prefix1;
                        sk += '.';
                        sk += prefix;
                        if (this->_parent == nullptr) {
                            if (auto it = _full_key_map.find(sk); it == _full_key_map.end()) {
                                _full_key_map.template emplace(sk, ptr);
                            }
                        }

                        if (on_picked)
                            on_picked(sk, ptr);
                    });
                }
            }
        }

    public:
#if defined(_DEBUG)
        struct map_streamer {
            std::ostream &_os;

            explicit map_streamer(std::ostream &os)
                : _os(os) {}

            template<class K, class V>
            void operator()(std::pair<K, V> const &val) {
                // .first is your key, .second is your value
                _os << " - " << val.first << " : " << val.second << "\n";
            }
        };

        template<class K, class V, class Comp = std::less<K>>
        inline void print_sorted(std::ostream &os, std::unordered_map<K, V> const &um, Comp pred = Comp()) const {
            std::map<K, V> m(um.begin(), um.end(), pred);
            std::for_each(m.begin(), m.end(), map_streamer(os));
        }

        // template<class K, class V>
        // void print_sorted(std::unordered_map<K, V> const &um) {
        //     print_sorted(um, std::less<K>());
        // }

        void dump_full_keys(std::ostream &os, const_chars leading_title = nullptr) const {
            if (leading_title) os << leading_title;
            else
                os << "Dumping for var_t ...";
            os << std::endl;

            // std::vector<std::string> keys;
            // keys.reserve(_full_key_map.size());
            // for (auto &it : _full_key_map) {
            //     keys.push_back(it.first);
            // }
            // std::sort(keys.begin(), keys.end());
            //
            // for (auto &it : keys) {
            //     os << " - " << it << " => " << _full_key_map[it] << std::endl;
            // }
            print_sorted(os, _full_key_map);
        }

        // template<class K, class V, class Comp = std::less<K>>
        void dump_tree(std::ostream &os, const_chars leading_title = nullptr) const {
            if (leading_title) os << leading_title;
            else
                os << "Dumping for var_t as Tree ...";
            os << std::endl;

            dump_tree(os, _children, 0);
        }

    private:
        template<class K, class V>
        void dump_tree(std::ostream &os, std::unordered_map<K, V> const &um, int level) const {
            for (auto const &[k, v] : um) {
                for (int i = 0; i < level; i++)
                    os << "  ";
                os << k << ": " << v << std::endl;
                dump_tree(os, v._children, level + 1);
            }
        }
#endif

    public:
        const self_type &null_elem() const {
            static self_type o;
            return o;
        }

        [[nodiscard]] bool is_empty() const { return (*this) == null_elem(); }

        holderT &value() { return _value; }
        const holderT &value() const { return _value; }

        template<class T>
        void format(std::ostream &os, T const &val) {
            os << val;
        }

        friend std::ostream &operator<<(std::ostream &output, const var_t &v) {
            output << v._value;
            return output;
        }

        friend std::istream &operator>>(std::istream &input, var_t &v) {
            input >> v._value;
            return input;
        }

    private:
        // the saved entries of `app' looks like:
        //   server    array("tls", "port", "host", ...)
        //   client    array("tls", "server-addr", ...)
        //   logging   array("level", "target", ...)
        //   ...
        children_container _children;
        // the saved entries looks like:
        //   app.server         *var_t
        //   app.server.cert    *var_t
        //   app.client         *var_t
        //   app.logging        *var_t
        child_pointers _full_key_map;
        // any type data
        holderT _value;
        self_type *_parent{nullptr}; // need not to use std::weak_ptr
        // std::string _key_part;
    }; // class opt_var_t


    template<typename holderT = streamable_any>
    class store {
    public:
        typedef var_t<holderT> var_type;

    public:
        store() = default;
        ~store() = default;

    public:
        holderT &get(const_chars key) {
            auto &it = _root.get(key);
            return it.value();
        }

        void set(const_chars key, holderT &&val) { _root.set(key, val, &_root); }

        template<class Any>
        void set(const_chars key, Any &&val) {
            holderT v(val);
            _root.set(key, v, &_root);
        }

        template<class Any>
        void set(const_chars key, const Any &val) {
            holderT v(val);
            _root.set(key, v, &_root);
        }

    public:
        const var_type &root() const { return _root; }

        template<class T>
        void format(std::ostream &os, T const &v) const {
            os << v;
        }

        friend std::ostream &operator<<(std::ostream &output, store const &v) {
            v.format(output, v._root);
            return output;
        }

        friend std::istream &operator>>(std::istream &input, store &v) {
            input >> v._root;
            return input;
        }

    private:
        var_type _root;
    }; // class store

} // namespace cmdr::opt::vars

#endif //CMDR_CXX11_CMDR_VAR_T_HH
