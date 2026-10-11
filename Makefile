all:
	g++ src/main.cpp -o main -I "C:\SDL\x86_64-w64-mingw32\include" -L "C:\SDL\x86_64-w64-mingw32\lib" -lSDL3
clean:
	rm -f main.exe