#ifndef SERVER_H
#define SERVER_H
#include "player.h"
#include "bullet.h"
#include "iostream"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <string>
#include <map>
//#include <conio.h>

void receive_bullet_created(sf::Packet bulletPacket, int currentAmountOfBullets);
void set_player_position(std::vector<player> &players, sf::Packet posPacket);
void set_bullet_position(std::vector<bullet> &bullets, sf::Packet posPacket, int currentAmountOfBullets);
void send_position_bullet(sf::IpAddress ip, unsigned short port, std::vector<bullet> &bullets);
void receive_tcp_messages(sf::TcpSocket &socket, sf::TcpListener &listener);
void draw_everything(sf::RenderWindow &window, std::vector<player> &players, std::vector<bullet> &bullets);
void receive_position_packets(sf::UdpSocket &socket, std::vector<player> &players,
                              std::vector<bullet> &bullets, int currentAmountOfBullets);
bool player_check_walking(std::vector<player> &players,sf::Vector2f prevPosition);
void shoot_bullet(int &currentAmountOfBullets, std::vector<bullet> &bullets, float &bulletX, sf::IpAddress &ip, unsigned short &port);
void server_receive_ip_port(sf::TcpSocket &TcpSocket, sf::TcpListener &listener,
                            unsigned short &clientPort);
void client_send_ip_port(std::string cIP, unsigned short clientPort, sf::IpAddress serverIP);
std::vector<player> makePlayers(int& amount_players) noexcept;
std::vector<bullet> makeBullets(int& amount_bullets) noexcept;
void send_position(sf::IpAddress ip, unsigned short port, std::vector<player> &players);
void playerWalking(std::vector<player> &players, bool &update);
void do_server(bool &initializing,std::vector<player> &players, bool &update, sf::RenderWindow &window);

#endif // SERVER_H
