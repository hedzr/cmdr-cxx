//
// Created by Hedzr Yeh on 2021/1/15.
//

#ifndef CMDR_CXX11_CUSTOM_ITERATOR_HH
#define CMDR_CXX11_CUSTOM_ITERATOR_HH

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <vector>


namespace cmdr::util {

#if __clang__

    template<class T>
    class iterator_traits {
    public:
        typedef iterator_traits self_type;
        typedef T value_type;
        typedef T &reference;
        typedef const T &const_reference;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef std::forward_iterator_tag iterator_category;
        typedef int difference_type;

        bool operator==(const self_type &rhs) { return ptr_ == rhs.ptr_; }

        bool operator!=(const self_type &rhs) { return ptr_ != rhs.ptr_; }

        friend bool operator==(const self_type &a, const self_type &b) { return a.ptr_ == b.ptr_; };

        friend bool operator!=(const self_type &a, const self_type &b) { return a.ptr_ != b.ptr_; };

        explicit iterator_traits(pointer ptr)
            : ptr_(ptr) {}

    protected:
        pointer ptr_;
    };
    
    
    // https://gist.github.com/jeetsukumaran/307264
    //
    // More:
    // - [c++ - Writing your own STL Container - Stack Overflow](https://stackoverflow.com/questions/7758580/writing-your-own-stl-container/7759622#7759622)
    // - [c++ - How to implement an STL-style iterator and avoid common pitfalls? - Stack Overflow](https://stackoverflow.com/questions/8054273/how-to-implement-an-stl-style-iterator-and-avoid-common-pitfalls)
    //
    template<typename T>
    class fixed_array {
    public:
        // typedef typename iteratorizer<T, fixed_array<T>>::size_type size_type;
        // typedef typename iteratorizer<T, fixed_array<T>>::iterator iterator;
        // typedef typename iteratorizer<T, fixed_array<T>>::const_iterator const_iterator;

        typedef size_t size_type;

        class iterator : public iterator_traits<T> {
        public:
            using parent = iterator_traits<T>;

            explicit iterator(T *ptr)
                : parent(ptr) {}

            iterator operator++(int) {
                iterator i = *this;
                this->ptr_++;
                return i;
            }

            iterator &operator++() {
                this->ptr_++;
                return *this;
            }

            T &operator*() { return *this->ptr_; }

            T *operator->() { return this->ptr_; }
        };

        class const_iterator : public iterator_traits<T> {
        public:
            using parent = iterator_traits<T>;

            explicit const_iterator(T *ptr)
                : parent(ptr) {}

            const_iterator operator++(int) {
                iterator i = *this;
                this->ptr_++;
                return i;
            }

            const_iterator &operator++() {
                this->ptr_++;
                return *this;
            }

            T &operator*() { return *this->ptr_; }

            T *operator->() { return this->ptr_; }
        };

        // https://stackoverflow.com/questions/6742008/what-are-the-typical-use-cases-of-an-iterator-trait
        template<class Ty>
        struct iterator_traits {
            typedef typename Ty::value_type value_type;
            typedef typename Ty::difference_type difference_type;
            typedef typename Ty::iterator_category iterator_category;
            typedef typename Ty::pointer pointer;
            typedef typename Ty::reference reference;
        };
        template<typename Ty>
        struct iterator_traits<Ty *> {
            typedef std::random_access_iterator_tag iterator_category;
            typedef Ty value_type;
            typedef Ty *pointer;
            typedef Ty &reference;
            typedef std::ptrdiff_t difference_type;
        };
        template<typename Ty>
        struct iterator_traits<const Ty *> {
            typedef std::random_access_iterator_tag iterator_category;
            typedef Ty value_type;
            typedef ptrdiff_t difference_type;
            typedef const Ty *pointer;
            typedef const Ty &reference;
        };

        // https://elloop.github.io/c++/2015-12-29/learning-using-stl-19-iterator-traits
        template<typename Con>
        class asso_inserter_iterator
            : public std::iterator<std::output_iterator_tag, typename Con::value_type> {
        public:
            explicit asso_inserter_iterator(Con &con)
                : conRef_(con) {}

            asso_inserter_iterator<Con> &operator=(const typename Con::value_type &val) {
                conRef_.insert(val);
                return *this;
            }

            asso_inserter_iterator<Con> &operator*() { return *this; }

            asso_inserter_iterator<Con> &operator++() { return *this; }

            asso_inserter_iterator<Con> &operator++(int) { return *this; }

        protected:
            Con &conRef_;
        };

        // convenience function to create asso_inserter_iterator.
        template<typename Con>
        inline asso_inserter_iterator<Con> asso_inserter(Con &con) {
            return asso_inserter_iterator<Con>(con);
        }

        explicit fixed_array(size_type size)
            : _size(size) {
            _data = new T[_size];
        }

        [[nodiscard]] size_type size() const { return _size; }

        T &operator[](size_type index) {
            CMDR_ASSERT(index < _size);
            return _data[index];
        }

        const T &operator[](size_type index) const {
            CMDR_ASSERT(index < _size);
            return _data[index];
        }

        iterator begin() {
            return iterator(_data);
        }

        iterator end() {
            return iterator(_data + _size);
        }

        [[nodiscard]] const_iterator begin() const {
            return const_iterator(_data);
        }

        [[nodiscard]] const_iterator end() const {
            return const_iterator(_data + _size);
        }

    private:
        T *_data;
        size_type _size;

    public:
        static int main() {
            fixed_array<double> point3d(3);
            point3d[0] = 2.3;
            point3d[1] = 3.2;
            point3d[2] = 4.2;

            for (auto it = point3d.begin(); it != point3d.end(); it++) {
                auto val = *it;
                std::cout << val << '/' << *it << " ";
            }

            for (auto &it : point3d) {
                auto val = it;
                std::cout << val << '/' << it << " ";
            }

            std::cout << '\n';

            std::vector<double> vec;
            std::copy(point3d.begin(), point3d.end(), std::back_inserter(vec));

            for (std::vector<double>::iterator i = vec.begin(); i != vec.end(); i++) {
                std::cout << *i << " ";
            }

            std::cout << '\n';
            return 0;
        }
    };
    
#endif
    
} // namespace cmdr::util

#endif //CMDR_CXX11_CUSTOM_ITERATOR_HH
