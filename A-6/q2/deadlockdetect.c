#include "types.h"
#include "stat.h"
#include "user.h"

#define MAXN 4
#define MAXR 3

static int nproc, nres;
static int alloc[MAXN][MAXR];   /* allocation matrix: who HOLDS what */
static int req[MAXN][MAXR];     /* request matrix: who WAITS for what */
static int wfg[MAXN][MAXN];     /* wait-for graph adjacency */
static int color[MAXN];         /* 0 white, 1 gray (on stack), 2 black */
static int path[MAXN];          /* current DFS path */
static int plen;

static void
build_wfg(void)
{
  int i, j, r;
  for(i = 0; i < nproc; i++)
    for(j = 0; j < nproc; j++){
      wfg[i][j] = 0;
      for(r = 0; r < nres; r++)
        if(req[i][r] && alloc[j][r])
          wfg[i][j] = 1;   /* P_i waits for a resource held by P_j */
    }
}

static void
print_matrices(void)
{
  int i, r;
  printf(1, "Allocation matrix (rows=process, cols=resource):\n    ");
  for(r = 0; r < nres; r++) printf(1, " R%d", r);
  printf(1, "\n");
  for(i = 0; i < nproc; i++){
    printf(1, "P%d  ", i);
    for(r = 0; r < nres; r++) printf(1, "  %d", alloc[i][r]);
    printf(1, "\n");
  }
  printf(1, "Request matrix:\n    ");
  for(r = 0; r < nres; r++) printf(1, " R%d", r);
  printf(1, "\n");
  for(i = 0; i < nproc; i++){
    printf(1, "P%d  ", i);
    for(r = 0; r < nres; r++) printf(1, "  %d", req[i][r]);
    printf(1, "\n");
  }
}

static void
print_edges(void)
{
  int i, j, r;
  printf(1, "Wait-for graph edges:\n");
  for(i = 0; i < nproc; i++)
    for(j = 0; j < nproc; j++)
      if(wfg[i][j])
        for(r = 0; r < nres; r++)
          if(req[i][r] && alloc[j][r])
            printf(1, "  P%d -> P%d   (P%d waits for R%d held by P%d)\n",
                   i, j, i, r, j);
}

/* DFS; on back-edge print exact cycle from the path stack */
static int
dfs(int u)
{
  int v, k, t;
  color[u] = 1;
  path[plen++] = u;
  for(v = 0; v < nproc; v++){
    if(!wfg[u][v]) continue;
    if(color[v] == 1){                 /* back-edge -> cycle found */
      printf(1, "Cycle: ");
      for(k = 0; k < plen; k++){
        if(path[k] == v){
          for(t = k; t < plen; t++)
            printf(1, "P%d -> ", path[t]);
          printf(1, "P%d\n", v);
          return 1;
        }
      }
    }
    if(color[v] == 0 && dfs(v))
      return 1;
  }
  plen--;
  color[u] = 2;
  return 0;
}

static int
detect(void)
{
  int i;
  for(i = 0; i < nproc; i++) color[i] = 0;
  plen = 0;
  for(i = 0; i < nproc; i++)
    if(color[i] == 0 && dfs(i))
      return 1;
  return 0;
}

static void
run(char *name)
{
  printf(1, "\n=== %s ===\n", name);
  print_matrices();
  build_wfg();
  print_edges();
  if(detect())
    printf(1, "Result: DEADLOCK DETECTED\n");
  else
    printf(1, "Result: NO DEADLOCK (wait-for graph is acyclic)\n");
}

int
main(void)
{
  /* Scenario 1: chain P3->P2->P1->P0, acyclic */
  nproc = 4; nres = 3;
  memset(alloc, 0, sizeof alloc);
  memset(req, 0, sizeof req);
  alloc[0][0] = 1; alloc[1][1] = 1; alloc[2][2] = 1;
  req[1][0] = 1;   req[2][1] = 1;   req[3][2] = 1;
  run("Scenario 1: NO deadlock (acyclic wait-for graph)");

  /* Scenario 2: 3-process circular wait P0->P1->P2->P0 */
  nproc = 3; nres = 3;
  memset(alloc, 0, sizeof alloc);
  memset(req, 0, sizeof req);
  alloc[0][0] = 1; alloc[1][1] = 1; alloc[2][2] = 1;
  req[0][1] = 1;   req[1][2] = 1;   req[2][0] = 1;
  run("Scenario 2: DEADLOCK, 3-process circular wait");

  /* Scenario 3: bonus 2-process cycle P0<->P1 */
  nproc = 2; nres = 2;
  memset(alloc, 0, sizeof alloc);
  memset(req, 0, sizeof req);
  alloc[0][0] = 1; alloc[1][1] = 1;
  req[0][1] = 1;   req[1][0] = 1;
  run("Scenario 3: DEADLOCK, 2-process cycle (bonus)");

  exit();
}
