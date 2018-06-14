#include "server.h"
#include <sstream>
#include <cassert>
#include <algorithm>
#include <numeric>

void set_shooting_dir(int &shooting_dir)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
            shooting_dir=0;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
            shooting_dir=1;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
            shooting_dir=2;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
        || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
            shooting_dir=3;
    }
}
/*


  std::vector<int> v = {1,2,3,4,5,6};
  std::swap(v[3], v.back());
  v.pop_back();
*/
void bulletHit(std::vector<bullet> &bullets, std::vector<player> players, const int celSize)
{
      int playerCelX =players[0].getPosX()/celSize;
      int playerCelY = players[0].getPosY()/celSize;

      for(unsigned int i=0; i<bullets.size(); ++i)
      {
           int bulletCelX= bullets[i].getPosX()/celSize;
           int bulletCelY= bullets[i].getPosY()/celSize;
           if(playerCelX==bulletCelX && playerCelY==bulletCelY)
           {
               std::swap(bullets[i], bullets.back());
               bullets.pop_back();
               std::cout<<"HIT HIT HIT HIT!!!!!!!!!!!!!!!!!!!!! \n"<<std::flush;
           }
      }


      //std::pair<int, int> playerGridPos{playerPosX, playerPosY};
}

bool fire_able(int time)
{
    time+=1;
    if(time>30)
    {
        return true;
    }
    else
    {
        return false;
    }
}
void receive_bullet_created(sf::Packet bulletPacket,std::vector<bullet> &bullets, std::vector<player> &players)
{
    std::string messageType=" ";
    int speedX=0;
    int speedY=0;
    if(bulletPacket>>messageType)
    {
        if(messageType=="bulletCreated")
        {
            bullets.push_back(bullet(10,10, players[1].getPosX(), players[1].getPosY(), speedX, speedY));
        }
    }
}

void set_player_position(std::vector<player> &players, sf::Packet posPacket)
{
    sf::Vector2f changingPosition;
    std::string messageType;

    if(posPacket>>changingPosition.x>>changingPosition.y>> messageType) //>> objectNumber -> needs to be added
    {
        if(messageType=="player")
        {
            players[1].setPlayerPosition(changingPosition.x, changingPosition.y);
        }
    }
}

void set_bullet_position(std::vector<bullet> &bullets, sf::Packet posPacket,std::string bulletText)
{
    sf::Vector2f changingPosition;
    std::ostringstream messageType;
    std::string messageReceived;
    for(unsigned int i=0; i<bullets.size(); ++i)
    {
        if(posPacket>>changingPosition.x>>changingPosition.y>> messageReceived)
        {
            messageType<<bulletText<<i;
            if(messageType.str()==messageReceived)
            {
                bullets[i].setBulletPosition(changingPosition.x+10, changingPosition.y+10);
            }
        }
    }
}


/*

packet << static_cast<sf::Uint32>(list.size());
for (std::list<xxx>::const_iterator it = list.begin(); it != list.end(); ++it)
    packet << *it;

const auto pi = 3.14;

std::vector<int> v;
packet << v.size();
//for (const auto e: v) packet << e;
const int size = v.size();
for (int i=0; i!=size; ++i)
{
  const int e = v[i];
  packet << e;
}
 */

void send_position_bullet(const sf::IpAddress ip, const unsigned short port,
                          std::vector<bullet> &bullets)
{    
    std::ostringstream bulletMessage;
    std::string bulletText="bullet";

    //std::vector<sf::Vector2f> bulletPositions;

    sf::UdpSocket socket;
    sf::Packet posPacket;
    if(bullets.size()>0)
    {
        //packet << static_cast<sf::Uint32>(bullets.size());
        for (unsigned i=0; i<bullets.size(); i++)
        {
            bulletMessage<<bulletText<<i;
            posPacket<<bullets[i].getPosX() << bullets[i].getPosY() << bulletMessage.str();
        }
        if (socket.send(posPacket, ip, port) != sf::Socket::Done)
        {
            //std::cout<<"whoops... some data wasn't sent";
        }
    }
}


void draw_everything(sf::RenderWindow &window, std::vector<player> &players, std::vector<bullet> &serverBullets, std::vector<bullet> &clientBullets, const std::string role, const int celSize)
{
    window.clear();


    for(int i=0; i<static_cast<int>(clientBullets.size()); ++i)
    {
       clientBullets[i].display(window);
    }
    for(int i=0; i<static_cast<int>(serverBullets.size()); ++i)
    {
       serverBullets[i].display(window);
    }

    if(role == "s" || role =="spectator")
    {
        for(int i=0; i<static_cast<int>(players.size()); ++i)
        {
            players[i].display(window);
        }
    }
    if(role == "p" || role =="player")
    {
            players[0].display(window);
    }
    //Grid
    for(int i=0; i<17; ++i)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(celSize*i, 0)),
            sf::Vertex(sf::Vector2f(celSize*i, 500))
        };
        sf::Vertex line2[] =
        {
            sf::Vertex(sf::Vector2f(0, celSize*i)),
            sf::Vertex(sf::Vector2f(500, celSize*i))
        };
        window.draw(line, 2, sf::Lines);
        window.draw(line2, 2, sf::Lines);
    }
    window.display();
}

void receive_position_packets(sf::UdpSocket &socket, std::vector<player> &players, std::vector<bullet> &bullets, std::pair<int, sf::IpAddress> playerPortIp)
{

    std::string bulletText ="bullet";
    sf::IpAddress sender;
    unsigned short port;
    sf::Packet posPacket;

    for(int i=0; i<3; ++i)
    {
        if (socket.receive(posPacket,sender,port) != sf::Socket::Done)
        {
            //playerPortIp={port, sender};
            //std::cout<<playerPortIp.second;
            //std::cout<<"whoops... some data wasn't received";
        }

        set_player_position(players, posPacket);

        set_bullet_position(bullets, posPacket, bulletText);
        receive_bullet_created(posPacket, bullets, players);
    }

}
void send_position_packets(sf::UdpSocket &socket, std::vector<player> &players, std::vector<bullet> &bullets)
{

}

/// player::has_walked() const noexcept { return getPos() != getPrevPos(); }
bool player_check_walking(const std::vector<player> &players, sf::Vector2f prevPosition)
{
    if(prevPosition.x!=players[0].getPosX() || prevPosition.y!=players[0].getPosY())
    {
        return true;
    }
    else
    {
        return false;
    }
}


/// shoot_bullets
void shoot_bullet(std::vector<bullet> &bullets,sf::IpAddress &ip, unsigned short &port, std::vector<player> &players,
                  bool &update, int &time, const int shooting_dir)
{
    int bulletSpeedX=3;
    int bulletSpeedY=0;
    switch(shooting_dir)
    {
        case 0:
        bulletSpeedX=3;
        bulletSpeedY=0;
        break;
        case 1:
        bulletSpeedX=-3;
        bulletSpeedY=0;
        break;
        case 2:
        bulletSpeedX=0;
        bulletSpeedY=-3;
        break;
        case 3:
        bulletSpeedX=0;
        bulletSpeedY=3;
        break;
    }
     sf::Packet posPacket;
     if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && update==true && fire_able(time)==true)
     {
         bullets.push_back(bullet(10,10, players[0].getPosX()+10, players[0].getPosY()+10, bulletSpeedX, bulletSpeedY));
         std::string bulletMessage ="bulletCreated";
         sf::UdpSocket socket;
         posPacket<<bulletMessage;

         if (socket.send(posPacket, ip, port) != sf::Socket::Done)
         {
             //std::cout<<"whoops... some data wasn't sent";
         }

         time=0;
     }
     if(bullets.size()>0)
     {
        for(unsigned int i=0; i<bullets.size(); ++i)
        {
            bullets[i].moveBullet();
        }
     }
}

void server_receive_ip_port(sf::TcpSocket &TcpSocket, sf::TcpListener &listener, unsigned short &clientPort, std::vector<unsigned short> &vectorClientPorts, bool &clientConnecting)
{
    std::string messageType;
        std::pair<std::string, int> playerType;
    sf::Packet cIPandPortPacket;
    std::string cIP;
    sf::IpAddress clientIpReceived;

    listener.accept(TcpSocket);

    if(TcpSocket.receive(cIPandPortPacket)==sf::Socket::Done)
    {
        if(cIPandPortPacket>>messageType)
        {
            if(messageType=="Port")
            {
                std::cout<<"BOOP BEEP BOOP BOOP BEEP!!!!!!!!!!!!"<<std::flush;
                if(cIPandPortPacket>>playerType.first>>playerType.second)
                {
                    std::cout<<"BOOP    " << playerType.first ;
                }
            }

            if(messageType=="IpPort")
            {
                std::cout<<"fdskljfsdlakjfdkslajfklsdajkflsdajklfdsajakl";
                if(cIPandPortPacket>>cIP>>clientPort)
                {
                    std::cout<<"BEEP";
                    clientConnecting=true;
                    std::cout<<cIP<<"\n";
                    clientIpReceived=cIP;
                    std::cout<<clientPort<<"\n";
                    vectorClientPorts.push_back(clientPort);
                }
            }

        }
    }
}
void server_receive_playerType(sf::TcpSocket &TcpSocket, sf::TcpListener &listener)
{
    std::string messageType;
    std::pair<std::string, int> playerType;
    sf::Packet playerTypePacket;

    listener.accept(TcpSocket);

    if(TcpSocket.receive(playerTypePacket)==sf::Socket::Done)
    {
        if(playerTypePacket>>messageType);
        {
            if(messageType=="playerType")
            {
            if(playerTypePacket>>playerType.first>>playerType.second)
            {

                std::cout<<"BOOP";
            }
                //std::cout<<"role: "<<playerType.first<<"\n";
                //std::cout<<"team: "<<playerType.second<<"\n";
            }
        }
    }
}
void send_request_team_role()
{
    sf::TcpSocket TcpSocket;
    sf::Packet team_role;
    sf::TcpListener listenerForServer;
    listenerForServer.setBlocking(false);

    //listenerForClients.listen(clientPort);
    //TcpSocket.connect(serverIP,2000);
    //cIPandPortPacket<<cIP<<clientPort;
    //TcpSocket.send(cIPandPortPacket);

}

void client_send_ip_port(std::string cIP, unsigned short clientPort, sf::IpAddress serverIP)
{
    std::string messageType="IpPort";
    sf::TcpSocket TcpSocket;
    sf::Packet cIPandPortPacket;
    sf::TcpListener listenerForClients;
    listenerForClients.setBlocking(false);

    listenerForClients.listen(clientPort);
    TcpSocket.connect(serverIP,2000);
    cIPandPortPacket<<messageType<<cIP<<clientPort;
    TcpSocket.send(cIPandPortPacket);
}

std::vector<player> makePlayers(int& amount_players) noexcept
{
    std::vector<player> newPlayer;
    for(int i=0; i!= amount_players; ++i)
    {
        newPlayer.push_back(player(30,30,0,0));
    }
    return newPlayer;
}

void send_player_position(sf::IpAddress ip, std::vector<unsigned short> ports, const std::vector<player> &players)
{

    std::string playerMessage ="player";
    sf::UdpSocket socket;
    sf::Packet posPacket;
    posPacket<<players[0].getPosX() <<players[0].getPosY()<< playerMessage;//<<playerNumber;

    for(unsigned i=0; i<ports.size(); ++i)
    {
        if (socket.send(posPacket, ip, ports[i]) != sf::Socket::Done)
        {
            //std::cout<<"whoops... some data wasn't sent";
        }
    }
}

void send_client_player_position(sf::IpAddress ip, unsigned short port, const std::vector<player> &players)
{

    std::string playerMessage ="player";
    sf::UdpSocket socket;
    sf::Packet posPacket;
    posPacket<<players[0].getPosX() <<players[0].getPosY()<< playerMessage;//<<playerNumber;

    if (socket.send(posPacket, ip, port) != sf::Socket::Done)
    {
        //std::cout<<"whoops... some data wasn't sent";
    }
}
void playerWalking(std::vector<player> &players, bool &update, int &time, const int celSize)
{
    float posX=players[0].getPosX();
    float posY=players[0].getPosY();

    if(update==true&& time>10)
    {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                    time=0;
                    posX+=celSize;
                    players[0].setPlayerPosition(posX, posY);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                   time=0;
                    posX-=celSize;
                   players[0].setPlayerPosition(posX, posY);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            {
                time=0;
                posY-=celSize;
                players[0].setPlayerPosition(posX, posY);

            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)
                || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                time=0;
                posY+=celSize;
                players[0].setPlayerPosition(posX, posY);
            }
        }
}

void do_server(std::vector<player> &players,std::pair<std::string,int> playerType, bool &update, sf::RenderWindow &window)
{
    const std::string role =playerType.first;
    std::pair<int,sf::IpAddress> playerPortIp;
    const int celSize =30;
    int timeWalking=10;
    int timeShooting=30;
    bool clientConnecting =false;
    std::vector<bullet> serverBullets{};
    std::vector<bullet> clientBullets{};
    sf::Event Event;
    sf::Vector2f prevPosition;
    sf::UdpSocket socket;
    unsigned short clientPort;
    socket.bind(2000);
    std::vector<unsigned short> vectorClientPorts;

    sf::IpAddress clientIP=sf::IpAddress::getLocalAddress();
    std::string cIP=clientIP.sf::IpAddress::toString();

    socket.setBlocking(false);

    sf::TcpSocket TcpSocket;
    sf::TcpListener listener;
    int shooting_dir=0;
    listener.listen(2000);
    listener.setBlocking(false);
    TcpSocket.setBlocking(false);

    while(window.isOpen())
    {
    window_events(window, Event, update); //selecting and deselecting the window and if the user presses escape, the window closes

    ++timeWalking;
    ++timeShooting;
    server_receive_ip_port(TcpSocket, listener, clientPort, vectorClientPorts, clientConnecting);
    //server_receive_playerType(TcpSocket, listener);
    if(role =="p" || role =="player")
    {
        prevPosition = sf::Vector2f(players[0].getPosX(), players[0].getPosY());
        playerWalking(players, update, timeWalking, celSize);

        sf::IpAddress recipient = clientIP;

        if(player_check_walking(players, prevPosition))
        {
             send_player_position(recipient, vectorClientPorts, players);
        }
        set_shooting_dir(shooting_dir);

        shoot_bullet(serverBullets, recipient, clientPort,players, update, timeShooting, shooting_dir);

        send_position_bullet(recipient, clientPort, serverBullets);

        bulletHit(clientBullets, players, celSize);
     }

        receive_position_packets(socket, players, clientBullets, playerPortIp);

    draw_everything(window, players, serverBullets, clientBullets, role, celSize);
   }
}
