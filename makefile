all:
	g++ -std=c++17 -Wall -shared -fPIC -I/usr/include/python3.8 test.cpp -o test.so
