#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <time.h>
#include <unistd.h>

// DCLP reference:
// Source of this problem:
//   https://www.aristeia.com/Papers/DDJ_Jul_Aug_2004_revised.pdf
//   https://www.drdobbs.com/cpp/c-and-the-perils-of-double-checked-locki/184405726
//   https://www.drdobbs.com/cpp/c-and-the-perils-of-double-checked-locki/184405772
// Preshing answered:
//   https://preshing.com/20130930/double-checked-locking-is-fixed-in-cpp11/

class DCLP {
private:
  static DCLP *pIns_;
  static std::atomic<DCLP *> apIns_;
  static std::weak_ptr<DCLP> wpIns_;
#if __cplusplus >= 202002L
  // Reference:
  // c++20, atomic weak_ptr support:
  //   Only MSVC: 19.27* support for now.(2020-09)
  //   http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0718r2.html
  //   https://en.cppreference.com/w/cpp/compiler_support
  //   https://en.cppreference.com/w/cpp/memory/weak_ptr/atomic2
  //
  // c++11, atomic shared_ptr support:
  //   https://en.cppreference.com/w/cpp/memory/shared_ptr/atomic
  //   https://www.modernescpp.com/index.php/atomic-smart-pointers
  static std::atomic<std::weak_ptr<DCLP>> awpIns_;
#endif
  static std::mutex mux_;

public:
  DCLP();
  ~DCLP();
  DCLP &GetInstance_static();
  static DCLP *GetInstance();
  static DCLP *GetInstance2();
  static std::shared_ptr<DCLP> GetInstance3();
#if __cplusplus >= 202002L
  static std::shared_ptr<DCLP> GetInstance4();
#endif
  void member();
};