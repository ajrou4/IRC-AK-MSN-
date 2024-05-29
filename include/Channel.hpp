/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haguezou <haguezou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/26 10:50:47 by majrou            #+#    #+#             */
/*   Updated: 2024/05/29 19:08:32 by haguezou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include "ircserver.hpp"

class Client; // we use forward declaration to avoid circular dependency (thats means we include client.hpp in Channel.cpp because we use Client class in Channel.cpp and Client is not yet defined in Channel.hpp)
class Server;
class Channel
{
    private:
        std::string                 name;
        std::string                 key;
        std::string                 topic;
        int                         userLimit;
        bool                        inviteOnly;
        std::vector<std::string >   inviteUser2;
        std::vector<Client>         users;
        std::vector<Client>         oper;
        Server                      *server; // for server

        bool    isUserInChannel(std::string& username);
        bool    isOperator(std::string& username);
        bool    isUserInvited( std::string& username);

    public:
            Channel();
            Channel(std::string const &chName);
            Channel(const Channel &src);
            Channel& operator=(const Channel &src);
            ~Channel();

            void addUser(Client& client);
            void addOperator(Client& client);
            void removeUser(Client& client);
            
            void kickUser(const std::string &userName);
            void setMode(std::string &mode);
            void inviteUser(std::string userName);
            void setTopic(const std::string &topic);
            // void joinChannel(Client& client);
            std::string  const &getName()const;
            const std::vector<Client>  &getUsers()const;
            const std::string &getMode()const;
            bool hasUser(Client &client);
            const std::string  &getTopic() const;

            void sendPublicMessage(int from_socket, const std::string &message);
            void sendPrivateMessage(int from_socket, int to_socket, const std::string &message);

            bool    hasClient(int fd) const;
            
            void    broadcastMessage(const std::string& message, int from_socket);
};

#endif
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: majrou <majrou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/26 10:50:47 by majrou            #+#    #+#             */
/*   Updated: 2024/05/26 10:50:48 by majrou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #ifndef CHANNEL_HPP
// #define CHANNEL_HPP

// #include <iostream>
// #include <vector> //-> for vector
// #include <algorithm>
// #include <sys/socket.h> //-> for socket()
// #include <sys/types.h> //-> for socket()
// #include <netinet/in.h> //-> for sockaddr_in
// #include <fcntl.h> //-> for fcntl()
// #include <unistd.h> //-> for close()
// #include <arpa/inet.h> //-> for inet_ntoa()
// #include <poll.h> //-> for poll()
// #include <csignal> //-> for signal()
// #include <string>
// #include <netdb.h>
// #include <cstring>
// #include "client.hpp"



// class Channel
// {
//     private:


//         std::string name;
//         std::string key;
//         std::string topic;
//         int userLimit;
//         bool inviteOnly;
//         std::vector<std::string > inviteUser2;
//         // std::vector<Client> users;
//         // std::vector<Client> oper;

//         bool isUserInChannel(std::string& username);
//         bool isOperator(std::string& username);
//         bool isUserInvited( std::string& username);
//     public:


//             Channel();
//             Channel(std::string const &chName);
//             Channel(const Channel &src);
//             Channel& operator=(const Channel &src);
//             ~Channel();

//             void addUser(Client& client);
//             void addOperator(Client& client);
//             void kickUser(const std::string &userName);
//             void setMode(std::string &mode);
//             void inviteUser(std::string userName);
//             void setTopic(const std::string &topic);
//             // void joinChannel(Client& client);
            
//             std::string getTopic();

//             void sendPublicMessage(int from_socket, const std::string &message);
//             void sendPrivateMessage(int from_socket, int to_socket, const std::string &message);
// };

// #endif