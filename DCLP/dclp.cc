#include "dclp.h"

DCLP *DCLP::pIns_;
std::atomic<DCLP *> DCLP::apIns_;
std::weak_ptr<DCLP> DCLP::wpIns_;
#if __cplusplus >= 202002L
std::atomic<std::weak_ptr<DCLP>> DCLP::awpIns_;
#endif
std::mutex DCLP::mux_;

DCLP::DCLP() {
  usleep(rand() % 10);
  printf("construct:%p\n", this);
}

DCLP::~DCLP() {
  usleep(rand() % 10);
  printf("deconstruct:%p\n", this);
}

DCLP &DCLP::GetInstance_static() {
  static DCLP ins;
  return ins;
}

// DCLP* DCLP::GetInstance_wrong(){
//   DCLP* ret = pIns_;
//   if(!pIns_){
//     std::lock_guard<std::mutex> lock(mux_);
//     ret = pIns_;
//     if(!pIns_){
//       ret = new DCLP;
//       pIns_ = ret;
//     }
//   }
//   return ret;
// }

DCLP *DCLP::GetInstance() {
  DCLP *ret = apIns_.load(std::memory_order_acquire);
  if (!ret) {
    std::lock_guard<std::mutex> lock(mux_);
    ret = apIns_.load(std::memory_order_relaxed);
    if (!ret) {
      ret = new DCLP;
      apIns_.store(ret, std::memory_order_release);
    }
  }
  return ret;
}

DCLP *DCLP::GetInstance2() {
  DCLP *ret = apIns_.load(std::memory_order_relaxed);
  // According to this fence, no memory operation after this sentences will
  // be rearranged ahead of any memory read before this sentences.
  std::atomic_thread_fence(std::memory_order_acquire);
  if (!ret) {
    std::lock_guard<std::mutex> lock(mux_);
    ret = apIns_.load(std::memory_order_relaxed);
    if (!ret) {
      ret = new DCLP;
      // According to this fence, no memory operation before this sentences
      // will be rearranged behind of any memory write after this sentences.
      std::atomic_thread_fence(std::memory_order_release);
      apIns_.store(ret, std::memory_order_relaxed);
    }
  }
  return ret;
}

std::shared_ptr<DCLP> DCLP::GetInstance3() {
  std::lock_guard<std::mutex> guard(mux_);
  std::shared_ptr<DCLP> ret = wpIns_.lock();
  if (!ret) {
    ret = std::make_shared<DCLP>();
    wpIns_ = ret;
  }
  return ret;
}

#if __cplusplus >= 202002L
std::shared_ptr<DCLP> DCLP::GetInstance4() {
  std::shared_ptr<DCLP> ret = awpIns_.load(std::memory_order_acquire).lock();
  if (!ret) {
    std::lock_guard<std::mutex> lock(mux_);
    ret = awpIns_.load(std::memory_order_relaxed).lock();
    if (!ret) {
      ret = std::make_shared<DCLP>();
      std::weak_ptr<DCLP> tmp = ret;
      // or save with std::memory_order_release.
      awpIns_.exchange(tmp, std::memory_order_release);
    }
  }
  return ret;
}
#endif

// Error implementation: wpIns_.lock() and wpIns_.swap() is not atomic
// std::shared_ptr<DCLP> DCLP::GetInstance(){
//   std::shared_ptr<DCLP> ret = wpIns_.lock();
//   std::atomic_thread_fence(std::memory_order_acquire);
//   if(!ret){
//     std::lock_guard<std::mutex> guard(mux_);
//     ret = wpIns_.lock();
//     if(!ret){
//       ret = std::make_shared<DCLP>();
//       std::weak_ptr<DCLP> tmp = ret;
//       std::atomic_thread_fence(std::memory_order_release);
//       wpIns_.swap(tmp); // is swap atomic, compare to construct
//     }
//   }
//   return ret;
// }

void DCLP::member() {
  usleep(rand() % 997);
  printf("this is :%p\n", this);
}