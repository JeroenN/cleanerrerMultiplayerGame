#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <map>
#include <sstream>
#include <string>
#include "player.h"
#include "server.h"
#include "bullet.h"
#include "initializing.h"
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>
void client_send_playerType(const std::pair<std::string, int> playerType, const sf::IpAddress serverIP, const unsigned short clientPort)
{
    std::string messageType="Port";
    sf::TcpSocket TcpSocket;
    sf::Packet playerTypePacket;

    TcpSocket.connect(serverIP,2000);
    playerTypePacket<<messageType<<playerType.first<<playerType.second;
    TcpSocket.send(playerTypePacket);
}
void receive_tcp_messages(sf::TcpSocket &TcpSocket, sf::TcpListener &listener)
{
    std::string messageType;
    sf::Packet tcpPacket;

    listener.accept(TcpSocket);
    int team;
    int role;

    if(TcpSocket.receive(tcpPacket)==sf::Socket::Done)
    {
        if(tcpPacket>>messageType)
        {
            if(messageType=="type_team_1")
            {
               if(tcpPacket>>team>>role)
               {
                   std::cout<<"amount of players team 1: "<<team << "\n"<<std::flush ;
                   switch(role)
                   {
                        case 1:
                        std::cout<<"role selected: spectator" << "\n \n"<<std::flush;
                        break;
                        case 2:
                        std::cout<< "role slected: player" << "\n \n"<<std::flush;
                        break;
                        case 3:
                        std::cout<< "role slected: both spectator and player" << "\n \n"<<std::flush;
                        break;
                   }

               }
            }
        }
    }
}

void do_client(std::vector<player> &players, std::pair<std::string, int> playerType, unsigned short &clientPort, bool &update,sf::RenderWindow &window)
{
    const std::string role =playerType.first;
    int shooting_dir=0;
    int timeWalking=0;
    int timeShooting=0;
    int celSize=30;
    std::vector<bullet> clientBullets{};
    std::vector<bullet> serverBullets{};
    sf::Event Event;
    const sf::IpAddress serverIP="127.0.0.1"; //Standard server ip
    const sf::IpAddress clientIP=sf::IpAddress::getLocalAddress();
    const std::string cIP=clientIP.sf::IpAddress::toString();

      /* initialize random seed: */
      srand (time(NULL));

     clientPort = rand() % 99999 + 1000;

    sf::TcpListener listener;
    sf::UdpSocket socket;
    sf::TcpSocket TcpSocket;

    socket.bind(clientPort);

    listener.listen(clientPort);
    socket.setBlocking(false);
    listener.setBlocking(false);
    TcpSocket.setBlocking(false);


    client_send_ip_port(cIP, clientPort, serverIP);
    client_send_playerType(playerType, serverIP, clientPort);

    while(window.isOpen())
    {
        receive_tcp_messages(TcpSocket, listener);
        window_events(window, Event, update); //selecting and deselecting the window and if the user presses escape the window closes
        timeWalking+=1;
        timeShooting+=1;
        const sf::Vector2f prevPosition = sf::Vector2f(players[0].getPosX(), players[0].getPosY());
        playerWalking(players, update, timeWalking, celSize);

        sf::IpAddress recipient = serverIP;
        unsigned short serverPort = 2000;
        set_shooting_dir(shooting_dir);
        shoot_bullet(clientBullets, recipient, serverPort,players, update, timeShooting, shooting_dir);


        if(player_check_walking(players, prevPosition)==true)
        {
            send_client_player_position(recipient, serverPort, players);
        }
        send_position_bullet(recipient, serverPort, clientBullets);

        receive_position_packets(socket, players, serverBullets);

        bulletHit(serverBullets, players, celSize);

        draw_everything(window, players, serverBullets, clientBullets, role, celSize);
    }
}

/*


  std::vector<int> v = {1,2,3,4,5,6};
  std::swap(v[3], v.back());
  v.pop_back();
*/
