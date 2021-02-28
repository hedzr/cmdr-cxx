//
// Created by Hedzr Yeh on 2021/1/29.
//

#include <valarray>

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>

#include <functional>

#include <deque>
#include <list>
#include <vector>


namespace bet {

    template<class T>
    class ti_queue {
    public:
        struct element {
            T _data;
            element *_last;
            element *_next;
        };

    public:
        ti_queue() {
            _head = new element;
            _tail = new element;
            _head->_next = _tail;
            _tail->_last = _head;
        }
        ~ti_queue() {
            auto *p = _head;
            while (p != nullptr) {
                auto *t = p;
                p = p->_next;
                delete t;
            }
        }

        void push(T const &data) {
            auto h = _tail->_last;
            auto *ptr = new element{data, h, _tail};
            _tail->_last = ptr;
            h->_next = ptr;
        }

        T pop() {
            auto p = _head->_next;
            if (p == _tail) {
                return T{};
            }
            p->_last->_next = p->_next;
            p->_next->_last = p->_last;
            T t = p->_data;
            delete p;
            return t;
        }

        bool empty() const { return _tail->_last == _head; }

        T const &peek() const {
            if (empty()) return _tail->_data;
            return _tail->_last->_data;
        }

    private:
        element *_head{};
        element *_tail{};
    };


    template<class T, class PT = int>
    using comparer = std::function<PT(T const &lhs, T const &rhs)>;

    template<class T,
             class PT = int,
             class Comp = comparer<T, PT>,
             class Container = std::list<T>,
             bool ReverseComp = false>
    class priority_queue {
    public:
        struct element {
            Container _list;
            int _min_value;
            std::shared_ptr<element> _left{};
            std::shared_ptr<element> _right{};
            element() = default;
            ~element() = default;
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
            T &pop(std::size_t &count) {
                if (ReverseComp) {
                    if (_left) {
                        std::size_t before = count;
                        T &t = _left->pop(count);
                        if (before > count)
                            return t;
                    }
                    if (!_list.empty()) {
                        count--;
                        T &t = _list.front();
                        _list.pop_front();
                        return t;
                    }
                    if (_right) {
                        std::size_t before = count;
                        T &t = _right->pop(count);
                        if (before > count)
                            return t;
                    }
                    return _null;
                }

                // normal
                if (_right) {
                    std::size_t before = count;
                    T &t = _right->pop(count);
                    if (before > count)
                        return t;
                }
                if (!_list.empty()) {
                    count--;
                    T &t = _list.front();
                    _list.pop_front();
                    return t;
                }
                if (_left) {
                    std::size_t before = count;
                    T &t = _left->pop(count);
                    if (before > count)
                        return t;
                }
                return _null;
            }
            T &front() {
                if (ReverseComp) {
                    if (_left) {
                        T &t = _left->front();
                        if (t != _null)
                            return t;
                    }
                    if (!_list.empty()) {
                        return _list.front();
                    }
                    if (_right) {
                        T &t = _right->front();
                        if (t != _null)
                            return t;
                    }
                    return _null;
                }

                // normal
                if (_right) {
                    T &t = _right->front();
                    if (t != _null)
                        return t;
                }
                if (!_list.empty()) {
                    return _list.front();
                }
                if (_left) {
                    T &t = _left->front();
                    if (t != _null)
                        return t;
                }
                return _null;
            }
            static T _null;
        };

    public:
        priority_queue()
            : _root{std::make_shared<element>()}
            , _comparer{} {}
        virtual ~priority_queue() {}

        void push_back(T const &data) { push(data); }
        void pop_front() { _root->pop(_count); }
        T &front() { return _root->front(); }

        void push(T const &data) {
            _push(_root, data);
            _count++;
        }
        T &pop() { return _root->pop(_count); }
        bool empty() const { return _count == 0; }
        static bool is_null(T const &t) { return t == element::_null; }

    public:
        void dump(std::function<void(element *)> const &fn) { ReverseComp ? _root->LNR(fn) : _root->RNL(fn); }

    protected:
        void mid_walk(std::function<void(element *)> const &fn) { _root->LNR(fn); }
        void pot_walk(std::function<void(element *)> const &fn) { _root->rgt_walk(fn); }
        void lft_walk(std::function<void(element *)> const &fn) { _root->LRN(fn); }

    private:
        void _push(std::shared_ptr<element> &at, T const &data) {
            PT l{}, r{};
            for (auto const &v : at->_list) {
                PT ret = _comparer(data, v);
                if (ret < 0) {
                    if (ret < l)
                        l = ret;
                } else if (ret > 0) {
                    if (ret > r)
                        r = ret;
                }
            }

            if (l < 0) {
                if (at->_left) {
                    _push(at->_left, data);
                } else {
                    at->_left = std::make_shared<element>(element{{data}, l});
                }
                return;
            } else if (r > 0) {
                if (at->_right) {
                    _push(at->_right, data);
                } else {
                    at->_right = std::make_shared<element>(element{{data}, r});
                }
            } else {
                at->_list.push_back(data);
            }
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
    inline T priority_queue<T, PT, Comp, Container, ReverseComp>::element::_null{};


    class Matrix {
        std::valarray<int> data;
        int dim;

    public:
        Matrix(int r, int c)
            : data(r * c)
            , dim(c) {}
        int &operator()(int r, int c) { return data[r * dim + c]; }
        [[nodiscard]] int trace() const {
            return data[std::slice(0, dim, dim + 1)].sum();
        }
    };

} // namespace bet

void test_pq() {
    std::list<int> vi;
    // vi.pop_front();

    struct pq_comp {
        std::function<int(std::string const &lhs, std::string const &rhs)> pq_comp = [](std::string const &lhs, std::string const &rhs) -> int {
            if (lhs.substr(0, 4) == "CMD:") {
                if (rhs.substr(0, 4) == "CMD:")
                    return std::less()(lhs, rhs) ? -1 : 1;
            } else {
                if (rhs.substr(0, 4) == "CMD:")
                    return -1;

                return std::less()(lhs, rhs) ? -1 : 1;
            }
            return 0;
        };
        int operator()(std::string const &lhs, std::string const &rhs) const {
            return pq_comp(lhs, rhs);
        }
    };

    bet::priority_queue<std::string, int, pq_comp> pq;
    pq.push("CMD:CONNECT TO");
    pq.push("data:a123");
    pq.push("data:a125");
    pq.push("data:b1");
    pq.push("CMD:SEND");
    pq.push("data:tv1");
    pq.push("data:c3");
    pq.push("CMD:CLOSE");

    std::cout << "DUMP..." << '\n';
    pq.dump([](bet::priority_queue<std::string, int, pq_comp>::element *el) {
        for (auto const &it : el->_list) {
            std::cout << it << ',';
        }
        std::cout << '\n';
    });
    std::cout << "POP..." << '\n';
    while (!pq.empty())
        std::cout << pq.pop() << '\n';


    bet::priority_queue<std::string, int, pq_comp,
                        bet::priority_queue<std::string, int, pq_comp>>
            pq2;
    pq2.push("CMD:CONNECT TO");
    pq2.push("data:a123");
    pq2.push("data:a125");
    pq2.push("data:b1");
    pq2.push("CMD:SEND");
    pq2.push("data:tv1");
    pq2.push("data:c3");
    pq2.push("CMD:CLOSE");
    std::cout << "DUMP..." << '\n';

    pq2.dump([](bet::priority_queue<std::string, int, pq_comp,
                                    bet::priority_queue<std::string, int, pq_comp>>::element *el) {
        for (auto const &it : el->_list) {
            std::cout << it << ',';
        }
        std::cout << '\n';
    });
    std::cout << "POP..." << '\n';
    while (!pq2.empty())
        std::cout << pq2.pop() << '\n';
}

int main() {
    bet::Matrix m(3, 3);
    int n = 0;
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            m(r, c) = ++n;
    std::cout << "Trace of the matrix (1,2,3) (4,5,6) (7,8,9) is " << m.trace() << '\n';

    std::cout << '\n'
              << "queue: ";
    bet::ti_queue<int> tiq;
    tiq.push(31);
    tiq.push(17);
    tiq.push(19);
    tiq.push(73);

    auto tmp = tiq.pop();
    std::cout << tmp;

    while (!tiq.empty()) {
        tmp = tiq.pop();
        std::cout << ',' << tmp;
    }

    std::cout << '\n'
              << '\n';

    test_pq();
}
