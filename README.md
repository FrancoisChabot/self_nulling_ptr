# self_nulling_ptr

A safer alternative to `this` for callbacks.

## In a nutshell

```
#include "self_nulling_ptr.h"

void perform_async_operation(std::function<void(int)>);

class my_class : public slt::enable_self_nulling_ptr<my_class> {
  int status_;
public:
  void foo() {
    perform_async_operation([self = get_self_nulling_ptr()](int x) {
      if(self) { // If I still exist.
        self->status_ = x;
      }
    });
  }

};
```

## The problem

The following is a pattern that I find myself using more and more:
```
void perform_async_operation(std::function<void(int)>);

class some_class {
  public:
    void foo() {
      perform_async_operation([this](int result) {
        data_ = result;
      });
    }

  private:
    int data_;
};
```

Unfortunately, I found myself struggling to find a satisfying way to ensure
`some_class` doesn't get deleted before the operation is complete. In some cases, `some_class` can be assumed to be handled through shared ownership (see ASIO's examples for a demonstration of this), but that's not always the case.

## Thread safety

Copying and passing around self-nulling pointers can be done in a thread-safe manner.

However, dereferencing a self-nulling pointer is **not thread-safe**. If you are going to access the object pointed to by a self-nulling pointer in a different thread than the one who deletes the underlying object, you are responsible for the synchronization.

That being said, creating a fully thread-safe version of this should be feasible (with a slightly altered API). If you would use such a thing, feel free to file a GitHub issue.
