# FLAGS

DEBUG = -Wall -Wextra

FLAGS = -lpthread -pthread

CC = g++


all: SDIS

SDIS:
	$(CC) -g -o sdis $(FLAGS) ./src/main.cpp




