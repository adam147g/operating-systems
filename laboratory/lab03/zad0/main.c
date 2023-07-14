#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

// #include <stdio.h>
// #include <sys/types.h>
// #include <unistd.h>

 int main() {
     pid_t child_pid;
     printf("PID glownego programu: %d\n", (int)getpid());
     child_pid = fork();
     if(child_pid!=0) {
         printf("Proces rodzica: Proces rodzica ma pid:%d\n", (int)getpid());
         printf("Proces rodzica: Proces dziecka ma pid:%d\n", (int)child_pid);
     } else {
         printf("Proces dziecka: Proces rodzica ma pid:%d\n",(int)getppid());
         printf("Proces dziecka: Proces dziecka ma pid:%d\n",(int)getpid());
     }

     return 0;
 }


// int main() {
//     pid_t child_pid;
//     printf("Początek\n");
//     for(int i = 0; i < 10; i++){    
//         child_pid = fork();
//     }
//     printf("PID glownego programu: %d\n", (int)getpid());
//     printf("koniec\n");
//     return 0;
// }

//int main() {
//    pid_t child_pid;
//    printf("Początek\n");
//    child_pid = fork();
//    for (int i = 0; i < 8; i++)
//        if(child_pid != 0)
//            child_pid = fork();
//
//    printf("koniec\n");
//    return 0;
//}