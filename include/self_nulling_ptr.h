#ifndef SLT_SELF_NULLING_PTR_INCLUDED_H
#define SLT_SELF_NULLING_PTR_INCLUDED_H

#include <atomic>

namespace slt {

template <typename T>
struct self_nulling_ptr_storage_ {
  T* ptr;
  std::atomic<int> count{1};
};

template <typename T>
class self_nulling_ptr {
 public:
  self_nulling_ptr(self_nulling_ptr const& rhs) : storage_(rhs.storage_) {
    if (storage_) {
      storage_->count++;
    }
  }

  self_nulling_ptr(self_nulling_ptr&& rhs) : storage_(rhs.storage_) {
    rhs.storage_ = nullptr;
  }

  self_nulling_ptr& operator=(self_nulling_ptr const& rhs) {
    storage_ = rhs.storage_;
    if (storage_) {
      storage_->count++;
    }
  }

  self_nulling_ptr& operator=(self_nulling_ptr&& rhs) {
    storage_ = rhs.storage_;
    rhs.storage_ = nullptr;
  }

  ~self_nulling_ptr() {
    if (storage_) {
      if (storage_->count-- == 0) {
        delete storage_;
      }
    }
  }

  operator bool() const { return storage_->ptr != nullptr; }
  T* operator->() const { return storage_->ptr; }
  T& operator*() const { return *storage_->ptr; }

  T* get() const { return storage_->ptr; }

 private:
  template <typename CRTP>
  friend class enable_self_nulling_ptr;

  self_nulling_ptr(self_nulling_ptr_storage_<T>* storage) : storage_(storage) {
    storage->count++;
  }
  self_nulling_ptr_storage_<T>* storage_;
};

template <typename CRTP>
class enable_self_nulling_ptr {
 public:
  enable_self_nulling_ptr() = default;

  // Returns a self-nulling pointer to this object.
  self_nulling_ptr<CRTP> get_self_nulling_ptr() {
    if (!weak_data_) {
      weak_data_ = new self_nulling_ptr_storage_<CRTP>;
      weak_data_->ptr = static_cast<CRTP*>(this);
    }

    return self_nulling_ptr<CRTP>(weak_data_);
  }

  // explicitely do nothing:
  // N.B. It could be possible to handle move construction/assignment
  // but doing so in a thread-safe manner would be very messy and
  // probably not worth the trouble.
  enable_self_nulling_ptr(enable_self_nulling_ptr&& rhs) {}
  enable_self_nulling_ptr& operator=(enable_self_nulling_ptr&& rhs) {}
  enable_self_nulling_ptr(enable_self_nulling_ptr const& rhs) {}
  enable_self_nulling_ptr& operator=(enable_self_nulling_ptr const&) {}

  // On destruction,
  // If there are nullable pointers in existence, set the storage to null
  // Otherwise delete the storage.
  ~enable_self_nulling_ptr() {
    if (weak_data_) {
      if (weak_data_->count-- == 0) {
        delete weak_data_;
      } else {
        weak_data_->ptr = nullptr;
      }
    }
  }




 private:
  self_nulling_ptr_storage_<CRTP>* weak_data_ = nullptr;
};
}
#endif
