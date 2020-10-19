#pragma once
#include <assert.h>
#include <ctype.h>
#include <execinfo.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

struct CallStack;
struct FuncLine {
  const char* func_name_;
  size_t line_;
};

#define $ FuncsCalledPush((struct FuncLine){__func__, __LINE__})
#define $$ FuncsCalledPop()

void FuncsCalledPush();

void FuncsCalledPop();

struct FuncLine* FuncsCalledGetValues(size_t* num);

void segv_handler(int signum);