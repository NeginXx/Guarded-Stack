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
#define MYDEBUG
#define Type double

extern const size_t vector_max_size;

enum vector_status {
	VECTOR_NOT_CONSTRUCTED,
	VECTOR_CONSTRUCTED_CSTR,
	VECTOR_CONSTRUCTED_NEW,
	VECTOR_DESTROYED
};
enum vector_errors {
  VECTOR_SIZE_LIMIT = 101,
  VECTOR_EMPTY = 102,
  VECTOR_PTR_NULL = 103,
  VECTOR_OUT_OF_RANGE = 104,
  VECTOR_REALLOC_FAILED = 105
};

#ifdef MYDEBUG
	#define VECTOR_CAN0 0x6F84AA10FB934DEE
	#define VECTOR_CAN1 0x88CBD9122C4C27F4
#endif

struct Vector {
	#ifdef MYDEBUG
		size_t can0;
	#endif

	enum vector_status status_;
	Type* ptr_;
	size_t size_;
	size_t capacity_;
	size_t mincapacity_;
	size_t incfac_;
	
	#ifdef MYDEBUG
		size_t can1;
	#endif
};

char* VectorGetStatus(struct Vector* vec);
void VectorPrintInfo(struct Vector* vec);
struct Vector* NewVector(size_t initsize);
void VectorConstruct(struct Vector* vec, size_t initsize);
size_t VectorExpand(struct Vector* vec);
void VectorShrink(struct Vector* vec);
size_t VectorPushback(struct Vector* vec, Type val);
size_t VectorPopback(struct Vector* vec);
size_t VectorGetSize(struct Vector* vec);
void VectorClear(struct Vector* vec);
void VectorDestroy(struct Vector* vec);