#include "types.h"
#include "stat.h"
#include "user.h"

void work()
{
    for (int i = 0; i < 1000000; i++)
        asm volatile("nop");
}

int main(int argc, char *argv[])
{
    set_priority(getpid(), 1);
    int prios[3] = {5, 10, 15};

    for (int i = 0; i < 3; i++)
    {
        int pid = fork();
        if (pid < 0)
            printf(1, "Fork failed\n");
        else if (pid == 0)
        {
            set_priority(getpid(), prios[i]);
            work();
            printf(1, "My priority = %d\n", prios[i]);
            printf(1, "Finished.\n");
            exit();
        }
    }

    for (int i = 0; i < 3; i++)
        wait();
    printf(1, "Done\n");

    exit();
}