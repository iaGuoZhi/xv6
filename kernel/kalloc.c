// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock[NCPU];
  struct run* freelist[NCPU];
} kmem;

void
kinit()
{
  static char names_buf[PGSIZE];
  int off = 0;
  for(int i = 0; i < NCPU; ++i) {
    int n = snprintf(names_buf + off, PGSIZE - off, "kmem-%d", i);
    initlock(&(kmem.lock[i]), names_buf + off);
    names_buf[off + n] = 0;
    off += (n + 1);
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  int cpu;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  // cpu() should be called when interrupts are turned off
  push_off();
  cpu = cpuid();
  pop_off();

  acquire(&(kmem.lock[cpu]));
  r->next = kmem.freelist[cpu];
  kmem.freelist[cpu] = r;
  release(&(kmem.lock[cpu]));
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;
  int cpu;

  // cpu() should be called when interrupts are turned off
  push_off();
  cpu = cpuid();
  pop_off();

  acquire(&(kmem.lock[cpu]));
  r = kmem.freelist[cpu];
  if(r)
    kmem.freelist[cpu] = r->next;
  release(&(kmem.lock[cpu]));

  // Current cpu's freelist is empty,
  // borrow from others
  if(!r) {
    int idx = cpu;
    do {
      idx = (idx + 1) % NCPU;
      acquire(&(kmem.lock[idx]));
      r = kmem.freelist[idx];
      if(r)
        kmem.freelist[idx] = r->next;
      release(&(kmem.lock[idx]));
    } while(!r && idx != cpu);
  }

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
