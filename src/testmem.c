/*Student Name: Lester Hernandez Alfonso
  Panther ID: 4017986
  Course: COP4610
  Assignment#4*/

#include <stdio.h>
#include "mem.h"

#define REGION_SIZE (10*1024)

void* myalloc(int size)
{
  printf("allocate memory of size=%d bytes...", size);
  void* p = Mem_Alloc(size);
  if(p) printf("  success (p=%p, f=%g)\n", p, Mem_GetFragmentation());
  else printf("  failed\n");
  return p;
}

void myfree(void* p)
{
  printf("free memory at p=%p...", p);
  if(!Mem_Free(p)) printf("  success (f=%g)\n", Mem_GetFragmentation());
  else printf("  failed\n");
}
  
int main(int argc, char* argv[])
{
  myalloc(1000);

  printf("init memory allocator...");
  if(Mem_Init(REGION_SIZE, MEM_POLICY_BESTFIT) < 0) {
    printf("  unable to initialize memory allocator!\n");
    return -1;
  } else printf("  success!\n");
    
  printf("init memory allocator, again...");
  if(Mem_Init(REGION_SIZE, MEM_POLICY_BESTFIT) < 0)
    printf("  failed, but this is expected behavior!\n");
  else {
    printf("  success, which means the program incorrectly handles duplicate init...\n");
    return -1;
  }

  void* x1 = myalloc(64);
  void* p1 = myalloc(200);
  void* x2 = myalloc(64);
  void* p2 = myalloc(100);
  void* x3 = myalloc(64);
  void* p3 = myalloc(100000);
  void* x4 = myalloc(64);
  void* p4 = myalloc(500);
  void* x5 = myalloc(64);

  printf("\nTrue size of block at %p = %d bytes\n", p2, Mem_GetSize(p2));
  if(Mem_IsValid(p4)) printf("%p is allocated according to Mem_IsValid()\n\n", p4);

  myfree(p1);
  myfree(p1+10);
  myfree(p2+10);
  myfree(p2+10);
  printf("\nTrue size of block at %p = %d bytes -> empty block\n\n", p2, Mem_GetSize(p2));
  myfree(p3);
  myfree(x4);
  void* p5 = myalloc(50);
  myfree(NULL);
  myfree(x1);
  myfree(x2);
  myfree(x3+10);	
  myfree(x5+10);
  myfree(p4);
  myfree(p5);

  if(!Mem_IsValid(p4)) printf("\n%p is NOT allocated according to Mem_IsValid()\n\n", p4);
  
  return 0;
}
