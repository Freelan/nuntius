#include <iostream>
#include <curses.h>
#include <SFML/Network.hpp>


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

void Disconnect( sf::TcpSocket& socket )
{
    sendMessage( socket, "kill" );

    printw("\rDisconnecting...\n");
    refresh();

    socket.disconnect();
}

void action( sf::TcpSocket& socket )
{
    printw("\n[enter] Send message\n"
           "[2] Disconnect\n\n");
    refresh();

    socket.setBlocking(false);

    sf::Packet incomingMessages;
    std::string incomingIp;
    std::string incomingMessage;

    while( true )
    {
        nodelay( stdscr, TRUE );

        if( socket.receive( incomingMessages ) == sf::Socket::Done )
        {
            incomingMessages >> incomingIp >> incomingMessage;
            //if( incomingMessage != "whoops!" )//&& incomingIp != "whoops!" )
            printw("\r[%s] %s\n", incomingIp.c_str(), incomingMessage.c_str() );
            refresh();
        }
            printw("\r>");
            refresh();
            char choice = getch();
            switch( choice )
            {
                case 10:
                {
                    nodelay( stdscr, FALSE );
                    char message[70];
                    echo();
                    printw(">> ");
                    refresh();
                    getstr( message );
                    noecho();
                    sendMessage( socket, message );
                    nodelay( stdscr, TRUE );
                    break;
                }
                case '2':
                {
                    Disconnect( socket );
                    goto loopEnd;
                    break;
                }
            }
    }
loopEnd:
    printw( "OKI\n");
    nodelay( stdscr, FALSE );
}

void Connect( sf::TcpSocket& socket, std::string ip, int port )
{
    if( socket.connect( ip, port ) != sf::Socket::Done )
    {
        printw( "Can't connect to %s\n", ip.c_str() );
        refresh();
    }else
    {
        socket.setBlocking(true);
        std::string connectionMessage = receiveMessage( socket );
        if( connectionMessage != "whoops!" )
        {
            printw("\n%s\n", connectionMessage.c_str() );
            refresh();
            action( socket );
        }
    }
}

int main()
{
    initscr();
    scrollok(stdscr,TRUE);
    noecho();
    // ----- The client -----
    sf::TcpSocket socket;

    printw(" - NUNTIUS - \n\n"
           "[1] localhost\n"
           "[2] 89.40.127.246\n"
           "[3] Other\n"
           "[q] Quit\n");
    refresh();
    char choice;

    while( true )
    {
        printw(">> ");
        switch( choice = getch() )
        {
            case '1':
            {
                Connect( socket, "127.0.0.1", 55001 );
                break;
            }
            case '2':
            {
                Connect( socket, "89.40.127.246", 55001 );
                break;
            }
            case '3':
            {
                char ip[30];
                echo();
                getstr( ip );
                noecho();
                Connect( socket, ip, 55001 );
                break;
            }
            case 'q':
            {
                return 0;
                break;
            }
        }
    }
    endwin();
    return 0;
}
