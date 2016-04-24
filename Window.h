#ifndef WINDOW_H
#define WINDOW_H

#include <string>
#include <vector>
#include <curses.h>

class Window
{
    public:
        Window(int Height, int Width, int Starty, int Startx);
        virtual ~Window();

        void operator+= ( std::string string );
        void draw();
        void drawBorder();

        WINDOW* window;

    protected:
    private:
        std::vector <std::string> contents;
        int height, width, starty, startx;
};

#endif // WINDOW_H
