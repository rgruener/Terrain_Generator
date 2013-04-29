rubiks.exe: main.o terrain.o InitShader.o
	g++ -o terrain.exe main.o terrain.o InitShader.o -lGLEW -lglut -lGL -lXmu -lX11 -lm -I./include

main.o: main.cpp
	g++ -c main.cpp -I./include

terrain.o: terrain.cpp terrain.h
	g++ -c terrain.cpp -I./include

InitShader.o: InitShader.cpp
	g++ -c InitShader.cpp -I./include

clean:
	rm -f *.exe *.o *.stackdump *~ *.swp *.swo *.swn
