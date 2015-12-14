.RECIPEPREFIX = >

CXX=g++
CXXFLAGS= -Wall
SDL = -lSDL2 -lGL
GLEW = -lGLEW 
EXEC=program

# -Wl,-rpath,/usr/lib64
program : main.o game.o shader.o utils.o
> $(CXX) $^ -o $@ $(SDL) $(GLEW) 

main.o : main.cpp
> $(CXX) -c $^ $(CXXFLAGS) $(SDL) $(GLEW) -o $@

game.o : game.cpp
> $(CXX) -c $^ $(CXXFLAGS) $(SDL) $(GLEW) -o $@

shader.o : shader.cpp
> $(CXX) -c $^ $(CXXFLAGS) $(SDL) $(GLEW) -o $@

utils.o : utils.cpp
> $(CXX) -c $^ $(CXXFLAGS) $(SDL) $(GLEW) -o $@
 
clean :
> rm *.o
> rm $(EXEC)
