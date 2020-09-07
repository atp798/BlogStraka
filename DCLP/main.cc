#include "dclp.h"
#include <thread>

// Compile with command: g++ -std=c++11 -o dclp.11 main.cc dclp.cc -lpthread
// Or with c++2a standard: g++ -std=c++2a -o dclp.2a main.cc dclp.cc -lpthread

#define DYNAMIC_SINGLETON

void func() {
  int max = 0;
  for (int u = 0; u < 10; u++) {
    {
#ifdef DYNAMIC_SINGLETON
      auto b = DCLP::GetInstance3();
#else
      auto b = DCLP::GetInstance();
#endif

#ifdef DYNAMIC_SINGLETON
      b->member();
      if (max < b.use_count()) {
        max = b.use_count();
      }
#endif
    }
    // Do not move this into the block, otherwise you won't see deconstruct
    usleep(rand() % 100);
  }
  printf("use count:%d\n", max);
}

int main() {
  srand(time(NULL));

  const int num = 4;
  std::thread threads[num];
  for (int i = 0; i < num; ++i) {
    threads[i] = std::thread(func);
  }
  for (int i = 0; i < num; ++i) {
    threads[i].join();
  }
  return 0;
}