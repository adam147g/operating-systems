#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/wait.h>

int interrupt_num;

void info(int sig_num, siginfo_t *info, void *context) {
    printf("Signal number: %d \n", info->si_signo);
    printf("Process ID of sending process: %d \n", info->si_pid);
    printf("Signal code: %d\n", info->si_code);
    printf("Signal band number: %ld\n", info->si_band);
    printf("Signal value: %d \n\n", info->si_value.sival_int);
}

void test1(){
    struct sigaction case1;
    case1.sa_flags = SA_SIGINFO;
    case1.sa_sigaction = info;
    sigemptyset(&case1.sa_mask);

    if (sigaction(SIGUSR1, &case1, NULL) == -1 || sigaction(SIGUSR2, &case1, NULL) == -1 
        || sigaction(SIGRTMIN, &case1, NULL) == -1){
        return;
    }
    printf("SIGINFO\n");
    raise(SIGUSR1);
    raise(SIGUSR2);
    raise(SIGRTMIN);
    printf("\n");
}


void sa_nodefer(int sig) {
    int im = interrupt_num;
    interrupt_num++;
    printf("Start SA_NODEFER %d %d\n", im, getpid());
    if(interrupt_num < 4)
        kill(getpid(), SIGINT);
    printf("End SA_NODEFER %d %d\n", im, getpid());
}

void test2() {
    struct sigaction act_info;
    printf("TESTING SA_NODEFER\n");
    act_info.sa_flags = SA_NODEFER;
    act_info.sa_handler = &sa_nodefer;
    sigaction(SIGINT, &act_info, NULL);
    raise(SIGINT);
    printf("\n");
}

void handle_resethand(int sig_no) {
    printf("My handle for SA_RESETHAND \n");
}

void test3() {
    struct sigaction act_info;
    sigemptyset(&act_info.sa_mask);
    act_info.sa_flags = SA_RESETHAND;
    act_info.sa_handler = handle_resethand;

    sigaction(SIGINT, &act_info, NULL);
    printf("SA_RESETHAND\n");
    raise(SIGINT);
    printf("If I use raise a second time, I'll get `Error 130`, which means,\n that sa_handler comes to default settings\n");

    // raise(SIGINT);
    printf("\n");
}

int main(int argc, char **argv) {
    // SA_SIGINFO - dodatkowe informacje o sygnale i źródle
    test1();
    // SA_NODEFER - gdy dostanie ten sam sygnał w czasie poprzedniego, 
    // przerywa i obsługuje nowy
    test2();
    // SA_RESETHAND - po obsłużeniu sygnału resetuje do domyślnej
    test3();
    return 0;
}