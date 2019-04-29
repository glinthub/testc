#include <signal.h>
#include <unistd.h>
#include "common.h"

/* zws:
 * on receiving a signal which is not registered by sigaction(),
 *  the default behaviour is to exit().
 * on receiving a signal which is registered,
 *  then the current blocking operation is interrupted.
 */
void sig_handler(int signum)
{
    printf("signal %d catched!\n", signum);
}
int main(int argc, char *argv[])
{
    struct sigaction act;
    act.sa_handler = sig_handler;
    sigaction(SIGALRM, &act, NULL);
    /*
     * fd[0] is for read, fd[1] is for write.
     * reversing them will cause error.
     */
    int ret = 0;
    pid_t pid = fork();
    if (pid == 0) //child process
    {
        char *msg = "hi, i'm child.";
        printf("%s\n", msg);
        while (1)
        {
            printf("%s\n", msg);
            sleep(1);
            kill(getppid(), SIGPIPE);
        }
    }
    else //parent proces
    {
        char *msg = "hi, i'm boss.";
        alarm(2); //arrange SIGALRM to sent to self 2 seconds later.
        while (1)
        {
            printf("%s\n", msg);
            sleep(1);
        }
    }
    return 0;
}
