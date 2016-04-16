all:
	g++ -std=c++11 main.cpp -o nuntius -lncurses -lsfml-network -lsfml-system

Release:
	g++ -std=c++11 main.cpp -o bin/Release/nuntius -lncurses -lsfml-network -lsfml-system
