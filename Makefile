CC = g++
PROJECT = output
SRC = main.cpp control_window_params.cpp callback_functions.cpp utility.cpp

LIBS = `pkg-config --cflags --libs opencv4`
$(PROJECT) : $(SRC)
	$(CC) $(SRC) -o $(PROJECT) $(LIBS)