#ifndef _DS_ORDERED_ARRAY_
#define _DS_ORDERED_ARRAY_

#include <libk/memlayout.h>
#include <stddef.h>

/*
 * This array is insertion sorted - it always remains in a sorted state (between calls).
 * It can store anything that can be cast to a void* -- so a int32_t, or any pointer.
 */
typedef void* type_t;

/* https://stackoverflow.com/questions/12662891/how-can-i-pass-a-member-function-where-a-free-function-is-expected */
typedef uint8_t (*lessthan_predicate_t)(type_t, type_t);


/* A standard less than predicate. */
uint8_t standardLessthanPredicate(type_t a, type_t b);

class OrderedArray {
    private:
        type_t *array;
        size_t size;
        size_t max_size;
        lessthan_predicate_t less_than;
    public:
        /* Only used when kmalloc available. It uses the standard lessthan predicate
         * @param max_size The maximum size of ordered array
         */
        OrderedArray(size_t max_size);
        /* Only used when kmalloc available
         * @param max_size The maximum size of ordered array
         * @param less_than A user supplied lessthan predicate
         */
        OrderedArray(size_t max_size, lessthan_predicate_t less_than);
        /* Only used when kmalloc is not working 
         * @param addr The address where the heap must be located
         * @param max_size The maximum size of ordered array
         * @param less_than A user supplied lessthan predicate
         */
        OrderedArray(virtual_addr *addr, size_t max_size, lessthan_predicate_t less_than);
        /* Deletes the item at location i from the list. */
        void removeAtIndex(size_t i);
        /* Returns Node at index i */
        type_t findAtIndex(size_t i);
        /* Add an item into the list. */
        void insertNode(type_t node);

        size_t getSize() { return size; }

        ~OrderedArray() {}
};

#endif  // _DS_ORDERED_ARRAY_