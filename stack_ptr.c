#include <gc/gc.h>
#include <stdio.h>
#include <stdlib.h>

void alloc() {
  void* p = GC_malloc(3);
  // Let the pointer be on the stack.
  asm(""::"m"(p));
}

void main_loop(void* p) {
  size_t before_gc = GC_get_free_bytes();
  GC_gcollect();
  size_t after_gc = GC_get_free_bytes();
  printf("%s: %zu => %zu\n",
         before_gc < after_gc ? "reclaimed" : "not reclaimed",
         before_gc, after_gc);
}

void call_main_loop() {
  // The stack location used in alloc() is not used in this function.
  void* p;
  // The allocated memory will be properly reclaimed even without our
  // clang plugin if we have this line.
  // p = NULL;
  asm(""::"m"(p));
  main_loop(p);
}

int main() {
  GC_init();
  GC_gcollect();

  alloc();
  call_main_loop();
  return 0;
}
