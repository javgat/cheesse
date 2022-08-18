/**
 * Implementacion de Arraylist
 * Javier Gatón Herguedas
 * 
 * Basada en la implementacion de Java, usa arrays que cuando se llenan
 * se copian en otro array de tamaño n*n, provocando que el tiempo amortizado
 * de insercion sea O(logn). El de busqueda es O(1).
 * Se usan punteros a void para representar elementos genericos.
 * Tambien se inspira en la implementacion de @marekweb de github.
 * No comprueba que se use correctamente, permite obtener datos del array
 * superiores al size real. Se espera su uso correcto para el funcionamiento
 * correcto.
**/
#include "arraylist.h"

/**
 * Macro para desplazar la memoria
 * s es la primera posicion, offset cuantas posiciones mover
 * length cuantos elementos mover
 */
#define arraylist_memshift(s, offset, length) memmove((s) + (offset), (s), (length)* sizeof(s));

/**
 * Crea un arraylist especificando la cantidad de memoria a reservar al inicio
 * y el factor de ampliacion que tendra. Si es 0 equivale a n*n.
 */
arraylist* arraylist_create_cap_fac(int cap, int fac){
  arraylist* new_list = (arraylist*) malloc(sizeof(arraylist));
  assert(new_list);
  new_list->size = 0;
  new_list->body = (void**) malloc(sizeof(void*) * cap);
  assert(new_list->body);
  new_list->capacity = cap;
  new_list->factor = fac;
  return new_list;
}

/**
 * Crea un arraylist especificando la cantidad de memoria a reservar al inicio
 */
arraylist* arraylist_create_cap(int cap){
  return arraylist_create_cap_fac(cap, DEFAULT_FACTOR);
}

/**
 * Crea un arraylist sin especificar la cantidad de memoria a reservar
 */
arraylist* arraylist_create(){
  return arraylist_create_cap(DEFAULT_CAPACITY);
}

/**
 * Asegura que el arraylist tenga suficiente hueco de capacidad como para
 * almacenar "size"
 */
void arraylist_allocate(arraylist* al, unsigned int size){
  while(size > al->capacity){
    unsigned int new_cap = al->capacity;
    if(al->factor==0 || al->factor == 1)
      new_cap = new_cap * new_cap;
    else
      new_cap = new_cap * al->factor;
    al->body = realloc(al->body, sizeof(void*) * new_cap);
    assert(al->body);
    al->capacity = new_cap;
  }
}

/**
 * Agrega un elemento al final de la lista
 */
void arraylist_add(arraylist* al, void* elem){
  al->size++;
  arraylist_allocate(al, al->size);
  al->body[al->size-1] = elem;
}

/**
 * Obtiene el elemento en posicion index
 */
void* arraylist_get(arraylist* al, unsigned int index){
  return al->body[index];
}

/**
 * Elimina y devuelve el ultimo elemento
 */
void* arraylist_pop(arraylist* al){
  al->size--;
  return al->body[al->size];
}

/**
 * Swap de el elemento en index por elem
 */
void arraylist_set(arraylist* al, unsigned int index, void* elem){
  al->body[index] = elem;
}

/**
 * Inserta un elemento en index, el resto se desplaza a la derecha
 */
void arraylist_insert(arraylist* al, unsigned int index, void* elem){
  int oldsize = al->size;
  al->size++;
  arraylist_allocate(al, al->size);

  arraylist_memshift(al->body + index, 1, oldsize - index);
  al->body[index] = elem;
}

/**
 * Elimina y devuelve el elemento en posicion index
 */
void* arraylist_remove(arraylist* al, unsigned int index){
  void* returned = al->body[index];
  int sigpos = index+1;
  arraylist_memshift(al->body + sigpos, -1, al->size - sigpos);
  al->size--;
  return returned;
}

/**
 * Marca como vacio el arraylist. Coste 0.
 */
void arraylist_clear(arraylist* al){
  al->size = 0;
}

/**
 * Devuelve un segmento del arraylist, especificando capacidad y factor
 */
arraylist* arraylist_slice_cap_fac(arraylist* al, unsigned int index, unsigned int length, unsigned int capacity, unsigned int factor){
  arraylist* new_list = arraylist_create_cap_fac(al->capacity, al->factor);
  arraylist_allocate(new_list, length);
  memcpy(new_list->body, al->body + index, length * sizeof(void*));
  new_list -> size = length;
  return new_list;
}

arraylist* arraylist_slice_same(arraylist* al, unsigned int index, unsigned int length){
  return arraylist_slice_cap_fac(al, index, length, al->capacity, al->factor);
}

/**
 * Devuelve un segmento del arraylist
 */
arraylist* arraylist_slice(arraylist* al, unsigned int index, unsigned int length){
  return arraylist_slice_cap_fac(al, index, length, DEFAULT_CAPACITY, DEFAULT_FACTOR);
}

/**
 * Devuelve una copia del arraylist
 */
arraylist* arraylist_clone(arraylist* al){
  return arraylist_slice_same(al, 0, al->size);
}

/**
 * Destruye y libera la memoria del arraylist
 */
void arraylist_destroy(arraylist* al){
  free(al->body);
  al->size = 0;
  al->capacity = 0;
  al->body = NULL;
  free(al);
}