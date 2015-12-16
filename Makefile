.RECIPEPREFIX = >

CXX=g++
CXXFLAGS= -Wall -std=c++11
SDL = -lSDL2 -lGL -lSDL2_image
GLEW = -lGLEW 
ASSIMP = -lassimp
EXEC=program

# -Wl,-rpath,/usr/lib64
program : main.o game.o player.o controller.o camera.o model.o mesh.o shader.o utils.o
> $(CXX) $^ -o $@ $(SDL) $(GLEW) $(ASSIMP)

main.o : main.cpp
> $(CXX) -c $^ $(CXXFLAGS) -o $@

game.o : game.cpp
> $(CXX) -c $^ $(CXXFLAGS) -o $@

player.o : player.cpp
> $(CXX) -c $^ $(CXXFLAGS) -o $@

controller.o : controller.cpp
>$(CXX) -c $^ $(CXXFLAGS) -o $@

camera.o : camera.cpp
> $(CXX) -c $^ $(CXXFLAGS) -o $@

model.o : Model.cpp
> $(CXX) -c $^ $(CXXFLAGS) -o  $@

mesh.o : mesh.cpp
> $(CXX) -c $^ $(CXXFLAGS) -o $@

shader.o : shader.cpp
> $(CXX) -c $^ $(CXXFLAGS) -o $@

utils.o : utils.cpp
> $(CXX) -c $^ $(CXXFLAGS) -o $@

clean :
> rm *.o

rclean :
> clean
> rm $(EXEC)
