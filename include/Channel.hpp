#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <iostream>
#include <vector> //-> for vector
#include <algorithm>
#include <sys/socket.h> //-> for socket()
#include <sys/types.h> //-> for socket()
#include <netinet/in.h> //-> for sockaddr_in
#include <fcntl.h> //-> for fcntl()
#include <unistd.h> //-> for close()
#include <arpa/inet.h> //-> for inet_ntoa()
#include <poll.h> //-> for poll()
#include <csignal> //-> for signal()
#include <string>
#include <netdb.h>
#include <cstring>
#include "client.hpp"

class Channel
{
    private:


        std::string name;
        std::string key;
        std::string topic;
        int userLimit;
        bool inviteOnly;
        std::vector<std::string > inviteUser2;
        std::vector<Client> users;
        std::vector<Client> oper;
    public:


            Channel();
            Channel(std::string const &chName);
            Channel(const Channel &src);
            Channel& operator=(const Channel &src);
            ~Channel();

            void addUser(Client& client);
            void addOperator(Client& client);
            void kickUser(const std::string &userName);
            void setMode(std::string &mode);
            void inviteUser(std::string userName);
            void setTopic(const std::string &topic);

            std::string getTopic();

            void sendPublicMessage(int from_socket, const std::string &message);
            void sendPrivateMessage(int from_socket, int to_socket, const std::string &message);
};

#endif