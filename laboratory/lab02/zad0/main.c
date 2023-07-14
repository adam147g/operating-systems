//
// Created by adam147g on 19.04.23.
//
#include <stdio.h>

int main ()
{
    char napis[20];
    FILE *plik=fopen("nazwa1.txt", "a+");
    if(plik)
    {
        fread(napis,1, 15,plik);
        printf("%s",napis);
        printf("\n");
        fwrite("Zdanie drugie.", 1, 14, plik);
        rename("nazwa1.txt","nazwa2.txt");
        fclose(plik);
    }
    return 0;
}