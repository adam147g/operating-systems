CC=gcc
CFLAGS=-Wall
LIB=libwc
REPL=wcREPL.c
BINS=wcREPL.c dllmanager.h

all:
	make lib_static
	make lib_shared
	make exe_static
	make exe_shared
	make exe_dlopen

lib_static: $(LIB).c
	$(CC) $(CFLAGS) $(OPT_FLAGS) -c $(LIB).c
	ar cr $(LIB)$(SUFFIX).a $(LIB).o

lib_shared: $(LIB).c
	$(CC) $(CFLAGS) $(OPT_FLAGS) -c -fPIC $(LIB).c
	$(CC) $(CFLAGS) $(OPT_FLAGS) -shared -o $(LIB).so $(LIB).o

exe_static: $(LIB).c $(BINS)
	$(CC) $(CFLAGS) $(OPT_FLAGS) $(REPL) -o wcREPL_static.exe -L . $(LIB).a

exe_shared: $(LIB).c $(BINS)
	$(CC) $(CFLAGS) $(OPT_FLAGS) $(REPL) -o wcREPL_shared.exe -L . $(LIB).so

exe_dlopen: $(LIB).c $(BINS)
	$(CC) $(CFLAGS) $(OPT_FLAGS) $(REPL) -o wcREPL_dlopen.exe -ldl -D USE_DLL

clean:
	rm -f *.o *.a *.so *.exe