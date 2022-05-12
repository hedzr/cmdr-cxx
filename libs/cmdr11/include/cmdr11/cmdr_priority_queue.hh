//
// Created by Hedzr Yeh on 2021/3/30.
//

#ifndef CMDR_CXX_CMDR_PRIORITY_QUEUE_HH
#define CMDR_CXX_CMDR_PRIORITY_QUEUE_HH

#include <functional>
#include <list>
#include <string>
#include <vector>

#include <iomanip>
#include <iostream>
#include <sstream>

#include <memory>

#include <cassert>


namespace cmdr::queue {

  template<class T, class PT = int>
  using comparer = std::function<PT(T const &lhs, T const &rhs)>;


  //


  template<class T,
           class PT                                        = int,
           class Comp                                      = comparer<T, PT>,
           template<typename, typename...> class Container = std::vector,
           // class Container = std::list<T>,
           bool ReverseComp                                = false>
  class priority_queue_todo {
  };


  //


  template<class T,
           class PT         = int,
           class Comp       = comparer<T, PT>,
           class Container  = std::list<T>,
           bool ReverseComp = false>
  class priority_queue {
  public:
    struct element;
    using value_type = T;
    struct _It {
      element *_el{};
      std::size_t _pos{(std::size_t) -1};

      priority_queue::value_type *get() const;
      bool operator==(_It const &rhs) const { return _el == rhs._el && _pos == rhs._pos; }
      bool operator!=(_It const &rhs) const { return _el != rhs._el || _pos != rhs._pos; }
      bool operator()() const { return _el != nullptr; }
      bool operator!() const { return _el == nullptr; }
      element *operator->() { return _el; }

      _It() = default;
      _It(element *el, std::size_t pos)
          : _el(el)
          , _pos(pos) {}
      _It(const _It &o)
          : _el(o._el)
          , _pos(o._pos) {}
      _It &operator=(const _It &o) {
        _el  = o._el;
        _pos = o._pos;
        return (*this);
      }
    };
    struct element {
      Container _list;
      int _min_value;
      std::shared_ptr<element> _left{};
      std::shared_ptr<element> _right{};
      element()  = default;
      ~element() = default;
      element(Container &&l, int m)
          : _list(l)
          , _min_value(m) {}
      element(value_type l, int m)
          : _list{}
          , _min_value(m) {
        _list.push_back(std::move(l));
      }
      // pre-order traversal
      void NLR(std::function<void(element *)> const &fn) {
        fn(this);
        if (_left)
          _left->LNR(fn);
        if (_right)
          _right->LNR(fn);
      }
      // in-order traversal
      void LNR(std::function<void(element *)> const &fn) {
        if (_left)
          _left->LNR(fn);
        fn(this);
        if (_right)
          _right->LNR(fn);
      }
      // post-order traversal
      void LRN(std::function<void(element *)> const &fn) {
        if (_left)
          _left->LRN(fn);
        if (_right)
          _right->LRN(fn);
        fn(this);
      }
      // reverse in-order
      void RNL(std::function<void(element *)> const &fn) {
        if (_right)
          _right->LRN(fn);
        fn(this);
        if (_left)
          _left->LRN(fn);
      }

      T pop(std::size_t &count) {
        if (ReverseComp) {
          if (_left) {
            std::size_t before = count;
            T t                = _left->pop(count);
            if (before > count)
              return t;
          }
          if (!_list.empty()) {
            count--;
            T t;
            std::swap(t, _list.front());
            _list.pop_front();
            return t;
          }
          if (_right) {
            std::size_t before = count;
            T t                = _right->pop(count);
            if (before > count)
              return t;
          }
          return _null;
        }

        // normal
        if (_right) {
          std::size_t before = count;
          T t                = _right->pop(count);
          if (before > count)
            return t;
        }
        if (!_list.empty()) {
          count--;
          T t;
          std::swap(t, _list.front());
          _list.pop_front();
          return t;
        }
        if (_left) {
          std::size_t before = count;
          T t                = _left->pop(count);
          if (before > count)
            return t;
        }
        return _null;
      }

      _It next(std::size_t pos) {
        if (ReverseComp) {
          if (pos == (std::size_t) -1) {
            if (_left) {
              auto p = _left->next(pos);
              if (p._el != nullptr)
                return p;
            }
            if (!_list.empty()) {
              return {this, 0};
            }
            if (_right) {
              auto p = _right->next(pos);
              if (p._el != nullptr)
                return p;
            }
          } else {
            assert(!_list.empty() && _list.size() >= pos);
            if (pos < _list.size() - 1)
              return {this, pos + 1};
            if (_right) {
              auto p = _right->next(0);
              if (p._el != nullptr)
                return p;
            }
          }
          return _It{};
        }

        // normal
        if (pos == (std::size_t) -1) {
          if (_right) {
            auto p = _right->next(pos);
            if (p._el != nullptr)
              return p;
          }
          if (!_list.empty()) {
            return {this, 0};
          }
          if (_left) {
            auto p = _left->next(pos);
            if (p._el != nullptr)
              return p;
          }
        } else {
          assert(!_list.empty() && _list.size() >= pos);
          if (pos < _list.size() - 1)
            return {this, pos + 1};
          if (_left) {
            auto p = _left->next(0);
            if (p._el != nullptr)
              return p;
          }
        }
        return _It{};
      }

      static priority_queue::value_type _null;
    };
    struct my_iterator {
      using iterator_category = std::forward_iterator_tag;
      using difference_type   = std::ptrdiff_t;
      using value_type        = priority_queue::value_type;
      using pointer           = priority_queue::value_type *;
      using reference         = priority_queue::value_type &;

      my_iterator()           = default;
      my_iterator(element *ptr, std::size_t pos = (std::size_t) -1)
          : _it{ptr, pos} {}

      reference operator*() const {
        _It &it = const_cast<_It &>(_it);
        // pointer ptr = it.get();
        auto &z = it._el->_list;
        auto x  = z.begin();
        std::advance(x, it._pos);
        reference data = (*x);
        return data;
      }
      pointer operator->() {
        _It &it = const_cast<_It &>(_it);
        // return it.get();
        auto &z = it._el->_list;
        auto x  = z.begin();
        std::advance(x, it._pos);
        reference data = (*x);
        auto *ptr      = &data;
        return ptr;
      }
      my_iterator &operator++() {
        if (_it._el != nullptr)
          _it = _it->next(_it._pos);
        return *this;
      }
      my_iterator operator++(int) {
        my_iterator tmp = *this;
        ++(*this);
        return tmp;
      }
      friend bool operator==(const my_iterator &a, const my_iterator &b) { return a._it == b._it; };
      friend bool operator!=(const my_iterator &a, const my_iterator &b) { return a._it != b._it; };

    private:
      _It _it;
    };

  public:
    priority_queue()
        : _root{std::make_shared<element>()}
        , _comparer{} {}
    virtual ~priority_queue() {}

    my_iterator begin() { return my_iterator{_root.get(), 0}; }
    my_iterator end() { return my_iterator(nullptr); }
    void push_back(T const &data) { push(data); }
    void pop_front() { _root->pop(_count); }
    T &front() { return *begin(); }
    std::size_t size() const { return _count; }
    bool empty() const { return _count == 0; }

    void push(T data) {
      _push(_root, std::move(data));
      _count++;
    }
    T pop() { return _root->pop(_count); }
    static bool is_null(T const &t) { return t == element::_null; }

  public:
    void dump(std::function<void(element *)> const &fn) { ReverseComp ? _root->LNR(fn) : _root->RNL(fn); }

  protected:
    void mid_walk(std::function<void(element *)> const &fn) { _root->LNR(fn); }
    void pot_walk(std::function<void(element *)> const &fn) { _root->rgt_walk(fn); }
    void lft_walk(std::function<void(element *)> const &fn) { _root->LRN(fn); }

  private:
    void _push(std::shared_ptr<element> &at, T data) {
      PT l{}, r{};
      if (!at->_list.empty()) {
        for (auto const &v: at->_list) {
          PT ret = _comparer(data, v);
          if (ret < 0) {
            if (ret < l)
              l = ret;
          } else if (ret > 0) {
            if (ret > r)
              r = ret;
          }
        }
      }

      if (l < 0) {
        if (at->_left) {
          _push(at->_left, data);
        } else {
          at->_left = std::make_shared<element>(element{data, l});
        }
        return;
      }

      if (r > 0) {
        if (at->_right) {
          _push(at->_right, data);
        } else {
          at->_right = std::make_shared<element>(element{data, r});
        }
        return;
      }

      at->_list.push_back(data);
    }

  private:
    std::shared_ptr<element> _root;
    Comp _comparer{};
    std::size_t _count{};
  };

  template<class T,
           class PT,
           class Comp,
           class Container,
           bool ReverseComp>
  inline typename priority_queue<T, PT, Comp, Container, ReverseComp>::value_type *
  priority_queue<T, PT, Comp, Container, ReverseComp>::_It::get() const {
    auto &z = const_cast<element *>(_el)->_list;
    auto x  = z.begin();
    std::advance(x, _pos);
    auto &data = (*x);
    auto *ptr  = &data;
    return ptr;
  }

  template<class T,
           class PT,
           class Comp,
           class Container,
           bool ReverseComp>
  inline typename priority_queue<T, PT, Comp, Container, ReverseComp>::value_type priority_queue<T, PT, Comp, Container, ReverseComp>::element::_null{};

} // namespace cmdr::queue


#endif // CMDR_CXX_CMDR_PRIORITY_QUEUE_HH
