#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef unsigned char byte;

static const int CVECTOR_GROWTH_FACTOR = 2;
static const int CVECTOR_CAPACITY_SIZE_RATIO = 4;

typedef struct
{
  byte* data;
  size_t size;
  size_t capacity;
  size_t size_of_type;
} CVector;

bool CVector_init(CVector* v, size_t size_of_type);
bool CVector_empty(const CVector* v);
bool CVector_reserve(CVector* v, size_t new_capacity);
bool CVector_resize(CVector* v, size_t new_size);
bool CVector_shrink_to_fit(CVector* v);
bool CVector_push_back(CVector* v, const void* element);
bool CVector_pop_back(CVector* v);
const byte* CVector_at(const CVector* v, size_t i);
const byte* CVector_front(const CVector* v);
const byte* CVector_back(const CVector* v);
bool CVector_insert(CVector* v, size_t i, const void* element);
bool CVector_erase(CVector* v, size_t i);
bool CVector_clear(CVector* v);
void CVector_destroy(CVector* v);

// TODO
const byte* CVector_begin(const CVector* v);
const byte* CVector_end(const CVector* v);
const byte* CVector_next(const CVector* v, const byte* iter);
bool CVector_iter_equal(const byte* iter1, const byte* iter2);

bool CVector_init(CVector* v, size_t size_of_type)
{
    if (!v) {
        fprintf(stderr, "Error: CVector_init(): Pointer to CVector is NULL\n");
        return false;
    }

    v->data = NULL;
    v->size = 0;
    v->capacity = 0;
    v->size_of_type = size_of_type;

    return true;
}

bool CVector_empty(const CVector* v)
{
    return !v || v->size == 0;
}

bool CVector_reserve(CVector* v, size_t new_capacity)
{
    if (!v) {
        fprintf(stderr, "Error: CVector_reserve(): Pointer to CVector is NULL\n");
        return false;
    }

    if (new_capacity == 0) {
        fprintf(stderr, "Warning: CVector_reserve(): new_capacity is zero, not resizing\n");
        return true;
    }

    if (new_capacity > v->capacity) {
        byte* new_data_buffer = (byte*) malloc(new_capacity * v->size_of_type);
        if (!new_data_buffer) {
            fprintf(stderr, "Error: CVector_reserve(): Could not malloc()\n");
            return false;
        }

        if (v->data) {
            memcpy(new_data_buffer, v->data, v->size * v->size_of_type);
            free(v->data);
        }

        v->data = new_data_buffer;
        v->capacity = new_capacity;
    }
    return true;
}

bool CVector_resize(CVector* v, size_t new_size)
{
    if (!v) {
        fprintf(stderr, "Error: CVector_resize(): Pointer to CVector is NULL\n");
        return false;
    }

    if (new_size == v->size) {
        return true;
    }

    if (new_size > v->size) {
        if (new_size > v->capacity) {
            if (!CVector_reserve(v, new_size)) {
                fprintf(stderr, "Error: CVector_resize(): CVector_reserve() failed\n");
                return false;
            }
        }
        byte* end = (byte*) v->data + (v->size * v->size_of_type);
        memset(end, 0, (new_size - v->size) * v->size_of_type);
        v->size = new_size;
        return true;
    }

    if (new_size < v->size) {
        v->size = new_size;
        if (!CVector_shrink_to_fit(v)) {
            fprintf(stderr, "Error: CVector_resize(): CVector_shrink_to_fit() failed\n");
            return false;
        }
    }

    return true;
}

bool CVector_shrink_to_fit(CVector* v)
{
    if (!v) {
        fprintf(stderr, "Error: CVector_shrink_to_fit(): Pointer to CVector is NULL\n");
        return false;
    }

    if (CVector_empty(v)) {
        return true;
    }

    if (v->capacity > v->size) {
        byte* new_data_buffer = (byte*) malloc(v->size * v->size_of_type);
        if (!new_data_buffer) {
            fprintf(stderr, "Error: CVector_shrink_to_fit(): Could not malloc()\n");
            return false;
        }

        memcpy(new_data_buffer, v->data, v->size * v->size_of_type);
        free(v->data);
        v->data = new_data_buffer;
        v->capacity = v->size;
    }

    return true;
}

bool CVector_push_back(CVector* v, const void* element)
{
    if (!v) {
        fprintf(stderr, "Error: CVector_push_back(): Pointer to CVector is NULL\n");
        return false;
    }
    if (!element) {
        fprintf(stderr, "Error: CVector_push_back(): Element pointer is NULL\n");
        return false;
    }

    if (v->size == v->capacity) {
        size_t new_capacity = (v->capacity == 0) ? 1 : v->capacity * CVECTOR_GROWTH_FACTOR;
	if (new_capacity < v->capacity) {
	  fprintf(stderr, "Error: CVector_push_back(): Capacity overflow detected\n");
	  return false;
	}

        if (!CVector_reserve(v, new_capacity)) {
            fprintf(stderr, "Error: CVector_push_back(): Calling CVector_reserve() failed\n");
            return false;
        }
    }

    byte* dest = (byte*) v->data + (v->size * v->size_of_type);
    memcpy(dest, element, v->size_of_type);
    v->size++;

    return true;
}

bool CVector_pop_back(CVector* v)
{
    if (!v) {
        fprintf(stderr, "Error: CVector_pop_back(): Pointer to CVector is NULL\n");
        return false;
    }

    if (CVector_empty(v)) {
        fprintf(stderr, "Error: CVector_pop_back(): Cannot pop, CVector->size is zero\n");
        return false;
    }

    v->size--;

    if (v->size > 0 && v->capacity / v->size >= CVECTOR_CAPACITY_SIZE_RATIO) {
        if (!CVector_shrink_to_fit(v)) {
            fprintf(stderr, "Warning: CVector_pop_back(): CVector_shrink_to_fit() failed\n");
        }
    }

    return true;
}

const byte* CVector_at(const CVector* v, size_t i)
{
    if (!v) {
        fprintf(stderr, "Error: CVector_at(): Pointer to CVector is NULL\n");
        return NULL;
    }

    if (i >= v->size) {
        fprintf(stderr, "Error: CVector_at(): Index out of bounds\n");
        return NULL;
    }

    return (byte*) v->data + (i * v->size_of_type);
}

const byte* CVector_front(const CVector* v)
{
    if (!v) {
        fprintf(stderr, "Error: CVector_front(): Pointer to CVector is NULL\n");
        return NULL;
    }

    if (CVector_empty(v)) return NULL;

    return (byte*) v->data;
}

const byte* CVector_back(const CVector* v)
{
    if (!v) {
        fprintf(stderr, "Error: CVector_back(): Pointer to CVector is NULL\n");
        return NULL;
    }

    if (CVector_empty(v)) return NULL;

    return (byte*) v->data + ((v->size - 1) * v->size_of_type);
}

bool CVector_insert(CVector* v, size_t i, const void* element)
{
    if (!v) {
        fprintf(stderr, "Error: CVector_insert(): Pointer to CVector is NULL\n");
        return false;
    }
    if (!element) {
        fprintf(stderr, "Error: CVector_insert(): Element pointer is NULL\n");
        return false;
    }

    if (i == v->size) {
      if (!CVector_push_back(v, element)) {
	fprintf(stderr, "Error: CVector_insert(): CVector_push_back() failed\n");
	return false;
      }
      return true;
    }

    if (i > v->size) {
        fprintf(stderr, "Error: CVector_insert(): Index out of bounds\n");
        return false;
    }

    if (v->size >= v->capacity) {
        if (!CVector_reserve(v, v->size + 1)) {
            fprintf(stderr, "Error: CVector_insert(): Insertion failed, CVector_reserve() failed\n");
            return false;
        }
    }

    byte* place = (byte*) v->data + (i * v->size_of_type);
    byte* dest = (byte*) place + v->size_of_type;

    memmove(dest, place, (v->size - i) * v->size_of_type);
    memcpy(place, element, v->size_of_type);

    v->size++;

    return true;
}

bool CVector_erase(CVector* v, size_t i)
{
    if (!v) {
        fprintf(stderr, "Error: CVector_erase(): Pointer to CVector is NULL\n");
        return false;
    }

    if (i >= v->size) {
        fprintf(stderr, "Error: CVector_erase(): Index out of bounds\n");
        return false;
    }

    if (i < v->size - 1) {
        byte* dest = (byte*) v->data + (i * v->size_of_type);
        byte* place = (byte*) dest + v->size_of_type;
        memcpy(dest, place, (v->size - (i + 1)) * v->size_of_type);
    }
    v->size--;

    if (v->capacity / v->size > CVECTOR_CAPACITY_SIZE_RATIO) {
        if (!CVector_shrink_to_fit(v)) {
            fprintf(stderr, "Warning: CVector_erase(): CVector_shrink_to_fit() failed\n");
        }
    }

    return true;
}

bool CVector_clear(CVector* v)
{
    if (!v) {
        fprintf(stderr, "Error: CVector_clear(): Pointer to CVector is NULL\n");
        return false;
    }

    if (CVector_empty(v)) return true;

    v->size = 0;

    return true;
}

void CVector_destroy(CVector* v)
{
    if (!v) {
        fprintf(stderr, "Warning: CVector_destroy(): Pointer to CVector is NULL\n");
        return;
    }

    if (v->data) free(v->data);
    v->data = NULL;
    v->size = 0;
    v->capacity = 0;
}
