#include "Heap.h"
#include <ctime>
#define FREE 0
#define BUSY 1

#define HEAP_TRUE 1
#define HEAP_FALSE 0

//block states
typedef enum{   
	eBlockState__OK = 0,
	eBlockState__Busy,
	eBlockState__NoSize,
}BlockState;

//Service data
struct Heap
{
	char status;
	uint block_size;
};

//internal functions

/*initzialize service part*/
static void* serv_init(void* block, int size, char status);
/*check for unallocated block*/
static Heap* normalize_block_allocation(Heap* mem_block);
/*check for state of the block*/
static BlockState check_block(void*  block, int size);
/*return next memory block*/
static void *get_next_block(Heap* mem_block);
/*internal alloc implementation*/
static void* my_alloc_internal(uint size, Heap* block_start, char bExitOnBusy);
/*unite 2 memory blocks*/
static void *unite_blocks(Heap* mem_block1, Heap* mem_block2);
/*offsets block to service part*/
static Heap* normalizeRawBlock(void* mem_block);

static uint defaul_heap_mem;
static int heapSize;
static void* heapmem;
static void * heap_start;
static void * free_start;

static void* serv_init(void* block, int size, char status){
	Heap * Serv_part = (Heap*)block;
	Serv_part->block_size = size == 0 ? Serv_part->block_size : size;
	Serv_part->status = status;
	return (char*)block + sizeof(Heap);
}
void heap_alloc(uint size){
	defaul_heap_mem = size + sizeof(Heap);
	heapmem = heap_start= calloc(defaul_heap_mem, sizeof(char));
	serv_init(heapmem, size, FREE);
}
static Heap* normalize_block_allocation(Heap* mem_block)
{
	if (mem_block->block_size == 0){
		mem_block->block_size = defaul_heap_mem - (mem_block - (Heap*)heap_start) - sizeof(Heap);
	}
	return mem_block;
}

static BlockState check_block(void*  block, uint size)
{
	Heap* mem_block = (Heap*)block;
	mem_block = normalize_block_allocation(mem_block);
	if (FREE == mem_block->status){
		if (mem_block->block_size >= size){
			return eBlockState__OK;

		}
		return eBlockState__NoSize;
	}
	return eBlockState__Busy;
}


static void *get_next_block(Heap* mem_block){
	char* out = (char*)mem_block;
	return out + sizeof(Heap)+mem_block->block_size;
}
static void *unite_blocks(Heap* mem_block1, Heap* mem_block2){
	mem_block1->block_size += mem_block2->block_size + sizeof(Heap);
	return mem_block1;
}
static void* my_alloc_internal(uint size, Heap* block_start, char bExitOnBusy = HEAP_FALSE)
{
	Heap* mem_block = (Heap*)block_start;
	Heap* mem_end = (Heap*)heap_start + defaul_heap_mem;
	//check for size overflowing defaul_heap_mem  and free size
	while (mem_block  <  mem_end){
			char block_status = check_block(mem_block, size);

		
		switch (block_status){
		case eBlockState__OK:
			//have memory return it, doesn’t handle situation when block is too big			
			return serv_init(mem_block , size,  BUSY);

			break;
		case eBlockState__Busy:
			//go to next block
			if (bExitOnBusy) { 
				return NULL;
			}
			mem_block = (Heap*)get_next_block(mem_block);
			break;
		case eBlockState__NoSize:
		{
									
									int  sizeleft = size - mem_block->block_size;

									
									Heap* next = (Heap*) my_alloc_internal(sizeleft, (Heap*)(get_next_block(mem_block), HEAP_TRUE));
									if (next != NULL){
										mem_block = (Heap*)unite_blocks(mem_block, next);
										return  serv_init(mem_block, 0, BUSY);
									}
									//no next, so we can’t alloc requested size
									return  NULL;

									break;
		}
		};

	}
	return NULL;
}
void* my_alloc(uint size)
{
	//heap_start but porper is first free block
	return my_alloc_internal(size, (Heap*)heap_start);
}
static Heap* normalizeRawBlock(void* mem_block)
{
	Heap* normalized_block = ((Heap*)((char*)mem_block - sizeof(Heap)));
	if (normalized_block  < heap_start){
		//something went wrong do nothing here
		return NULL;
	}
	return normalized_block;
}

void my_free(void* mem_block)
{
	Heap* normalized_block = normalizeRawBlock(mem_block);
	if (normalized_block) {
		normalized_block->status = FREE;
	}

}

void print_heap(){
	Heap* mem_block = (Heap*)heap_start;
	Heap* mem_end = (Heap*)heap_start + defaul_heap_mem;
	//check for size overflowing defaul_heap_mem  and free size
	while (mem_block  <  mem_end){
		mem_block  = normalize_block_allocation(mem_block);
		puts(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
		printf("Block Address :: %p\n", mem_block);
		printf("Block Status :: %u\n", (int)mem_block->status);
		printf("Block Size :: %u\n", mem_block->block_size);
		puts("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
		mem_block = (Heap*)get_next_block(mem_block);

			
	}
}


void main(){
	heap_alloc(124);
	while (1){
		int *p = (int*)my_alloc(1);
		if (p == NULL){
			break;
		}
		*p = 19;
				
	}
	print_heap();
}
