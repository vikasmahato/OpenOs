#include <data_structures/ordered_array.h>
#include <string.h>

uint8_t standardLessthanPredicate(type_t a, type_t b) {
   return (a<b)?1:0;
}

OrderedArray::OrderedArray(size_t max_size) { 
 //  this->array = kmalloc(max_size*sizeof(type_t));
 //  memset(this->array, 0, max_size*sizeof(type_t));
   this->size = 0;
   this->max_size = max_size;
   this->less_than = standardLessthanPredicate;
}

OrderedArray::OrderedArray(size_t max_size, lessthan_predicate_t less_than) { 
 //  this->array = kmalloc(max_size*sizeof(type_t));
 //  memset(this->array, 0, max_size*sizeof(type_t));
   this->size = 0;
   this->max_size = max_size;
   this->less_than = less_than;
}

OrderedArray::OrderedArray(virtual_addr *addr, size_t max_size, lessthan_predicate_t less_than) { 
    this->array = (type_t*) addr;
    memset(this->array, 0, max_size*sizeof(type_t));
    this->size = 0;
    this->max_size = max_size;
    this->less_than = less_than;
}

void OrderedArray::removeAtIndex(size_t i) {
    while (i < size) {
        array[i] = array[i+1];
        i++;
    }
    size--;
}

type_t OrderedArray::findAtIndex(size_t i) {
    if(i<size)
        return array[i];
    else
        return nullptr;
}

void OrderedArray::insertNode(type_t node) {
size_t iterator = 0;
    while (iterator < size && less_than(array[iterator], node))
       iterator++;
    if(iterator == size)
       array[size++] = node;
    else {
        type_t tmp = array[iterator];
        array[iterator] = node;
         while (iterator < size) {
            iterator++;
            type_t tmp2 = array[iterator];
            array[iterator] = tmp;
            tmp = tmp2;
        }
       size++;
    }
}