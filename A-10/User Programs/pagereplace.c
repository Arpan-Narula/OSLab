#include "types.h"
#include "stat.h"
#include "user.h"

int ref[] = {7,0,1,2,0,3,0,4,2,3,0,3,2,1,2,0,1,7,0,1};
int n = 20;

int
fifo(int frames, int verbose)
{
  int mem[10];
  int count = 0, front = 0, faults = 0;
  int i, j, found;

  for(i = 0; i < frames; i++)
    mem[i] = -1;

  for(i = 0; i < n; i++){
    found = 0;
    for(j = 0; j < frames; j++)
      if(mem[j] == ref[i]){ found = 1; break; }

    if(!found){
      faults++;
      if(count < frames){
        mem[count] = ref[i];
        count++;
      } else {
        mem[front] = ref[i];
        front = (front + 1) % frames;
      }
    }

    if(verbose){
      printf(1, "Page %d -> %s  Frames: ", ref[i], found ? "HIT" : "FAULT");
      for(j = 0; j < frames; j++)
        printf(1, "%d ", mem[j]);
      printf(1, "\n");
    }
  }
  return faults;
}

int
lru(int frames, int verbose)
{
  int mem[10], last_used[10];
  int count = 0, faults = 0;
  int i, j, found, lru_index, min_time;

  for(i = 0; i < frames; i++)
    mem[i] = -1;

  for(i = 0; i < n; i++){
    found = 0;
    for(j = 0; j < frames; j++)
      if(mem[j] == ref[i]){ found = 1; last_used[j] = i; break; }

    if(!found){
      faults++;
      if(count < frames){
        mem[count] = ref[i];
        last_used[count] = i;
        count++;
      } else {
        lru_index = 0;
        min_time = last_used[0];
        for(j = 1; j < frames; j++)
          if(last_used[j] < min_time){ min_time = last_used[j]; lru_index = j; }
        mem[lru_index] = ref[i];
        last_used[lru_index] = i;
      }
    }

    if(verbose){
      printf(1, "Page %d -> %s  Frames: ", ref[i], found ? "HIT" : "FAULT");
      for(j = 0; j < frames; j++)
        printf(1, "%d ", mem[j]);
      printf(1, "\n");
    }
  }
  return faults;
}

int
optimal(int frames, int verbose)
{
  int mem[10];
  int count = 0, faults = 0;
  int i, j, k, found, farthest_index, farthest, next_use;

  for(i = 0; i < frames; i++)
    mem[i] = -1;

  for(i = 0; i < n; i++){
    found = 0;
    for(j = 0; j < frames; j++)
      if(mem[j] == ref[i]){ found = 1; break; }

    if(!found){
      faults++;
      if(count < frames){
        mem[count] = ref[i];
        count++;
      } else {
        farthest_index = 0;
        farthest = -1;
        for(j = 0; j < frames; j++){
          next_use = n;
          for(k = i + 1; k < n; k++)
            if(ref[k] == mem[j]){ next_use = k; break; }
          if(next_use > farthest){ farthest = next_use; farthest_index = j; }
        }
        mem[farthest_index] = ref[i];
      }
    }

    if(verbose){
      printf(1, "Page %d -> %s  Frames: ", ref[i], found ? "HIT" : "FAULT");
      for(j = 0; j < frames; j++)
        printf(1, "%d ", mem[j]);
      printf(1, "\n");
    }
  }
  return faults;
}

int
main(void)
{
  int f, faults, hits, hit_ratio, fault_ratio;

  printf(1, "Reference String: 7 0 1 2 0 3 0 4 2 3 0 3 2 1 2 0 1 7 0 1\n");
  printf(1, "Frames: 3\n\n");

  printf(1, "FIFO:\n");
  faults = fifo(3, 1);
  printf(1, "Total Page Faults (FIFO): %d\n\n", faults);

  printf(1, "LRU:\n");
  faults = lru(3, 1);
  printf(1, "Total Page Faults (LRU): %d\n\n", faults);

  printf(1, "Optimal:\n");
  faults = optimal(3, 1);
  printf(1, "Total Page Faults (Optimal): %d\n\n", faults);

  printf(1, "Comparison Table (3, 4 and 5 frames)\n");
  printf(1, "Frames  Algo     Faults  Hits  HitRatio%%  FaultRatio%%\n");

  for(f = 3; f <= 5; f++){
    faults = fifo(f, 0);
    hits = n - faults;
    hit_ratio = (hits * 100) / n;
    fault_ratio = (faults * 100) / n;
    printf(1, "%d       FIFO     %d       %d     %d          %d\n", f, faults, hits, hit_ratio, fault_ratio);

    faults = lru(f, 0);
    hits = n - faults;
    hit_ratio = (hits * 100) / n;
    fault_ratio = (faults * 100) / n;
    printf(1, "%d       LRU      %d       %d     %d          %d\n", f, faults, hits, hit_ratio, fault_ratio);

    faults = optimal(f, 0);
    hits = n - faults;
    hit_ratio = (hits * 100) / n;
    fault_ratio = (faults * 100) / n;
    printf(1, "%d       Optimal  %d       %d     %d          %d\n", f, faults, hits, hit_ratio, fault_ratio);
  }

  exit();
}
