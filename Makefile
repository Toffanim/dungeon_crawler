#Need to change the prefix for command as it's not easy for me
#to do a tab char in emacs
.RECIPEPREFIX = >

#Define compiler related vars
CXX := g++
CXXFLAGS := -std=c++11
SDL := -lSDL2 -lGL -lSDL2_image
GLEW := -lGLEW 
ASSIMP := -lassimp

#Define project related vars
EXEC := dungeonGL
MODULES := main actor managers player game camera controller mesh shader utils
SRC_DIR := $(addprefix src/, $(MODULES))
BUILD_DIR := $(addprefix build/, $(MODULES))

SRC := $(foreach sdir, $(SRC_DIR), $(wildcard $(sdir)/*.cpp))
OBJ := $(patsubst src/%.cpp, build/%.o, $(SRC))
vpath %.cpp $(SRC_DIR)

#build command with .o and libs
define build-cmd
$(CXX) $^ -o $@ $(SDL) $(GLEW) $(ASSIMP)
endef

#default command taking a .cpp and outputing .o
define default-cmd
$1/%.o : %.cpp
> $(CXX) -c $$< $(CXXFLAGS) -o $$@
endef

#build program
build/$(EXEC) : $(OBJ)
> $(build-cmd)

#Make commands PHONY just in case we have a file with one of those names
#Most likely not the case
.PHONY: all checkdirs clean

#Need to create module target dir in build
checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
> @mkdir -p $@

#common clean cmd
clean:
> @rm -rf $(BUILD_DIR)

#cmd to execute if we want to compile without Makefile problems
all: checkdirs build/$(EXEC)

#iterate through each module target and call the default cmd
#Basically, create .o from .cpp files in modules directories
$(foreach bdir, $(BUILD_DIR), $(eval $(call default-cmd, $(bdir))))
