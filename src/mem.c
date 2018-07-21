/*Student Name: Lester Hernandez Alfonso
  Panther ID: 4017986
  Course: COP4610
  Assignment#4*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "mem.h"

typedef struct block{
	struct block* next_block;
	int block_size;	
	int free;	
}block;

block* first_block = NULL;
int alloc_policy;

int Mem_Init(int size, int policy) {
	static int already_allocated = 0;
	void* memory_pointer;
	int page_size;
	int extra_padding;
	int final_size;
	int fd;
	
	if (already_allocated == 1) {
		return -1;
	}
	if (size <= 0) {
		return -1;
	}
	if (policy < 0 || policy > 2){
		return -1;
	}
	
	//Allocated size must be a multiple of page size.
	page_size = getpagesize();
        extra_padding = page_size - (size % page_size);
        final_size = size + extra_padding;

	fd = open("/dev/zero", O_RDWR);

	if (fd == -1){
		return -1;
	}
	
	memory_pointer = mmap(NULL, final_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (memory_pointer == MAP_FAILED){
		close(fd);
		return -1;
	}

	close(fd);
	already_allocated = 1;

	first_block = memory_pointer;
	first_block->next_block = NULL;
	first_block->block_size = final_size - sizeof(block);
	first_block->free = 1;

	alloc_policy = policy;

	return 0;	
}

void* Mem_Alloc(int size) {
	block* current_block = first_block;
	block* chosen_block = NULL;
	int maxBlockSize;
	int currentBlockSize;
	int adjusted_size;

	if (first_block == NULL) {
		return NULL;
	}

	if (size <= 0) {
		return NULL;
	}

	//Make size a multiple of sizeof(block) since pointer can only be incremented by a unit of sizeof(block).
	adjusted_size = size + (sizeof(block) - (size % sizeof(block)));

	//First-fit
	if (alloc_policy == 0) {
		while(current_block != NULL) {
			if (current_block->free == 1 && current_block->block_size >= adjusted_size) {
				chosen_block = current_block;
				break;
			}
			current_block = current_block->next_block;
		}	
	}
	//Best-fit
	else if (alloc_policy == 1) {
		maxBlockSize = 2147483647;
		while(current_block != NULL) {
			if (current_block->free == 1 && current_block->block_size >= adjusted_size) {
				currentBlockSize = current_block->block_size;
				if (currentBlockSize < maxBlockSize) {
					chosen_block = current_block;
					maxBlockSize = currentBlockSize;
				}
			}
			current_block = current_block->next_block;
		}
	}
	//Worst-fit
	else {
		maxBlockSize = 0;
		while(current_block != NULL) {
			if (current_block->free == 1 && current_block->block_size >= adjusted_size) {
				currentBlockSize = current_block->block_size;
				if (currentBlockSize > maxBlockSize) {
					chosen_block = current_block;
					maxBlockSize = currentBlockSize;
				}
			}
			current_block = current_block->next_block;
		}
	}
	
	if (chosen_block == NULL) {
		return NULL;
	}

	//Create contiguous free block if there's extra space in the block before it is allocated.
	if (chosen_block->block_size - sizeof(block) > adjusted_size) {

		/*We add +1 so that sizeof(block) pertaining to the size of our block structured
		corresponding to the new block is accounted for.*/
		struct block* new_block = chosen_block + 1 + (adjusted_size/sizeof(block));

		/*The new block size might not be multiple of sizeof(block) if chosen_block is the initial block
		allocated by Mem_Init. The following lines have an effect adjusting the value of new_block only in that case.*/
		int newBlockSize = chosen_block->block_size - adjusted_size - sizeof(block);
		int adjusted_newBlockSize = newBlockSize - (newBlockSize % sizeof(block));
		new_block->block_size = adjusted_newBlockSize;

		if(chosen_block->next_block != NULL) {
			new_block->next_block = chosen_block->next_block;
		}
		else{
			new_block->next_block = NULL;
		}

		new_block->free = 1;
		chosen_block->next_block = new_block;
		chosen_block->block_size = adjusted_size;
	}
	
	chosen_block->free = 0;

	return chosen_block;
}


int Mem_Free(void* ptr) {
	block* current_block = first_block;
	block* previous_block = NULL;

	//c_ptr stands for casted ptr.
	block* c_ptr = (block*) ptr;
	
	if (c_ptr == NULL) {
		return -1;
	}

	if (c_ptr < first_block) {
		return -1;
	}

	//Find the occupied block in which pointer is in.
	while(current_block != NULL) {
		if (c_ptr >= current_block && c_ptr < (current_block + (current_block->block_size/sizeof(block)) + 1) 
                    && current_block->free == 0) {
			break;
		}
                previous_block = current_block;
		current_block = current_block->next_block;
	}

	//Pointer not in a occupied block.
	if(current_block == NULL) {
		return -1;
	}

	current_block->free = 1;

	//Merge next_block into current_block if next_block is also free.
	if(current_block->next_block != NULL && current_block->next_block->free == 1) {
		current_block->block_size += (current_block->next_block->block_size + sizeof(block));
		current_block->next_block = current_block->next_block->next_block;
	}

	//Merge current_block into previous_block if previous_block is also free.
	if(previous_block != NULL && previous_block->free == 1) {
		previous_block->block_size += (current_block->block_size + sizeof(block));
		previous_block->next_block = current_block->next_block;
	}

	return 0;		
}


int Mem_IsValid(void* ptr) {
	block* current_block = first_block;
	block* c_ptr = (block*) ptr;
	
	if (c_ptr == NULL) {
		return 0;
	}
	
	//ptr falls before the current process allocation area.
	if (c_ptr < first_block) {
		return 0;
	}	

	while(current_block != NULL) {
		if (c_ptr >= current_block && c_ptr < current_block + (current_block->block_size/sizeof(block)) + 1) {
			if (current_block->free == 0) {
				//ptr falls within a currently allocated object/block.
				return 1;
			}
			else {
				//ptr falls within a currently unallocated object.
				return 0;
			}
		}
		current_block = current_block->next_block;
	}
	//ptr falls beyond the current process allocation area.
	return 0;
}


int Mem_GetSize(void* ptr) {
	block* current_block = first_block;
	block* c_ptr = (block*) ptr;
	
	if (c_ptr == NULL) {
		return -1;
	}
	
	//ptr falls before the current process allocation area.
	if (c_ptr < first_block) {
		return -1;
	}	

	while(current_block != NULL) {
		if(current_block->next_block != NULL) {
			if (c_ptr >= current_block && c_ptr < (current_block + (current_block->block_size/sizeof(block)) + 1)){
				if (current_block->free == 0) {
					//ptr falls within a currently allocated object/block.
					return current_block->block_size;
				}
				else {
					//ptr falls within a currently unallocated object.
					return -1;
				}
			}
			current_block = current_block->next_block;
		}
	}

	//ptr falls beyond the current process allocation area.
	return -1;
}


float Mem_GetFragmentation(){
	block* current_block = first_block;
	double totalFreeMemory = 0;
	double largestFreeBlock = 0;

	while(current_block != NULL) {
		if (current_block->free == 1) {
			totalFreeMemory += (double)current_block->block_size;

			if(current_block->block_size > largestFreeBlock) {
				largestFreeBlock = (double)current_block->block_size;	
			}
		}
		current_block = current_block->next_block;
	}
	
	if(totalFreeMemory == 0) {
		return 1;
	}

	return largestFreeBlock/totalFreeMemory;
}











