#ifndef ARRAYLIST_H
#define ARRAYLIST_H
#include <stdlib.h>
#include <assert.h>
#include <string.h>
typedef struct {
  unsigned int size; // Cantidad de elementos en la lista
  unsigned int capacity; // Memoria asignada, en elementos que caben
  unsigned int factor; // Factor de ampliacion cuando no cabe la insercion. Si ==0 significa n*n.
  void** body; // Puntero a la memoria asignada. tamano = mem_alloc * sizeof(void)
}arraylist;
void arraylist_destroy(arraylist *al);
arraylist *arraylist_clone(arraylist *al);
arraylist *arraylist_slice(arraylist *al,unsigned int index,unsigned int length);
arraylist *arraylist_slice_same(arraylist *al,unsigned int index,unsigned int length);
arraylist *arraylist_slice_cap_fac(arraylist *al,unsigned int index,unsigned int length,unsigned int capacity,unsigned int factor);
void arraylist_clear(arraylist *al);
void *arraylist_remove(arraylist *al,unsigned int index);
void arraylist_insert(arraylist *al,unsigned int index,void *elem);
void arraylist_set(arraylist *al,unsigned int index,void *elem);
void *arraylist_pop(arraylist *al);
void *arraylist_get(arraylist *al,unsigned int index);
void arraylist_add(arraylist *al,void *elem);
void arraylist_allocate(arraylist *al,unsigned int size);
arraylist *arraylist_create();
arraylist *arraylist_create_cap(int cap);
arraylist *arraylist_create_cap_fac(int cap,int fac);
#define arraylist_memshift(s, offset, length) memmove((s) + (offset), (s), (length)* sizeof(s));
#define DEFAULT_FACTOR 0
#define DEFAULT_CAPACITY 4
#endif