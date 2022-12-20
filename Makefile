CC = g++
PROJECT = output
SRC = main.cpp control_window_params.cpp callback_functions.cpp utility.cpp circle_detection.cpp
OPTIONS = -flto -O3 -std=c++17

LIBS = `pkg-config --cflags --libs opencv4`
$(PROJECT) : $(SRC)
	$(CC) $(SRC) $(OPTIONS) -o $(PROJECT) $(LIBS)