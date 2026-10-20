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



//Changed for Q4
int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;

  addr = myproc()->sz;

  if(n > 0){
    myproc()->sz += n;          
  } else if(n < 0){
    if(growproc(n) < 0)  
      return -1;
  }

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


//Added for Q1
int
sys_va2pa(void)
{
  int va;

  if(argint(0, &va) < 0)
    return -1;
  return va2pa((uint)va);
}



//Added for Q2
int
sys_getmeminfo(void)
{
  struct meminfo *mi;
  struct proc *curproc = myproc();
  pde_t *pgdir;
  int i, count;

  if(argptr(0, (void*)&mi, sizeof(*mi)) < 0)
    return -1;

  pgdir = curproc->pgdir;

  mi->total_pages = PHYSTOP / PGSIZE;
  mi->free_pages  = get_free_pages();
  mi->proc_pages  = curproc->sz / PGSIZE;

  count = 1;
  for(i = 0; i < NPDENTRIES; i++){
    if(pgdir[i] & PTE_P)
      count++;
  }
  mi->pagetable_pages = count;

  return 0;
}


//Added for Q3

int
sys_vmprint(void)
{
  vmprint();
  return 0;
}
