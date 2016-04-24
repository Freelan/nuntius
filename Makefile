all:
	g++ -std=c++11 main.cpp Window.cpp -o nuntius -lncurses -lsfml-network -lsfml-system

Release:
	g++ -std=c++11 main.cpp Window.cpp -o bin/Release/nuntius -lncurses -lsfml-network -lsfml-system

Debug:
	g++ -g -std=c++11 main.cpp Window.cpp -o bin/Debug/nuntius -lncurses -lsfml-network -lsfml-system
