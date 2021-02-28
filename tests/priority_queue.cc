//
// Created by Hedzr Yeh on 2021/2/28.
//

#include "cmdr11/cmdr_utils.hh"


void test_pq() {
    std::list<int> vi;
    // vi.pop_front();

    struct pq_comp {
        std::function<int(std::string const &lhs, std::string const &rhs)> _comp = [](std::string const &lhs, std::string const &rhs) -> int {
          if (lhs.substr(0, 4) == "CMD:") {
              if (rhs.substr(0, 4) == "CMD:")
                  return 0; // return std::less()(lhs, rhs) ? -1 : 1;
          } else {
              if (rhs.substr(0, 4) == "CMD:")
                  return -1;

              return std::less()(lhs, rhs) ? -1 : 1;
          }
          return 0;
        };
        int operator()(std::string const &lhs, std::string const &rhs) const {
            return _comp(lhs, rhs);
        }
    };

    cmdr::util::priority_queue<std::string, int, pq_comp> pq;
    pq.push("CMD:CONNECT TO");
    pq.push("data:a123");
    pq.push("data:a125");
    pq.push("data:b1");
    pq.push("CMD:SEND");
    pq.push("data:tv1");
    pq.push("data:c3");
    pq.push("CMD:CLOSE");

    std::cout << "DUMP..." << '\n';
    pq.dump([](cmdr::util::priority_queue<std::string, int, pq_comp>::element *el) {
      for (auto const &it : el->_list) {
          std::cout << it << ',';
      }
      std::cout << '\n';
    });
    std::cout << "ITER..." << '\n';
    for (auto &it : pq) {
        std::cout << it << '\n';
    }
    std::cout << "POP..." << '\n';
    while (!pq.empty())
        std::cout << pq.pop() << '\n';

#if 1
    std::cout << '\n';
    std::cout << '\n';
    std::cout << "Multi-level..." << '\n';
    cmdr::util::priority_queue<std::string, int, pq_comp,
            cmdr::util::priority_queue<std::string, int, pq_comp>>
            pq2;
    pq2.push("CMD:CONNECT TO");
    pq2.push("data:a123");
    pq2.push("data:a125");
    pq2.push("data:b1");
    pq2.push("CMD:SEND");
    pq2.push("data:tv1");
    pq2.push("data:c3");
    pq2.push("CMD:CLOSE");

    std::cout << "- DUMP..." << '\n';
    pq2.dump([](cmdr::util::priority_queue<std::string, int, pq_comp,
            cmdr::util::priority_queue<std::string, int, pq_comp>>::element *el) {
        for (auto const &it : el->_list) {
            std::cout << it << ',';
        }
        std::cout << '\n';
    });
    std::cout << "- POP..." << '\n';
    while (!pq2.empty())
        std::cout << pq2.pop() << '\n';
#endif
}

int main() {
    test_pq();
}
