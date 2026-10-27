#include "types.h"
#include "stat.h"
#include "user.h"

#define MAXFRAMES 10
#define MAXREFS 100

int
find(int *frames, int nframes, int page)
{
  int i;
  for(i = 0; i < nframes; i++){
    if(frames[i] == page)
      return i;
  }
  return -1;
}

int
fifo(int *refs, int nrefs, int nframes, int printit)
{
  int frames[MAXFRAMES];
  int next = 0;
  int filled = 0;
  int faults = 0;
  int i, j;

  for(i = 0; i < nframes; i++)
    frames[i] = -1;

  for(i = 0; i < nrefs; i++){
    int page = refs[i];
    int hit = (find(frames, nframes, page) != -1);

    if(!hit){
      faults++;
      frames[next] = page;
      next = (next + 1) % nframes;
      if(filled < nframes)
        filled++;
    }

    if(printit){
      printf(1, "ref %d : ", page);
      for(j = 0; j < nframes; j++){
        if(frames[j] == -1)
          printf(1, "_ ");
        else
          printf(1, "%d ", frames[j]);
      }
      printf(1, "%s\n", hit ? "HIT" : "FAULT");
    }
  }
  return faults;
}

int
lru(int *refs, int nrefs, int nframes, int printit)
{
  int frames[MAXFRAMES];
  int lastused[MAXFRAMES];
  int filled = 0;
  int faults = 0;
  int i, j, time;

  for(i = 0; i < nframes; i++){
    frames[i] = -1;
    lastused[i] = -1;
  }

  time = 0;
  for(i = 0; i < nrefs; i++){
    int page = refs[i];
    int idx = find(frames, nframes, page);
    int hit = (idx != -1);

    if(!hit){
      faults++;
      if(filled < nframes){
        idx = filled;
        filled++;
      } else {
        int victim = 0;
        for(j = 1; j < nframes; j++){
          if(lastused[j] < lastused[victim])
            victim = j;
        }
        idx = victim;
      }
      frames[idx] = page;
    }
    lastused[idx] = time;
    time++;

    if(printit){
      printf(1, "ref %d : ", page);
      for(j = 0; j < nframes; j++){
        if(frames[j] == -1)
          printf(1, "_ ");
        else
          printf(1, "%d ", frames[j]);
      }
      printf(1, "%s\n", hit ? "HIT" : "FAULT");
    }
  }
  return faults;
}

int
main(int argc, char *argv[])
{
  int refs[MAXREFS];
  int nrefs;
  int nframes;
  int i;

  if(argc < 3){
    printf(1, "usage: pagerepl <frames> <ref1> <ref2> ...\n");
    exit();
  }

  nframes = atoi(argv[1]);
  nrefs = argc - 2;
  for(i = 0; i < nrefs; i++)
    refs[i] = atoi(argv[i + 2]);

  printf(1, "FIFO with %d frames:\n", nframes);
  int ffaults = fifo(refs, nrefs, nframes, 1);
  printf(1, "FIFO total faults: %d\n\n", ffaults);

  printf(1, "LRU with %d frames:\n", nframes);
  int lfaults = lru(refs, nrefs, nframes, 1);
  printf(1, "LRU total faults: %d\n\n", lfaults);

  printf(1, "frames  FIFO_faults  FIFO_hitratio  LRU_faults  LRU_hitratio\n");
  for(i = 3; i <= 5; i++){
    int ff = fifo(refs, nrefs, i, 0);
    int lf = lru(refs, nrefs, i, 0);
    printf(1, "%d       %d            %d%%            %d           %d%%\n",
           i, ff, 100 - (ff * 100) / nrefs, lf, 100 - (lf * 100) / nrefs);
  }

  exit();
}
