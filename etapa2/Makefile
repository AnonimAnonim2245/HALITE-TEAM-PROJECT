CC = g++
CCFLAGS = -Wall -Wextra -std=c++17 -O0 -lm

.PHONY: build clean

build: MyBot

run:
	./MyBot


MyBot: MyBot.cpp
	$(CC) -o $@ $^ $(CCFLAGS)

clean:
	rm -f MyBot

.SILENT: run