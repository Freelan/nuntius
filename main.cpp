#include <iostream>
#include <curses.h>
#include <SFML/Network.hpp>
#include "Window.h"

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);
void centerWMvPrintW( WINDOW* window, int y, std::string message );
void Connect( WINDOW* window, sf::TcpSocket& socket, std::string ip, int port );
void Disconnect( WINDOW* window, sf::TcpSocket& socket );
void sendMessage( sf::TcpSocket& socket, std::string message );
std::string receiveMessage( sf::TcpSocket& socket );

int main()
{
    // ----- The client -----
    initscr();
    //cbreak();
    curs_set(0);
    noecho();

    //socket
    sf::TcpSocket socket;

    //title window
    Window titleScreen( 4, getmaxx(stdscr)-2, 1, 1 );
    titleScreen += "-- NUNTIUS --";
    titleScreen += "- not connected -";
    titleScreen.draw();

    //choice window
    std::string choices[4];
    choices[0] = "- local host -";
    choices[1] = "-   server   -";
    choices[2] = "-   custom   -";
    choices[3] = "-    quit    -";

    Window connectionChoiceScreen( 7, 18, 5, (getmaxx(stdscr)-18)/2 );
    for( int k = 0; k < 4; k++)
        connectionChoiceScreen += choices[k];
    connectionChoiceScreen.draw();

    wattron( connectionChoiceScreen.window, A_REVERSE );
    centerWMvPrintW( connectionChoiceScreen.window, 1, choices[0] );
    wattroff( connectionChoiceScreen.window, A_REVERSE );

    int i = 0; //which one
    const int minChoice = 0;
    const int maxChoice = 3;

    char Char;

    while( Char = getch() )
    {
        titleScreen.draw();
        connectionChoiceScreen.draw();

        //mvwprintw( titleScreen.window, 2, 2, "       \r%d", Char );
        //mvwprintw( titleScreen.window, 1, 2, "       \r%d", i );
        wrefresh( titleScreen.window );

        if( Char == 'a' && i != minChoice )
        {
            i--;
        }else if( Char == 'z' && i != maxChoice )
        {
            i++;
        }

        for( int j = 0; j <= maxChoice; j++ )
        {
            if( j == i )
                wattron( connectionChoiceScreen.window, A_REVERSE );
            centerWMvPrintW( connectionChoiceScreen.window, j+1, choices[j] );
            wattroff( connectionChoiceScreen.window, A_REVERSE );
        }

        if( Char == 10 )
        {
            wrefresh( connectionChoiceScreen.window );
            switch( i )
            {
                case 0:
                {
                    Connect( titleScreen.window, socket, "127.0.0.1", 55001 );
                    break;
                }
                case 1:
                {
                    Connect( titleScreen.window, socket, "89.40.127.246", 55001 );
                    break;
                }
                case 2:
                {
                    char ip[30];
                    echo();
                    mvwprintw( connectionChoiceScreen.window, getmaxy( connectionChoiceScreen.window ) - 2, 1, ">" );
                    wgetstr( connectionChoiceScreen.window, ip );
                    noecho();
                    connectionChoiceScreen.draw();

                    Connect( titleScreen.window, socket, ip, 55001 );
                    break;
                }
                case 3:
                {
                    clear();
                    return 0;
                    break;
                }
            }
            //titleScreen.draw();
            connectionChoiceScreen.draw();
        }
    }

    getch();
    endwin();
    return 0;
}

void action( sf::TcpSocket& socket )
{
    Window messagesScreen( getmaxy(stdscr)-10, getmaxx(stdscr)-2, 5, 1 );
    Window inputScreen( 4, getmaxx(stdscr)-2, getmaxy(stdscr)-5, 1 );

    messagesScreen.draw();
    inputScreen.draw();

    socket.setBlocking( false );

    sf::Packet incomingMessages;
    std::string incomingIp;
    std::string incomingMessage;

    while( true )
    {
        nodelay( stdscr, TRUE );

        if( socket.receive( incomingMessages ) == sf::Socket::Done )
        {
            incomingMessages >> incomingIp >> incomingMessage;

            scrollok( messagesScreen.window, TRUE );

            wprintw( messagesScreen.window, "\n " );
            messagesScreen.drawBorder();
            wprintw( messagesScreen.window, "[%s] %s", incomingIp.c_str(), incomingMessage.c_str() );
            wrefresh( messagesScreen.window );
        }

        char choice = getch();
        switch( choice )
        {
            case 10:
            {
                char message[120];

                wprintw( inputScreen.window, "\n " );
                inputScreen.drawBorder();

                curs_set(1);
                echo();
                wgetstr( inputScreen.window, message );
                noecho();
                curs_set(0);

                wclear(inputScreen.window);
                inputScreen.drawBorder();

                sendMessage( socket, message );

                break;
            }
            case '2':
            {
                wprintw( messagesScreen.window, "\n " );
                messagesScreen.drawBorder();
                Disconnect( messagesScreen.window, socket );
                goto loopEnd;
                break;
            }
        }
    }
loopEnd:
    nodelay( stdscr, FALSE );

    wclear( messagesScreen.window );
    destroy_win( messagesScreen.window );
    destroy_win( inputScreen.window );
}

void Connect( WINDOW* window, sf::TcpSocket& socket, std::string ip, int port )
{
    if( socket.connect( ip, port ) != sf::Socket::Done )
    {
        centerWMvPrintW( window, 2, "Can't connect to " + ip );
        wrefresh( window );
    }else
    {
        socket.setBlocking(true);
        std::string connectionMessage = receiveMessage( socket );
        if( connectionMessage != "whoops!" )
        {
            centerWMvPrintW( window, 2, connectionMessage );
            wrefresh( window );
            action( socket );
        }
    }
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void destroy_win(WINDOW *local_win)
{
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners
	 * and so an ugly remnant of window.
	 */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	/* The parameters taken are
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window
	 * 3. rs: character to be used for the right side of the window
	 * 4. ts: character to be used for the top side of the window
	 * 5. bs: character to be used for the bottom side of the window
	 * 6. tl: character to be used for the top left corner of the window
	 * 7. tr: character to be used for the top right corner of the window
	 * 8. bl: character to be used for the bottom left corner of the window
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	wrefresh(local_win);
	delwin(local_win);
}

void sendMessage( sf::TcpSocket& socket, std::string message )
{
    sf::Packet packet;
    packet << message;
    socket.send( packet );
}

std::string receiveMessage( sf::TcpSocket& socket )
{
    std::string message;
    sf::Packet packet;

    if( socket.receive( packet ) == sf::Socket::Done )
    {
         packet >> message;
         return message;
    }else
    {
        return "whoops!";
    }
}

void Disconnect( WINDOW* window, sf::TcpSocket& socket )
{
    sendMessage( socket, "kill" );

    wprintw( window, "Disconnecting...");
    wrefresh( window );

    socket.disconnect();
}

int getCenteredX( WINDOW* window, int width )
{
    int tempX = getmaxx( window );

    return (tempX - width) / 2;
}

int getCenteredY( WINDOW* window, int height )
{
    int tempY = getmaxy( window );

    return (tempY - height) / 2;
}

void centerWMvPrintW( WINDOW* window, int y, std::string message )
{
    mvwprintw( window, y, ( getmaxx( window ) - message.length() ) / 2, message.c_str() );
    wrefresh( window );
}
