#ifndef _DS_ORDERED_ARRAY_
#define _DS_ORDERED_ARRAY_

#include <libk/memlayout.h>

/*
 * This array is insertion sorted - it always remains in a sorted state (between calls).
 * It can store anything that can be cast to a void* -- so a int32_t, or any pointer.
 */
typedef void* type_t;

/* https://stackoverflow.com/questions/12662891/how-can-i-pass-a-member-function-where-a-free-function-is-expected */
typedef char (*lessthan_predicate_t)(type_t, type_t);


/* A standard less than predicate. */
int8_t standardLessthanPredicate(type_t a, type_t b);

class OrderedArray {
    private:
        type_t *array;
        int32_t size;
        int32_t max_size;
        lessthan_predicate_t less_than;
    public:
        OrderedArray() {}
        OrderedArray(int32_t max_size);
        OrderedArray(int32_t max_size, lessthan_predicate_t less_than);
        /* Only used when kmalloc is not working */
        OrderedArray(virtual_addr *addr, int32_t max_size, lessthan_predicate_t less_than);
        /* Deletes the item at location i from the list. */
        void removeAtIndex(int32_t i);
        /* Returns Node at index i */
        type_t findAtIndex(int32_t i);
        /* Add an item into the list. */
        void insertNode(type_t node);
        ~OrderedArray() {}
};

#endif  // _DS_ORDERED_ARRAY_