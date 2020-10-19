#include "vector_header.h"
const size_t vector_max_size = 1e8;

char* VectorGetStatus(struct Vector* vec) {
  assert(vec != NULL);
  switch(vec->status_) {
    case VECTOR_CONSTRUCTED_CSTR:
      return "VECTOR_CONSTRUCTED_CSTR";
    case VECTOR_CONSTRUCTED_NEW:
      return "VECTOR_CONSTRUCTED_NEW";
    case VECTOR_DESTROYED:
      return "VECTOR_DESTROYED";
    default:
      return "VECTOR_NOT_CONSTRUCTED";
  }
}

struct Vector* NewVector(size_t initsize) {
  assert(initsize <= vector_max_size);
  struct Vector* vec = (struct Vector*)calloc(1, sizeof(struct Vector));
  vec->mincapacity_ = 100;
  vec->status_ = VECTOR_CONSTRUCTED_NEW;
  if (initsize < vec->mincapacity_) {
    initsize = vec->mincapacity_;
  }

  if (vec != NULL) {
    #ifdef MYDEBUG
      vec->can0 = VECTOR_CAN0;
      vec->can1 = VECTOR_CAN1;
    #endif

    #ifdef MYDEBUG
      vec->ptr_ = (Type*)calloc(1, 8 + initsize * sizeof(Type) + 8);
      *(size_t*)vec->ptr_ = vec->can0;
      *(size_t*)((void*)vec->ptr_ + initsize * sizeof(Type) + 8) = vec->can1;
      vec->ptr_ = (Type*)((void*)vec->ptr_ + 8);
    #else
      vec->ptr_ = (Type*)calloc(initsize, sizeof(Type));
    #endif

    vec->size_ = 0;
    vec->capacity_ = initsize;
    vec->incfac_ = 2;
  }
  return vec;
}

void VectorConstruct(struct Vector* vec, size_t initsize) {
  assert(vec != NULL);
  assert(initsize <= vector_max_size);
  vec->mincapacity_ = 100;
  vec->status_ = VECTOR_CONSTRUCTED_CSTR;
  if (initsize < vec->mincapacity_) {
    initsize = vec->mincapacity_;
  }

  #ifdef MYDEBUG
    vec->can0 = VECTOR_CAN0;
    vec->can1 = VECTOR_CAN1;
  #endif

  #ifdef MYDEBUG
    vec->ptr_ = (Type*)calloc(1, 8 + initsize * sizeof(Type) + 8);
    *(size_t*)vec->ptr_ = vec->can0;
    *(size_t*)((void*)vec->ptr_ + initsize * sizeof(Type) + 8) = vec->can1;
    vec->ptr_ = (Type*)((void*)vec->ptr_ + 8);
  #else
    vec->ptr_ = (Type*)calloc(initsize, sizeof(Type));
  #endif

  assert(vec->ptr_ != NULL || !"No more memory for vector\n");
  vec->size_ = 0;
  vec->capacity_ = initsize;
  vec->incfac_ = 2;
}

// if return value is VECTOR_REALLOC_FAILED, than ran out of space
size_t VectorExpand(struct Vector* vec) {
  assert(vec != NULL);
  assert(vec->ptr_ != NULL);
  if (vec->capacity_ * vec->incfac_ > vector_max_size) {
    return VECTOR_SIZE_LIMIT;
  }

  #ifdef MYDEBUG
    vec->ptr_ = (Type*)((void*)vec->ptr_ - 8);
    vec->ptr_ = (Type*)realloc(vec->ptr_, 8 + vec->capacity_ * vec->incfac_ * sizeof(Type) + 8);
    if (vec->ptr_ != NULL) {
      vec->ptr_ = (Type*)((void*)vec->ptr_ + 8);
      *(size_t*)(vec->ptr_ + vec->capacity_) = 0;
    }
  #else
    vec->ptr_ = realloc(vec->ptr_, vec->capacity_ * vec->incfac_ * sizeof(Type));
  #endif

  if (vec->ptr_ == NULL) {
    return VECTOR_REALLOC_FAILED;
  }

  vec->capacity_ *= vec->incfac_;

  #ifdef MYDEBUG
    *(size_t*)(vec->ptr_ + vec->capacity_) = vec->can1;
  #endif

  return 0;
}

void VectorShrink(struct Vector* vec) {
  assert(vec != NULL);
  assert(vec->ptr_ != NULL);

  #ifdef MYDEBUG
    vec->ptr_ = (Type*)((void*)vec->ptr_ - 8);
    vec->ptr_ = realloc(vec->ptr_, 8 + vec->capacity_ / vec->incfac_ * sizeof(Type) + 8);
    vec->ptr_ = (Type*)((void*)vec->ptr_ + 8);
  #else
    vec->ptr_ = (Type*)realloc(vec->ptr_, vec->capacity_ / vec->incfac_ * sizeof(Type));
  #endif

  vec->capacity_ /= vec->incfac_;

  #ifdef MYDEBUG
    *(size_t*)(vec->ptr_ + vec->capacity_) = vec->can1;
  #endif
}

size_t VectorPushback(struct Vector* vec, Type val) {
  //$;
  assert(vec != NULL);
  assert(vec->ptr_ != NULL);
  assert(vec->size_ < vector_max_size);
  if (vec->size_ == vec->capacity_) {
    if (VectorExpand(vec) != 0) {
      return 0;
    }
  }
  vec->ptr_[vec->size_++] = val;
  //$$;
  return 1;
}

size_t VectorPopback(struct Vector* vec) {
  ///$;
  assert(vec != NULL);
  assert(vec->ptr_ != NULL);
  if (vec->size_ == 0) {
    return 0;
  }
  vec->ptr_[--vec->size_] = 0;
  if (vec->size_ <= vec->capacity_ / (size_t)(pow(vec->incfac_, 2))) {
    VectorShrink(vec);
  }
  //$$;
  return 1;
}

size_t VectorGetSize(struct Vector* vec) {
  //$;
  assert(vec != NULL);
  //$$;
  return vec->size_;
}

void VectorClear(struct Vector* vec) {
  assert(vec != NULL);
  assert(vec->ptr_ != NULL);

  #ifdef MYDEBUG
    vec->ptr_ = (Type*)((void*)vec->ptr_ - 8);
    vec->ptr_ = realloc(vec->ptr_, 8 + vec->mincapacity_ * sizeof(Type) + 8);
    vec->ptr_ = (Type*)((void*)vec->ptr_ + 8);
  #else
    vec->ptr_ = (Type*)realloc(vec->ptr_, vec->mincapacity_ * sizeof(Type));
  #endif

  vec->size_ = 0;
  vec->capacity_ = vec->mincapacity_;

  #ifdef MYDEBUG
  *(size_t*)(vec->ptr_ + vec->capacity_) = vec->can1;
  #endif

  for (int i = 0; i < vec->mincapacity_; i++) {
    vec->ptr_[i] = 0;
  }
}

void VectorDestroy(struct Vector* vec) {
  assert(vec != NULL);

  #ifdef MYDEBUG
    vec->ptr_ = (Type*)((void*)vec->ptr_ - 8);
  #endif

  free(vec->ptr_);

  if (vec->status_ == VECTOR_CONSTRUCTED_NEW) {
    free(vec);
  }
  vec->status_ = VECTOR_DESTROYED;
}