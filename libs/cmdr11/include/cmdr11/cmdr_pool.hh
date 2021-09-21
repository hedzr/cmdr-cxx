//
// Created by Hedzr Yeh on 2021/7/13.
//

#ifndef CMDR_CXX11_POOL_HH
#define CMDR_CXX11_POOL_HH

#if !defined(CMDR_ENABLE_THREAD_POOL_READY_SIGNAL)
#define CMDR_ENABLE_THREAD_POOL_READY_SIGNAL 1
#endif

// To enable debugging output for thread pool, adding this definition in your cmake script:
// -DCMDR_TEST_THREAD_POOL_DBGOUT=1

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <thread>

#include <functional>
#include <optional>
#include <queue>
#include <random>
#include <string>
#include <vector>

#include "cmdr_defs.hh"
// #include "cmdr_ringbuf.hh"
#include "cmdr_log.hh"

#if CMDR_TEST_THREAD_POOL_DBGOUT
#define pool_debug cmdr_print
#else
#define pool_debug cmdr_trace
#endif

namespace cmdr::pool {

    /**
     * @brief a wrapper class for using std condition variable concisely 
     * @tparam T any type holder
     * @tparam Pred a functor with prototype `bool()`
     * @tparam Setter a functor with prototype `void()`
     * @see cmdr::pool::conditional_wait_for_bool
     * @see cmdr::pool::conditional_wait_for_int
     */
    template<typename T, typename Pred = std::function<bool()>, typename Setter = std::function<void()>>
    class conditional_wait {
        Pred _p{};
        Setter _s{};

    protected:
        std::condition_variable _cv{};
        std::mutex _m{};
        T _var{};

    public:
        explicit conditional_wait(Pred &&p_, Setter &&s_)
            : _p(std::move(p_))
            , _s(std::move(s_)) {}
        virtual ~conditional_wait() { clear(); }
        // conditional_wait(conditional_wait &&) = delete;
        // conditional_wait &operator=(conditional_wait &&) = delete;
        CLAZZ_NON_COPYABLE(conditional_wait);

    public:
        /**
         * @brief wait for Pred condition matched
         */
        void wait() {
            std::unique_lock<std::mutex> lk(_m);
            _cv.wait(lk, _p);
        }
        /**
         * @brief wait for Pred condition matched, or a timeout arrived.
         * @tparam R 
         * @tparam P 
         * @param time a timeout (std::chrono::duration)
         * @return true if condition matched, false while not matched
         */
        template<class R, class P>
        bool wait_for(std::chrono::duration<R, P> const &time) {
            std::unique_lock<std::mutex> lk(_m);
            return _cv.wait_for(lk, time, _p);
        }
        const bool ConditionMatched = true;
        /**
         * @brief do Setter, and trigger any one of the wating routines
         */
        void set() {
            // cmdr_debug("%s", __FUNCTION_NAME__);
            {
                std::unique_lock<std::mutex> lk(_m);
                _s();
            }
            _cv.notify_one();
        }
        /**
         * @brief do Setter, trigger and wake up all waiting routines
         */
        void set_for_all() {
            {
                std::unique_lock<std::mutex> lk(_m);
                _s();
            }
            _cv.notify_all();
        }
        void clear() { _release(); }
        T const &val() const { return _value(); }
        T &val() { return _value(); }

    protected:
        virtual T const &_value() const { return _var; }
        virtual T &_value() { return _var; }
        virtual void _release() {}
    };

    template<typename CW>
    class cw_setter {
    public:
        cw_setter(CW &cw)
            : _cw(cw) {}
        ~cw_setter() { _cw.set(); }

    private:
        CW &_cw;
    };

    class conditional_wait_for_bool : public conditional_wait<bool> {
    public:
        conditional_wait_for_bool()
            : conditional_wait([this]() { return _wait(); }, [this]() { _set(); }) {}
        virtual ~conditional_wait_for_bool() = default;
        conditional_wait_for_bool(conditional_wait_for_bool &&) = delete;
        conditional_wait_for_bool &operator=(conditional_wait_for_bool &&) = delete;

    protected:
        bool _wait() const { return _var; }
        void _set() { _var = true; }

    public:
        void kill() { set_for_all(); }
    };

    class conditional_wait_for_int : public conditional_wait<int> {
    public:
        conditional_wait_for_int(int max_value_ = 1)
            : conditional_wait([this]() { return _wait(); }, [this]() { _set(); })
            , _max_value(max_value_) {}
        virtual ~conditional_wait_for_int() = default;
        conditional_wait_for_int(conditional_wait_for_int &&) = delete;
        conditional_wait_for_int &operator=(conditional_wait_for_int &&) = delete;

        inline int max_val() const { return _max_value; }

    protected:
        inline bool _wait() const { return _var >= _max_value; }
        inline void _set() { _var++; }

    private:
        int _max_value;
    };

} // namespace cmdr::pool

namespace cmdr::pool {
    /**
     * @brief helper class for shutdown a sleep loop gracefully.
     * 
     * @details Sample:
     * @code{c++}
     * class X {
     *     std::thread _t;
     *     cmdr::pool::timer_killer _tk;
     *     static void runner(timer *_this) {
     *         using namespace std::literals::chrono_literals;
     *         auto d = 10ns;
     *         while (!_this->_tk.wait_for(d)) {
     *             // std::this_thread::sleep_for(d);
     *             std::this_thread::yield();
     *         }
     *         cmdr_trace("timer::runner ended.");
     *     }
     *     void start(){ _t.detach(); }
     *   public:
     *     X(): _t(std::thread(runner, this)) { start(); }
     *     ~X(){ stop(); }
     *     void stop() {  _tk.kill(); }
     * };
     * @endcode
     */
    class timer_killer : public conditional_wait_for_bool {
        // bool _terminate = false;
        // mutable std::condition_variable _cv{};
        // mutable std::mutex _m{};

        CLAZZ_NON_COPYABLE(timer_killer);
        // timer_killer(timer_killer &&) = delete;
        // timer_killer(timer_killer const &) = delete;
        // timer_killer &operator=(timer_killer &&) = delete;
        // timer_killer &operator=(timer_killer const &) = delete;

    public:
        timer_killer() = default;
        virtual ~timer_killer() = default;
        // returns false if killed:
        // template<class R, class P>
        // bool wait_for(std::chrono::duration<R, P> const &time) const {
        //     std::unique_lock<std::mutex> lock(_m);
        //     return !_cv.wait_for(lock, time, [&] { return _terminate; });
        // }
        bool terminated() const { return val(); }
        /**
         * @brief wait for Pred condition matched, or a timeout arrived.
         * @tparam R 
         * @tparam P 
         * @param time a timeout (std::chrono::duration)
         * @return true if condition matched, false while not matched
         */
        template<class R, class P>
        bool wait_for(std::chrono::duration<R, P> const &time) {
            return conditional_wait_for_bool::wait_for(time);
        }
        void set() {
            // cmdr_debug("%s", __FUNCTION_NAME__);
            conditional_wait_for_bool::set();
        }
        void set_for_all() {
            // cmdr_debug("%s", __FUNCTION_NAME__);
            conditional_wait_for_bool::set_for_all();
        }

        // void kill() {
        //     bool go{false};
        //     {
        //         std::unique_lock<std::mutex> lock(_m);
        //         if (!_terminate) {
        //             go = _terminate = true; // should be modified inside mutex lock
        //         }
        //     }
        //     if (go)
        //         _cv.notify_all(); // it is safe, and *sometimes* optimal, to do this outside the lock}
        // }
    };

} // namespace cmdr::pool

namespace cmdr::pool {

    template<class T, class Coll = std::deque<T>>
    class threaded_message_queue {
    public:
        using locker = std::unique_lock<std::mutex>;
        void emplace_back(T &&t) {
            {
                locker l_(_m);
                _data.emplace_back(std::move(t));
            }
            _cv.notify_one();
        }
        // void push_back(T const &t) {
        //     {
        //         locker l_(_m);
        //         _data.push_back(t);
        //     }
        //     _cv.notify_one();
        // }
        // void push_back_bad(T t) {
        //     {
        //         locker l(_m);
        //         _data.push_back(std::move(t));
        //     }
        //     _cv.notify_one();
        // }

        inline std::optional<T> pop_front() {
            std::optional<T> ret;
            {
                locker l_(_m);
                _cv.wait(l_, [this] { return _abort || !_data.empty(); });
                if (_abort) {
                    pool_debug("pop_front, aborting");
                    return ret; // std::nullopt;
                }
                pool_debug("pop_front, wake up and got task");
                ret.emplace(std::move(_data.back()));
                _data.pop_back();
            }
            std::this_thread::yield();
            return ret;
        }
        // inline std::optional<T> pop_front_bad() {
        //     locker l(_m);
        //     _cv.wait(l, [this] { return _abort || !_data.empty(); });
        //     if (_abort) return {}; // std::nullopt;
        //
        //     auto r = std::move(_data.back());
        //     _data.pop_back();
        //     return r;
        // }

        void clear() {
            {
                locker l_(_m);
                _abort = true;
                _data.clear();
            }
            _cv.notify_all();
        }
        ~threaded_message_queue() { clear(); }

        bool empty() const { return _data.empty(); }

    private:
        std::condition_variable _cv{};
        std::mutex _m{};
        mutable Coll _data{};
        bool _abort = false;
    }; // class threaded_message_queue

    /**
     * @brief a c++11 thread pool with pre-created, fixed running threads and free tasks management.
     * 
     * @par Each thread will try to lock the task queue and fetch the newest one for launching.
     * 
     * @par This pool was inspired by one or two posts at stackoverflow, the original link needed.
     */
    class thread_pool {
    public:
        thread_pool(int n = 1u)
#if CMDR_ENABLE_THREAD_POOL_READY_SIGNAL
            : _cv_started((int) (n > 0 ? n : std::thread::hardware_concurrency()))
#endif
        {
            start_thread((n > 0 ? n : std::thread::hardware_concurrency()));
        }
        // thread_pool(thread_pool &&) = delete;
        // thread_pool &operator=(thread_pool &&) = delete;
        CLAZZ_NON_COPYABLE(thread_pool);
        ~thread_pool() { join(); }

    public:
        template<class F, class R = std::result_of_t<F &()>>
        std::future<R> queue_task(F &&task) {
            auto p = std::packaged_task<R()>(std::forward<F>(task));
            // std::packaged_task<R()> p(std::move(task));
            auto r = p.get_future();
            // _tasks.push_back(std::move(p));
            _tasks.emplace_back(std::move(p));
            pool_debug("queue_task.");
            std::this_thread::yield();
            return r;
        }
        template<class F, class R = std::result_of_t<F &()>>
        std::future<R> queue_task(F const &task) {
            auto p = std::packaged_task<R()>(std::forward<F>(task));
            // std::packaged_task<R()> p(std::move(task));
            auto r = p.get_future();
            // _tasks.push_back(std::move(p));
            _tasks.emplace_back(std::move(p));
            pool_debug("queue_task (copy).");
            std::this_thread::yield();
            return r;
        }
        void join() { clear_threads(); }
        std::size_t active_threads() const { return _active; }
        std::size_t total_threads() const { return _threads.size(); }
        auto &tasks() { return _tasks; }
        auto const &tasks() const { return _tasks; }

    private:
        template<class F, class R = std::result_of_t<F &()>>
        std::future<R> run_task(F &&task) {
            if (active_threads() >= total_threads()) {
                start_thread();
            }
            return queue_task(std::move(task));
        }
        void clear_threads() {
            _tasks.clear();
            _future_ended.wait();
            _threads.clear();
        }
        void start_thread(std::size_t n = 1) {
            while (n-- > 0) {
                _threads.push_back(
                        std::async(std::launch::async,
                                   [&
#if CMDR_TEST_THREAD_POOL_DBGOUT
                                    ,
                                    n
#endif
                ] {
                                       cw_setter cws(_future_ended);
#if CMDR_ENABLE_THREAD_POOL_READY_SIGNAL
                                       _cv_started.set();
#endif
#if CMDR_TEST_THREAD_POOL_DBGOUT
                                       pool_debug("  . pool.n = %lu..", n);
#endif
                                       while (auto task = _tasks.pop_front()) {
                                           pool_debug("got_task.");
                                           ++_active;
                                           try {
                                               (*task)();
                                           } catch (...) {
                                               --_active;
                                               throw;
                                           }
                                           --_active;
                                       }
                                   }));
            }
#if CMDR_ENABLE_THREAD_POOL_READY_SIGNAL
            _cv_started.wait();
            pool_debug("  . pool.started (cv.get = %d)..", _cv_started.val());
#else
            pool_debug("  . pool.started..");
#endif
        }

    private:
        std::vector<std::future<void>> _threads{};                           // fixed, running pool
        mutable threaded_message_queue<std::packaged_task<void()>> _tasks{}; // the futures
        std::atomic<std::size_t> _active{0};
#if CMDR_ENABLE_THREAD_POOL_READY_SIGNAL
        conditional_wait_for_int _cv_started{};
#endif
        conditional_wait_for_int _future_ended{};
    }; // class thread_pool

} // namespace cmdr::pool

#endif //CMDR_CXX11_POOL_HH
