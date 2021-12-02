all:
	gcc -shared -fPIC -lpthread countStats.c -o libcountstats.so
	gcc test.c -L. -Wl,-rpath=. -lcountstats -o test.exe