typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef uint pde_t;

//Added for Q2
struct meminfo {
  int total_pages;
  int free_pages;
  int proc_pages;
  int pagetable_pages;
};
