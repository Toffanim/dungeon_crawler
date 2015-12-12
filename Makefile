.RECIPEPREFIX = >

CXX=g++
CXXFLAGS= -Wall
SDL = -lSDL2 -lGL
GLEW = -lGLEW 
EXEC=program

# -Wl,-rpath,/usr/lib64
program : main.o
> $(CXX) $^ -o $@ $(SDL) $(GLEW) 

main.o : main.cpp
> $(CXX) -c $^ $(CXXFLAGS) $(SDL) $(GLEW) -o $@

clean :
> rm *.o
> rm program
