#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int sigint_ctr = 0;
int sigu1_ctr = 0;
int sigu2_ctr = 0;

void signal_handler(int sign_n)
{
    switch(sign_n)
    {
        case SIGINT:
                sigint_ctr++;
                break;
        case SIGUSR1:
                sigu1_ctr++;
                break;
        case SIGUSR2:
                sigu2_ctr++;
                break;
    }
}

int main()
{
    printf("PID is %i\n", getpid());

    signal(SIGINT, signal_handler);
    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);
    signal(SIGTERM, SIG_IGN);

    while(sigint_ctr != 5)
    {
        printf("Current time is %ld\n", time(NULL));
        sleep(1);
    }
    printf("Recieved SIGINT: %i\n", sigint_ctr);
    printf("Recieved SIGUSR1: %i\n", sigu1_ctr);
    printf("Recieved SIGUSR2: %i\n", sigu2_ctr);
    return 0;
}
