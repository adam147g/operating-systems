#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <dlfcn.h>
#include "bibl1.h"

int main(int l_param, char *wparam[]) {
#ifdef DLL
    void *handle = dlopen("./bibl1.so", RTLD_LAZY);

    int (*f1)(int*, int) = dlsym(handle, "sumuj");
    double (*f2)(int*, int) = dlsym(handle, "srednia");
#endif

    int i;
    int tab[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
/*
1) otworz biblioteke
2) przypisz wskaznikom f1 i f2 adresy funkcji z biblioteki sumuj i srednia
3) stworz Makefile kompilujacy biblioteke 'bibl1' ladowana dynamicznie
   oraz kompilujacy ten program
*/

    for (i = 0; i < 5; i++) printf("Wynik: %d, %lf\n", f1(tab + i, 20 - i), f2(tab + i, 20 - i));
#ifdef DLL
    dlclose(handle);
#endif
    return 0;
}
