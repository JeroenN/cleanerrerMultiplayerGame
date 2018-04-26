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

void do_client(std::vector<player> &players, unsigned short &clientPort, bool &update,sf::RenderWindow &window)
{
    int shooting_dir=0;
    int time=0;
    std::vector<bullet> clientBullets{};
    std::vector<bullet> serverBullets{};
    sf::Event Event;
    const sf::IpAddress serverIP="127.0.0.1"; //Standard server ip
    const sf::IpAddress clientIP=sf::IpAddress::getLocalAddress();
    const std::string cIP=clientIP.sf::IpAddress::toString();
    clientPort=2001;

    sf::TcpListener listener;
    sf::UdpSocket socket;
    sf::TcpSocket messageTypeSocket;

    socket.bind(2001);

    //listener.listen(clientPort);
    socket.setBlocking(false);
    listener.setBlocking(false);
    messageTypeSocket.setBlocking(false);

    client_send_ip_port(cIP, clientPort, serverIP);

    while(window.isOpen())
    {
        window_events(window, Event, update); //selecting and deselecting the window and if the user presses escape the window closes
        time+=1;
        const sf::Vector2f prevPosition = sf::Vector2f(players[0].getPosX(), players[0].getPosY());
        playerWalking(players, update, time);

        sf::IpAddress recipient = serverIP;
        unsigned short serverPort = 2000;
        set_shooting_dir(shooting_dir);
        shoot_bullet(clientBullets, recipient, clientPort,players, update, time, shooting_dir);


        if(player_check_walking(players, prevPosition)==true)
        {
            send_position(recipient, serverPort, players);
        }
        send_position_bullet(recipient, serverPort, clientBullets);

        receive_position_packets(socket, players, serverBullets);

        draw_everything(window, players, serverBullets, clientBullets);
    }
}
