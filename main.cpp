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
    sf::SocketSelector selector;
    selector.add( socket );
    socket.setBlocking(false);
    //if( selector.wait( sf::seconds( 2 ) ) )
    {
        //if( selector.isReady( socket ) )
        {
            if( socket.receive( packet ) == sf::Socket::Done )
            {
                packet >> message;
                socket.setBlocking(true);
                return message;
            }else
            {
                socket.setBlocking(true);
                return "whoops!";
            }
        }
    }

}

void Disconnect( sf::TcpSocket& socket )
{
    sendMessage( socket, "kill" );

    printw("\rDisconnecting...\n");
    socket.disconnect();
}

void action( sf::TcpSocket& socket )
{
    printw("\n\n[enter] Send message\n"
           "[2] Disconnect\n\n");
    while( true )
    {
        nodelay( stdscr, TRUE );

        std::string incomingMessage = receiveMessage( socket );
        if( incomingMessage != "whoops!" )
            printw("\r[%s]\n", incomingMessage.c_str() );

        printw("\r>");
        char choice = getch();
        switch( choice )
        {
            case 10:
            {
                nodelay( stdscr, FALSE );
                char message[70];
                echo();
                printw(">> ");
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
    }

    std::string incomingMessage = receiveMessage( socket );
    if( incomingMessage != "whoops!" )
        printw("[%s]\n", incomingMessage.c_str() );
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
           "[3] Other\n");
    char choice;

    while( true )
    {
        printw(">> ");
        switch( choice = getch() )
        {
            case '1':
            {
                Connect( socket, "127.0.0.1", 55001 );
                action( socket );
                break;
            }
            case '2':
            {
                Connect( socket, "89.40.127.246", 55001 );
                action( socket );
                break;
            }
            case '3':
            {
                char ip[30];
                echo();
                getstr( ip );
                noecho();
                Connect( socket, ip, 55001 );
                action( socket );
                break;
            }
        }
    }
    endwin();
    return 0;
}
