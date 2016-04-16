#include <iostream>
#include <SFML/Network.hpp>

void sendMessage( sf::TcpSocket& socket, std::string message )
{
    sf::Packet packet;
    packet << message;
    socket.send( packet );
}

std::string receiveMessage( sf::TcpSocket& socket )
{
    sf::Packet packet;
    std::string message;
    socket.receive( packet );
    packet >> message;

    return message;
}

void Disconnect( sf::TcpSocket& socket )
{
    sendMessage( socket, "kill" );

    std::cout << "Disconnecting..." << std::endl;
    socket.disconnect();
}

void action( sf::TcpSocket& socket )
{
    while( true )
    {
        std::cout << "[1] Wyslij wiadomosc" << std::endl
             << "[2] Rozlacz" << std::endl;
        int choice;
        std::cin >> choice;
        switch( choice )
        {
            case 1:
            {
                std::string message;
                //getline( std::cin, message );
                std::cin >> message;
                sendMessage( socket, message );
                break;
            }
            case 2:
            {
                Disconnect( socket );
                goto loopEnd;
                break;
            }
        }
    }
    loopEnd:
    std::cout << "duh?" << std::endl;
}

void Connect( sf::TcpSocket& socket, std::string ip, int port )
{
    if( socket.connect( ip, port ) != sf::Socket::Done )
    {
        std::cout << "Nie można połączyć się z " << ip << std::endl;
        exit( 1 );
    }

    std::cout << receiveMessage( socket ) << std::endl;
}

int main()
{
    // ----- The client -----
    sf::TcpSocket socket;

    std::cout << " - NUNTIUS - " << std::endl << std::endl
         << "[1] Połącz z localhost" << std::endl
         << "[2] Inny" << std::endl;
    int choice;

    while( true )
    {
    std::cout << "?" << std::endl;
    std::cin >> choice;
    switch( choice )
    {
        case 1:
        {
            Connect( socket, "127.0.0.1", 55001 );
            action( socket );
            break;
        }
        case 2:
        {
            std::string ip;
            std::cin >> ip;
            Connect( socket, ip, 55001 );
            action( socket );
            break;
        }
    }
    }
    return 0;
}
