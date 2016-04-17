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
    sf::SocketSelector selector;
    selector.add( socket );
    if( selector.wait( sf::seconds( 2 ) ) )
    {
        if( selector.isReady( socket ) )
        {
            sf::Packet packet;
            socket.receive( packet );
            packet >> message;
        }
    }

    return message;
}

void Disconnect( sf::TcpSocket& socket )
{
    sendMessage( socket, "kill" );

    printw("Disconnecting...\n");
    socket.disconnect();
}

void action( sf::TcpSocket& socket )
{
    while( true )
    {
        //printw("%s\n", receiveMessage( socket ) );
        printw("[1] Wyslij wiadomosc\n"
               "[2] Rozlacz\n");
        char choice;
        switch( choice = getch() )
        {
            case '1':
            {
                char message[70];
                echo();
                getstr( message );
                noecho();
                sendMessage( socket, message );
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
    printw("duh?\n");
}

void Connect( sf::TcpSocket& socket, std::string ip, int port )
{
    if( socket.connect( ip, port ) != sf::Socket::Done )
    {
        printw( "Nie można połączyć się z %s\n", ip );
        exit( 1 );
    }

    printw("%s\n", receiveMessage( socket ) );
}

int main()
{
    initscr();
    noecho();
    // ----- The client -----
    sf::TcpSocket socket;

    printw(" - NUNTIUS - \n\n"
           "[1] Polacz z localhost\n"
           "[2] Inny\n");
    char choice;

    while( true )
    {
        printw("?\n");
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
