#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

// Added the below part

#define SHM_BASE 0x3F000000
#define MAX_SHM 8
#define MAX_SEM 32

static char shm_pages[MAX_SHM][PGSIZE] __attribute__((aligned(PGSIZE)));

struct sem {
  struct spinlock lock;
  int count;
};
struct sem sems[MAX_SEM];
int sem_inited = 0;

uint64 sys_shm_get(void) {
  int id;
  argint(0, &id);
  if(id < 0 || id >= MAX_SHM) return 0;

  struct proc *p = myproc();
  uint64 va = SHM_BASE + id * PGSIZE;
  pte_t *pte = walk(p->pagetable, va, 0);
  
  if(pte == 0 || (*pte & PTE_V) == 0) {
    if(mappages(p->pagetable, va, PGSIZE, (uint64)shm_pages[id], PTE_R | PTE_W | PTE_U) < 0)
      return 0;
  }
  return va;
}

uint64 sys_sem_init(void) {
  int id, val;
  argint(0, &id);
  argint(1, &val);
  if(id < 0 || id >= MAX_SEM) return -1;
  
  if(!sem_inited) {
    for(int i = 0; i < MAX_SEM; i++) initlock(&sems[i].lock, "sem");
    sem_inited = 1;
  }
  
  acquire(&sems[id].lock);
  sems[id].count = val;
  release(&sems[id].lock);
  return 0;
}

uint64 sys_sem_wait(void) {
  int id;
  argint(0, &id);
  if(id < 0 || id >= MAX_SEM) return -1;
  
  acquire(&sems[id].lock);
  while(sems[id].count <= 0) {
    sleep(&sems[id], &sems[id].lock);
  }
  sems[id].count--;
  release(&sems[id].lock);
  return 0;
}

uint64 sys_sem_post(void) {
  int id;
  argint(0, &id);
  if(id < 0 || id >= MAX_SEM) return -1;
  
  acquire(&sems[id].lock);
  sems[id].count++;
  wakeup(&sems[id]);
  release(&sems[id].lock);
  return 0;
}
