all:
	g++ -shared -fPIC -lpthread countStats.cpp -o libcountcpp.so
	g++ test.cpp -L. -Wl,-rpath=. -lcountcpp -o testcpp.exe