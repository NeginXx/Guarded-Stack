#include "stack_trace.h"

const size_t callstack_max_size = (size_t)1e8;

struct CallStack {
  struct FuncLine* ptr_;
  size_t size_;
  size_t capacity_;
  size_t mincapacity_;
  size_t incfac_;
} funcs_called = {NULL, 0, 100, 100, 2};

size_t CallStackExpand();
void CallStackShrink();
size_t CallStackPushback(struct FuncLine func_line);
size_t CallStackPopback();
void CallStackDestroy();

void FuncsCalledPush(struct FuncLine func_line) {
  static int cnt = 0;
  if (cnt == 0) {
    funcs_called.ptr_ = (struct FuncLine*)calloc(funcs_called.capacity_, sizeof(struct FuncLine));
    if (funcs_called.ptr_ == NULL) {
      printf("Can not use stack trace, no more memory for stack\n");
    }
  }

	CallStackPushback(func_line);
  ++cnt;
}

void FuncsCalledPop() {
	CallStackPopback();
}

struct FuncLine* FuncsCalledGetValues(size_t* num) {
  if (funcs_called.ptr_ == NULL) {
    return NULL;
  }

	size_t sz = funcs_called.size_;

	if (*num > sz) {
		*num = sz;
	}

	struct FuncLine* arr = (struct FuncLine*)calloc(*num, sizeof(int));
	assert(arr != NULL || !"Calloc failed in FuncsCalledGetValues");

	for (int i = 0; i < *num; i++) {
		arr[i] = (struct FuncLine){(funcs_called.ptr_[i]).func_name_, (funcs_called.ptr_[i]).line_};
	}

	return arr;
}

void segv_handler(int signum) {
  printf("caught segv :^(\n");
  size_t n = 20;
  struct FuncLine* arr = FuncsCalledGetValues(&n);
  if (arr == NULL) {
    printf("0 called functions in stack\n");
    exit(3);
  }

  printf("%lu called functions in stack:\n", n);
  for (int i = 0; i < n; i++) {
    printf("%d line: %lu, function name: %s\n", i + 1, arr[i].line_, arr[i].func_name_);
  }

  CallStackDestroy();
  exit(3);
}

size_t CallStackExpand() {
  if (funcs_called.ptr_ == NULL) {
    return 1;
  }

  if (funcs_called.capacity_ * funcs_called.incfac_ > callstack_max_size) {
    printf("Callstack_max_size reached\n");
    return 1;
  }

  funcs_called.ptr_ = realloc(funcs_called.ptr_, funcs_called.capacity_ * funcs_called.incfac_ * sizeof(struct FuncLine));

  if (funcs_called.ptr_ == NULL) {
    printf("No more memory for called functions stack\n");
    return 1;
  }

  funcs_called.capacity_ *= funcs_called.incfac_;

  return 0;
}

void CallStackShrink() {
  if (funcs_called.ptr_ == NULL) {
    return;
  }

  funcs_called.ptr_ = realloc(funcs_called.ptr_, funcs_called.capacity_ / funcs_called.incfac_ * sizeof(struct FuncLine));

  funcs_called.capacity_ /= funcs_called.incfac_;
}

size_t CallStackPushback(struct FuncLine func_line) {
  if (funcs_called.ptr_ == NULL) {
    return 0;
  }

  if (funcs_called.size_ >= callstack_max_size) {
    printf("Callstack_max_size reached\n");
    return 0;
  }

  if (funcs_called.size_ == funcs_called.capacity_) {
    if (CallStackExpand(funcs_called) != 0) {
      return 0;
    }
  }
  funcs_called.ptr_[funcs_called.size_++] = func_line;

  return 1;
}

size_t CallStackPopback() {
  if (funcs_called.size_ == 0 || funcs_called.ptr_ == NULL) {
    return 0;
  }

  funcs_called.ptr_[--funcs_called.size_] = (struct FuncLine){NULL, 0};
  if (funcs_called.size_ <= funcs_called.capacity_ / (size_t)(pow(funcs_called.incfac_, 2))) {
    CallStackShrink(funcs_called);
  }

  return 1;
}

void CallStackDestroy() {
  if (funcs_called.ptr_ == NULL) {
    return;
  }

  free(funcs_called.ptr_);
}