#include<iostream>
typedef unsigned int uint;

/*@brief 
*
*/
/*allocate memory in global heap*/
void heap_alloc(uint size);
/*free allocated memory in my heap*/
void  my_free(void* mem_block);
/*allocats memory in my heap*/
void* my_alloc(uint size);
/*print memory map of heap*/
void print_heap();
