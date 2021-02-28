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
            T _data{};
            element *_last{};
            element *_next{};
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
}
