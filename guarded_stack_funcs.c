#include "guarded_stack_header.h"
const size_t stack_max_size = (size_t)1e8;

size_t min(size_t a, size_t b) {
	return (a < b ? a : b);
}

void VectorPrintInfo(struct Vector* vec) {
	$;
  assert(vec != NULL);

  #ifdef MYDEBUG
    printf("struct vector canaries: %lu %lu\n", vec->can0, vec->can1);
  #endif

  printf("pointer on array: %p\n", vec->ptr_);
  if (vec->ptr_ == NULL) {
    return;
  }

  printf("size: %lu\ncapacity_: %lu\nmincapacity_: %lu\nincfac_: %lu\n",
         vec->size_, vec->capacity_, vec->mincapacity_, vec->incfac_);
  Type* arr = vec->ptr_;

  #ifdef MYDEBUG
    void* can0_ptr = (void*)arr - 8;
    void* can1_ptr = (void*)(arr + vec->capacity_);
    printf("array canaries values: %lu %lu\n", *(size_t*)can0_ptr,
                                               *(size_t*)can1_ptr);
  #endif

  printf("array values: \n");
  for (int i = 0; i < vec->capacity_; i++) {
    printf("%d %lf\n", i, arr[i]);
  }
  $$;
}

void StackDump(struct Stack* stck) {
	$;
	assert(stck != NULL);
	printf("something went wrong with stack\nStackDump is called\n");
	printf("name of stack: %s\n", stck->name_);
	printf("error code %d: ", stck->err_);

	switch (stck->err_) {
		case NO_ERROR:
			printf("NO_ERROR\n");
			break;
		case SIZE_LIMIT:
			printf("SIZE_LIMIT\n");
			break;
		case STACK_EMPTY:
			printf("STACK_EMPTY\n");
			break;
		case STACK_ARRAY_NULL:
			printf("STACK_ARRAY_NULL\n");
			break;
		case OUT_OF_RANGE:
			printf("OUT_OF_RANGE\n");
			break;
		case VECTOR_INCFAC0:
			printf("INCREASE_FACTOR_0\n");
			break;
		case STACK_ARRAY_PTR_NULL:
			printf("STACK_ARRAY_PTR_NULL\n");
			break;
		case STACK_CAN0_CHANGE:
			printf("STACK_CAN0_CHANGE\n");
			break;
		case STACK_CAN1_CHANGE:
			printf("STACK_CAN0_CHANGE\n");
			break;
		case VECTOR_CAN0_CHANGE:
			printf("VECTOR_CAN0_CHANGE\n");
			break;
		case VECTOR_CAN1_CHANGE:
			printf("VECTOR_CAN1_CHANGE\n");
			break;
		case STACK_HASH_CHANGE:
			printf("STACK_HASH_CHANGE\n");
		default:
			printf("No info about error code :^(\n");
	}
	printf("stack status: %d\n", stck->status_);
	printf("stack canaries values: %lu %lu\n", stck->can0, stck->can1);

	#ifdef MYSUPERDEBUG
		printf("stack hash value: %lu\n", stck->hash_);
	#endif

	printf("\nVector info:\n");
	VectorPrintInfo(stck->array_);
	$$;
}

bool VectorOk(struct Stack* stck) {
	$;
	assert(stck != NULL);
	assert(stck->array_ != NULL);
	if (stck->array_->can0 != VECTOR_CAN0) {
		stck->err_ = VECTOR_CAN0_CHANGE;
		return false;
	}
	if (stck->array_->can1 != VECTOR_CAN1) {
		stck->err_ = VECTOR_CAN1_CHANGE;
		return false;
	}
	if (stck->array_->ptr_ == NULL) {
		stck->err_ = STACK_ARRAY_PTR_NULL;
		return false;
	}
	if (stck->array_->size_ > stck->array_->capacity_) {
		stck->err_ = OUT_OF_RANGE;
		return false;
	}
	if (stck->array_->mincapacity_ > stack_max_size) {
		stck->err_ = OUT_OF_RANGE;
		return false;
	}
	if (stck->array_->incfac_ == 0) {
		stck->err_ = VECTOR_INCFAC0;
		return false;
	}
	if (stck->array_->capacity_ > stack_max_size) {
		stck->err_ = SIZE_LIMIT;
		return false;
	}
	$$;
	return true;
}

bool StackOk(struct Stack* stck) {
	$;
	assert(stck != NULL);

	if (stck->array_ == NULL) {
		stck->err_ = STACK_ARRAY_NULL;
		return false;
	}

	if (!VectorOk(stck)) {
		return false;
	}

	if (stck->can0 != STACK_CAN0) {
		stck->err_ = STACK_CAN0_CHANGE;
		return false;
	}

	if (stck->can1 != STACK_CAN1) {
		stck->err_ = STACK_CAN1_CHANGE;
		return false;
	}

	#ifdef MYSUPERDEBUG
		size_t temp = stck->hash_;
		if ((stck->hash_ = StackHashCalc(stck)) != temp) {
			stck->err_ = STACK_HASH_CHANGE;
			return false;
		}
	#endif

	$$;
	return true;
}

size_t HashCalc(void* begin, void* end) {
	$;
	assert(begin != NULL);
	assert(end != NULL);
	size_t h = 0;
	for (int i = 0; begin + i < end; i++) {
	  h = h * 123 + *((char*)begin + i);
	}
	$$;
	return h;
}


size_t StackHashCalc(struct Stack* stck) {
	$;
	stck->hash_ = 0;
	$$;
	return HashCalc(stck, stck + 1) + HashCalc(stck->array_, stck->array_ + 1) + 
				 HashCalc(stck->array_->ptr_, stck->array_->ptr_ + stck->array_->capacity_);
}

void VectorFillPoison(struct Vector* vec) {
	$;
	assert(vec != NULL);
	for (int i = vec->size_; i < vec->capacity_; i++) {
		vec->ptr_[i] = POISON;
	}
	$$;
}

struct Stack* StackConstruct(struct Stack* stck, size_t initsize, char* name) {
	$;
	assert(stck != NULL);
	assert(initsize < stack_max_size);
	assert(name != NULL);

	#ifdef MYDEBUG
		stck->can0 = STACK_CAN0;
		stck->can1 = STACK_CAN1;
		stck->err_ = NO_ERROR;
		stck->name_ = (char*)calloc(20, sizeof(char));
		if (name != NULL) {
			strcpy(stck->name_, name);
		} else {
			strcpy(stck->name_, "NO_NAME");
		}
	#endif

	stck->status_ = STACK_CONSTRUCTED;
	stck->array_ = NewVector(initsize);
	assert(stck->array_ != NULL || !"No more memory for stack array");

	#ifdef MYLITTLEDEBUG
		VectorFillPoison(stck->array_);
	#endif

	#ifdef MYSUPERDEBUG
		stck->hash_ = StackHashCalc(stck);
	#endif

	ASSERT_STACK(stck);
	$$;
}

struct Stack* NewStack(size_t initsize) {
	$;
	assert(initsize < stack_max_size);

	struct Stack* stck = (struct Stack*)calloc(1, sizeof(struct Stack));
	assert(stck != NULL || !"No more memory for stack");

	#ifdef MYDEBUG
		stck->can0 = STACK_CAN0;
		stck->can1 = STACK_CAN1;
		stck->err_ = NO_ERROR;
	#endif

	stck->status_ = STACK_CONSTRUCTED;
	stck->array_ = NewVector(initsize);
	assert(stck->array_ != NULL || !"No more memory for stack array");

	#ifdef MYLITTLEDEBUG
		VectorFillPoison(stck->array_);
	#endif

	#ifdef MYSUPERDEBUG
		stck->hash_ = StackHashCalc(stck);
	#endif

	ASSERT_STACK(stck);
	$$;
	return stck;
}

int StackPush(struct Stack* stck, Type val) {
	$;
	ASSERT_STACK(stck);
	ASSERT_CONSTRUCTED(stck);

	size_t curcapacity = stck->array_->capacity_;
	if (VectorPushback(stck->array_, val) != 1) {
		#ifdef MYLITTLEDEBUG
			stck->err_ = PUSH_FAILED;
		#endif
		return 0;
	}
	#ifdef MYLITTLEDEBUG
		if (stck->array_->capacity_ > curcapacity) {
			VectorFillPoison(stck->array_);
		}
	#endif

	#ifdef MYSUPERDEBUG
		stck->hash_ = StackHashCalc(stck);
	#endif

	ASSERT_STACK(stck);
	$$;
	return 1;
}

int StackPop(struct Stack* stck) {
	$;
	ASSERT_STACK(stck);
	ASSERT_CONSTRUCTED(stck);
	if (stck->array_->size_ == 0) {
		return 0;
	}
	
	size_t curcapacity = stck->array_->capacity_;
	VectorPopback(stck->array_);
	#ifdef MYLITTLEDEBUG
	if (stck->array_->capacity_ > curcapacity) {
		VectorFillPoison(stck->array_);
	}
	#endif

	#ifdef MYSUPERDEBUG
		stck->hash_ = StackHashCalc(stck);
	#endif

	ASSERT_STACK(stck);
	$$;
	return 1;
}

Type StackTop(struct Stack* stck) {
	$;
	ASSERT_STACK(stck);
	ASSERT_CONSTRUCTED(stck);

	if (stck->array_->size_ == 0) {
		return POISON;
	}

	#ifdef MYSUPERDEBUG
		stck->hash_ = StackHashCalc(stck);
	#endif

	$$;
	return stck->array_->ptr_[stck->array_->size_ - 1];
}

void StackClear(struct Stack* stck) {
	$;
	ASSERT_STACK(stck);
	ASSERT_CONSTRUCTED(stck);

	#ifdef MYDEBUG
		stck->err_ = NO_ERROR;
	#endif

	VectorClear(stck->array_);

	#ifdef MYLITTLEDEBUG
		VectorFillPoison(stck->array_);
	#endif

	#ifdef MYSUPERDEBUG
		stck->hash_ = StackHashCalc(stck);
	#endif

	ASSERT_STACK(stck);
	$$;
}

void StackDestroy(struct Stack* stck) {
	$;
	ASSERT_STACK(stck);
	ASSERT_CONSTRUCTED(stck);
	VectorDestroy(stck->array_);
	free(stck);
	$$;
}

size_t GetSize(struct Stack* stck) {
	$;
	ASSERT_STACK(stck);
	ASSERT_CONSTRUCTED(stck);
	#ifdef MYSUPERDEBUG
		stck->hash_ = StackHashCalc(stck);
	#endif
	$$;
	return stck->array_->size_;
}

int main() {
	$;
	signal(SIGSEGV, segv_handler);
	struct Stack stackk = {};
	STACK_CONSTRUCT(&stackk, 50);
	for (int i = 0; i < 400; i++) {
		StackPush(&stackk, i);
	}
	for (int i = 0; i < 100; i++) {
		StackPop(&stackk);
	}
	/*int* arr = (int*)calloc(5, sizeof(int));
	for (int i = 0; i < 5; i++) {
		arr[i] = i;
	}
	memcpy(stackk.array_, arr, 10);*/
	*(long long*)&stackk.array_->ptr_ = 5;
	printf("%lf\n", StackTop(&stackk));
	$$;
	return 0;
}