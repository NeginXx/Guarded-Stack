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

#include "vector_header.h"
#include "stack_trace.h"

#define MYSUPERDEBUG

#ifdef MYSUPERDEBUG
	#define MYDEBUG
	#define MYLITTLEDEBUG
#endif

#ifdef MYDEBUG
	#define MYLITTLEDEBUG
#endif

#ifdef MYDEBUG
	#define STACK_CAN0 0x812A948EA88DDAED
	#define STACK_CAN1 0x100E9D8ABAB124DA
#endif

#ifdef MYDEBUG
	#define ASSERT_STACK(stack) if (!StackOk(stack)) {StackDump(stack); assert(!"OK");}
#else
	#define ASSERT_STACK(stack)
#endif

#define STACK_CONSTRUCT(stack, initsize) StackConstruct(stack, initsize, #stack)
#define ASSERT_CONSTRUCTED(stack) assert(stck->status_ == STACK_CONSTRUCTED || !"Invalid operation, stack is not constructed")
#define POISON NAN
extern const size_t stack_max_size;

enum stack_errors {
	NO_ERROR = 0,
	SIZE_LIMIT = 101,
	STACK_EMPTY = 102,
	STACK_ARRAY_NULL = 103,
	STACK_ARRAY_PTR_NULL = 104,
	OUT_OF_RANGE = 105,
	VECTOR_INCFAC0 = 106,
	STACK_CAN0_CHANGE = 108,
	STACK_CAN1_CHANGE = 109,
	VECTOR_CAN0_CHANGE = 110,
	VECTOR_CAN1_CHANGE = 111,
	STACK_HASH_CHANGE = 112,
	PUSH_FAILED = 113
};
enum stack_status {
	STACK_NOT_CONSTRUCTED = 0,
	STACK_CONSTRUCTED = 1,
	STACK_DESTROYED = 2
};

struct Stack {
	#ifdef MYDEBUG
		size_t can0;
		enum stack_errors err_;
		char* name_;
	#endif

	enum stack_status status_;
	struct Vector* array_;

	#ifdef MYSUPERDEBUG
		size_t hash_;
	#endif
		
	#ifdef MYDEBUG
		size_t can1;
	#endif
};

size_t min(size_t a, size_t b);

void segv_handler(int signum);

void VectorPrintInfo(struct Vector* vec);

#ifdef MYDEBUG
void StackDump(struct Stack* stck);

bool VectorOk(struct Stack* stck);

bool StackOk(struct Stack* stck);
#endif

#ifdef MYLITTLEDEBUG
void VectorFillNan(struct Vector* vec);
#endif

struct Stack* NewStack(size_t initsize);

int StackPush(struct Stack* stck, Type val);

int StackPop(struct Stack* stck);

Type StackTop(struct Stack* stck);

void StackClear(struct Stack* stck);

void StackDestroy(struct Stack* stck);

size_t GetSize(struct Stack* stck);

size_t StackHashCalc(struct Stack* stck);

size_t HashCalc(void* begin, void* end);