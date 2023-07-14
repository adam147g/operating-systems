#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFF_SIZE 2048
#define MAIL_CMD_SIZE 248


int main(int argc, char* argv[]) {
    if (argc == 2)
    {
        char* key = argv[1];
        char command[10] = "sort -k ";
        // sort -k 2 - po adresie e-mail
        //         3 - po dacie
        if (strcmp(key, "nadawca") == 0) { 
            strcat(command, "2"); }
        else if (strcmp(key, "data") == 0) { 
            strcat(command, "3"); }
        else{
            return 1;
        }
        FILE* sort_input = popen(command, "w");
        FILE* mail_output = popen("mailq", "r");
        char buff[BUFF_SIZE];
        while(fgets(buff, sizeof(buff), mail_output) != NULL){
            fputs(buff, sort_input);
        }
        pclose(mail_output);
        pclose(sort_input);

    }
    else if (argc == 4){
        char command[MAIL_CMD_SIZE] = "mail -s ";
        strcat(command, "'");
        strcat(command, argv[2]);
        strcat(command, "' ");
        strcat(command, "'");
        strcat(command, argv[1]);
        strcat(command, "' ");
        // stworzenie komendy mail -s 'tytul' 'mail'
        
        FILE *mail_input = popen(command, "w");
        fputc('"', mail_input);
        fputs(argv[3], mail_input);
        fputc('"', mail_input);
        pclose(mail_input);
    }
    return 0;
}