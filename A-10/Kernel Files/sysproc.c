#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


//Added these functions for Q1, Q2 and Q3
int
sys_memstats(void)
{
  int alloc, freed, curr, avail;
  getmemstats(&alloc, &freed, &curr, &avail);
  cprintf("Total Allocated Pages: %d\n", alloc);
  cprintf("Total Freed Pages: %d\n", freed);
  cprintf("Currently Allocated Pages: %d\n", curr);
  cprintf("Available Physical Memory Pages: %d\n", avail);
  return 0;
}

int
sys_vtop(void)
{
  int va, pa;
  struct proc *curproc = myproc();

  if(argint(0, &va) < 0)
    return -1;

  pa = va2pa(curproc->pgdir, (uint)va);
  if(pa == -1){
    cprintf("Invalid or unmapped virtual address\n");
    return -1;
  }
  cprintf("Virtual Address: 0x%x -> Physical Address: 0x%x\n", va, pa);
  return pa;
}

int
sys_pageinfo(void)
{
  int va;
  uint frame;
  int present, writable, user;
  struct proc *curproc = myproc();

  if(argint(0, &va) < 0)
    return -1;

  if(getpageinfo(curproc->pgdir, (uint)va, &frame, &present, &writable, &user) < 0){
    cprintf("Page not mapped\n");
    return -1;
  }

  cprintf("Frame Number: %d\n", frame);
  cprintf("Flags -> P:%d W:%d U:%d\n", present, writable, user);
  return 0;
}

int
sys_setreadonly(void)
{
  int va;
  struct proc *curproc = myproc();

  if(argint(0, &va) < 0)
    return -1;

  if(clearwrite(curproc->pgdir, (uint)va) < 0){
    cprintf("Page not mapped\n");
    return -1;
  }

  cprintf("Write permission removed for page at 0x%x\n", va);
  return 0;
}
