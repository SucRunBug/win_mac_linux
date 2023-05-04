#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t id = fork();
    if (id == 0)
    {
        sleep(5);
        int a = 1 / 0;  // 除0错误，如果父进程不进行等待，会成为僵尸进程
    }
    else 
    {
        while (1)
        {
            printf("I am father process.\n");
            sleep(1);
            int status;
            pid_t ret = wait(&status);
            if (ret > 0)
            {
                if (WIFEXITED(status))
                {
                    printf("Child process %d exit with code %d\n", ret, WIFEXITED(status));
                }
                else if(WIFSIGNALED(status))
                {
                    printf("Child process %d exit due to signal %d\n", ret, WTERMSIG(status));
                }
            }
        }
    }
    return 0;
}
// int main()
// {
//     pid_t id = fork();
//     if(id == 0)
//     {
//         sleep(5);
//         int a = 1 / 0;
//     }
//     else 
//     {
//         while (1)
//         {
//             printf("I am father!\n");
//         }
//     }
//     return 0;
// }