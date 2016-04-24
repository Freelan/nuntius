#include "Window.h"

WINDOW *create_newwin(int height, int width, int starty, int startx);
void centerWMvPrintW( WINDOW* window, int y, std::string message );

Window::Window( int Height, int Width, int Starty, int Startx ) : height(Height), width(Width), starty(Starty), startx(Startx)
{
    //ctor
}

Window::~Window()
{
    //dtor
}

void Window::operator+=  (std::string String )
{
    contents.push_back( String );
}

void Window::draw()
{
    refresh();
    window = create_newwin( height, width, starty, startx );
    wrefresh( window );

    for( int i = 0; i < contents.size(); i++ )
    {
        centerWMvPrintW( window, i+1, contents[i] );
    }
}

void Window::drawBorder()
{
    box( window, 0 , 0 );
    wrefresh( window );
}
