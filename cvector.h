#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef unsigned char byte;

typedef struct {
    byte* data;
    size_t size;
    size_t capacity;
    size_t size_of_type;
} CVector;

int CVector_init(CVector* v, size_t size_of_type) {
    if (!v) {
        fprintf(stderr, "Error: CVector_init(): Pointer to CVector is NULL\n");
        return 0;
    }

    v->data = NULL;
    v->size = 0;
    v->capacity = 0;
    v->size_of_type = size_of_type;

    return 1;
}

bool CVector_empty(CVector* v) {
    if (!v) {
        fprintf(stderr, "Error: CVector_empty(): Pointer to CVector is NULL\n");
        return true;
    }
    return v->size == 0;
}

int CVector_reserve(CVector* v, size_t new_capacity) {
    if (!v) {
        fprintf(stderr, "Error: CVector_reserve(): Pointer to CVector is NULL\n");
        return 0;
    }

    if (new_capacity == 0) {
        fprintf(stderr, "Warning: CVector_reserve(): new_capacity is zero, not resizing\n");
        return 1;
    }

    if (new_capacity > v->capacity) {
        byte* new_data = (byte*) malloc(new_capacity * v->size_of_type);
        if (!new_data) {
            fprintf(stderr, "Error: CVector_reserve(): Could not malloc()\n");
            return 0;
        }

        if (v->data) {
            memcpy(new_data, v->data, v->size * v->size_of_type);
            free(v->data);
        }

        v->data = new_data;
        v->capacity = new_capacity;
    }
    return 1;
}

int CVector_resize(CVector* v, size_t new_size) {
    if (!v) {
        fprintf(stderr, "Error: CVector_resize(): Pointer to CVector is NULL\n");
        return 0;
    }

    if (new_size == v->size) {
        fprintf(stderr, "Warning: CVector_resize(): New size is the same as old size, not resizing\n");
        return 1;
    }

    if (new_size > v->size) {
        if (new_size > v->capacity) {
            if (!CVector_reserve(v, new_size)) {
                fprintf(stderr, "Error: CVector_resize(): CVector_reserve() failed\n");
                return 0;
            }
        }
        byte* end = (byte*) v->data + (v->size * v->size_of_type);
        memset(end, 0, (new_size - v->size) * v->size_of_type);
        v->size = new_size;
        return 1;
    }

    if (new_size < v->size) {
        v->size = new_size;
        if (!CVector_shrink_to_fit(v)) {
            fprintf(stderr, "Error: CVector_resize(): CVector_shrink_to_fit() failed\n");
            return 0;
        }
    }

    return 1;
}

int CVector_shrink_to_fit(CVector* v) {
    if (!v) {
        fprintf(stderr, "Error: CVector_shrink_to_fit(): Pointer to CVector is NULL\n");
        return 0;
    }

    if (CVector_empty(v)) {
        return 1;
    }

    if (v->capacity > v->size) {
        byte* new_data = (byte*) malloc(v->size * v->size_of_type);
        if (!new_data) {
            fprintf(stderr, "Error: CVector_shrink_to_fit(): Could not malloc()\n");
            return 0;
        }

        memcpy(new_data, v->data, v->size * v->size_of_type);
        free(v->data);
        v->data = new_data;
        v->capacity = v->size;
    }

    return 1;
}

int CVector_push_back(CVector* v, const void* element) {
    if (!v) {
        fprintf(stderr, "Error: CVector_push_back(): Pointer to CVector is NULL\n");
        return 0;
    }
    if (!element) {
        fprintf(stderr, "Error: CVector_push_back(): Element pointer is NULL\n");
        return 0;
    }

    if (v->size == v->capacity) {
        size_t new_capacity = (v->capacity == 0) ? 1 : v->capacity + (v->capacity / 2);
        if (!CVector_reserve(v, new_capacity)) {
            fprintf(stderr, "Error: CVector_push_back(): Calling CVector_reserve() failed\n");
            return 0;
        }
    }

    byte* dest = (byte*) v->data + (v->size * v->size_of_type);
    memcpy(dest, element, v->size_of_type);
    v->size++;

    return 1;
}

int CVector_pop_back(CVector* v) {
    if (!v) {
        fprintf(stderr, "Error: CVector_pop_back(): Pointer to CVector is NULL\n");
        return 0;
    }

    if (CVector_empty(v)) {
        fprintf(stderr, "Error: CVector_pop_back(): Cannot pop, CVector->size is zero\n");
        return 0;
    }

    byte* last = (byte*) v->data + ((v->size - 1) * v->size_of_type);
    memset(last, 0, v->size_of_type);

    v->size--;

    if (v->size > 0 && v->capacity / v->size >= 2) {
        if (!CVector_shrink_to_fit(v)) {
            fprintf(stderr, "Warning: CVector_pop_back(): CVector_shrink_to_fit() failed\n");
        }
    }

    return 1;
}

byte* CVector_at(CVector* v, size_t i) {
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

byte* CVector_front(CVector* v) {
    if (!v) {
        fprintf(stderr, "Error: CVector_front(): Pointer to CVector is NULL\n");
        return NULL;
    }

    if (CVector_empty(v)) return NULL;

    return (byte*) v->data;
}

byte* CVector_back(CVector* v) {
    if (!v) {
        fprintf(stderr, "Error: CVector_back(): Pointer to CVector is NULL\n");
        return NULL;
    }

    if (CVector_empty(v)) return NULL;

    return (byte*) v->data + ((v->size - 1) * v->size_of_type);
}

int CVector_insert(CVector* v, size_t i, const void* element) {
    if (!v) {
        fprintf(stderr, "Error: CVector_insert(): Pointer to CVector is NULL\n");
        return 0;
    }
    if (!element) {
        fprintf(stderr, "Error: CVector_insert(): Element pointer is NULL\n");
        return 0;
    }

    if (i > v->size) {
        fprintf(stderr, "Error: CVector_insert(): Index out of bounds\n");
        return 0;
    }

    if (v->size >= v->capacity) {
        if (!CVector_reserve(v, v->size + 1)) {
            fprintf(stderr, "Error: CVector_insert(): Insertion failed, CVector_reserve() failed\n");
            return 0;
        }
    }

    byte* place = (byte*) v->data + (i * v->size_of_type);
    byte* dest = (byte*) place + v->size_of_type;

    memcpy(dest, place, (v->size - i) * v->size_of_type);
    memcpy(place, element, v->size_of_type);

    v->size++;

    return 1;
}

int CVector_erase(CVector* v, size_t i) {
    if (!v) {
        fprintf(stderr, "Error: CVector_erase(): Pointer to CVector is NULL\n");
        return 0;
    }

    if (i >= v->size) {
        fprintf(stderr, "Error: CVector_erase(): Index out of bounds\n");
        return 0;
    }

    if (i < v->size - 1) {
        byte* dest = (byte*) v->data + (i * v->size_of_type);
        byte* place = (byte*) dest + v->size_of_type;
        memcpy(dest, place, (v->size - (i + 1)) * v->size_of_type);
    }
    v->size--;

    if (v->size == 0) {
        v->capacity = 0;
        free(v->data);
        v->data = NULL;
    } else if (v->capacity / v->size >= 2) {
        if (!CVector_shrink_to_fit(v)) {
            fprintf(stderr, "Warning: CVector_erase(): CVector_shrink_to_fit() failed\n");
        }
    }

    return 1;
}

int CVector_clear(CVector* v) {
    if (!v) {
        fprintf(stderr, "Error: CVector_clear(): Pointer to CVector is NULL\n");
        return 0;
    }

    if (CVector_empty(v)) {
        fprintf(stderr, "Warning: CVector_clear(): Data empty, not cleaning\n");
        return 1;
    }

    memset(v->data, 0, v->size * v->size_of_type);
    v->size = 0;

    return 1;
}

void CVector_destroy(CVector* v) {
    if (!v) {
        fprintf(stderr, "Warning: CVector_destroy(): Pointer to CVector is NULL\n");
        return;
    }

    free(v->data);
}
