all:
	g++ src/*.cpp -o main -I "C:\SDL\x86_64-w64-mingw32\include" -L "C:\SDL\x86_64-w64-mingw32\lib" -lSDL3
	cp "C:/SDL/x86_64-w64-mingw32/bin/SDL3.dll" .
clean:
	rm -f main.exe SDL3.dll