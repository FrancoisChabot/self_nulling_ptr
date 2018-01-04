#include "self_nulling_ptr.h"

#include <functional>
#include <iostream>
#include <queue>

std::queue<std::function<void(int)>> queue_;

void queue_delayed(std::function<void(int)> cb) {
  queue_.emplace(std::move(cb));
}

class my_class : public slt::enable_self_nulling_ptr<my_class> {
 public:
  my_class() {
    queue_delayed([ref = get_self_nulling_ptr()](int v) {
      if (ref) {
        std::cout << "passed " << v << std::endl;
        ref->value = v;
      } else {
        std::cout << "saved " << v << std::endl;
      }
    });
  }

 private:
  int value = 0;
};

int main() {
  my_class a;
  queue_.front()(12);
  queue_.pop();
  { my_class b; }
  queue_.front()(5);
  queue_.pop();

  return 0;
}
