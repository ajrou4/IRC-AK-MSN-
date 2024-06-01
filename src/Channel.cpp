/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/26 10:51:02 by majrou            #+#    #+#             */
/*   Updated: 2024/06/02 00:19:38 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "client.hpp"

Channel::Channel() : name(""), password(""), userLimit(0), mode(0), server(NULL) {}

Channel::Channel(std::string Name, std::string password, Server *server):
name(Name), password(password), userLimit(0), mode(0), server(server) {
    if (!password.empty()) {
        mode |= (1 << Key); // set the password mode
    } else {
        mode &= ~(1 << Key); // unset the password mode
    }
    if (name[0] != '#') {
        name = "#" + name;
    }
}

Channel::Channel(std::string const &chName){
    this->name = chName;
}

Channel::Channel(const Channel &src){
    *this = src;
}

Channel &Channel::operator=(const Channel &src)
{
    if(this!= &src)
       {
            this->name =src.name;
            this->key = src.key;
            this->topic = src.topic;
            this->userLimit = src.userLimit;
            this->inviteOnly = src.inviteOnly;
       } 
    
    return *this;
}

Channel::~Channel(){}

// void Channel::addUser(Client& client){
//     if(users.size() == userLimit) {
//         std::cout << "Channel is full!"<<std::endl;
//         return;
//     }
//     if(inviteOnly){
//         std::vector<std::string>::iterator it = std::find(this->inviteUser2.begin(), this->inviteUser2.end(), client);
//         if(it == inviteUser2.end())
//         {
//             std::cout << "User " << client.getUserName()  << " is not invited to the channel " << this->name << std::endl;
//             return;
//         }
//     }
//     users.push_back(client);
//     inviteUser2.erase(std::remove(inviteUser2.begin(), inviteUser2.end(), client), inviteUser2.end());
// }

void    Channel::addOperator(int socket) {
    if (!isOperator(socket)) {
        // add the client to the channel
        operators.push_back(socket);
    } else {
        std::cerr << "Client already in the channel" << std::endl;
    }
}

// void Channel::kickUser(const std::string &userName){
//     std::vector<Client>::iterator it = users.begin();
//     while(it != users.end())
//     {
//         if(it->getUserName() == userName)
//         {
//             users.erase(it);
//             std::cout << "User "<<userName<<"has been kicked form channel"<< this->name <<std::endl;
//             break;
//         }
    
//     else{
//         std::cout << "User "<<userName<<"is not  in the channel"<< this->name <<std::endl;
//     }
//     it++;
//     }
// }

void Channel::setMode(std::string &mode){
    if(mode == "i")
        inviteOnly = true;
    else if(mode == "-i")
        inviteOnly = false;
    else if(mode == "k")
        key = mode;
    else if(mode == "-k")
        key = "";
    else if(mode == "t")
        topic = mode;
    else if(mode == "-t")
        topic = "";
    else if(mode == "l")
        userLimit = std::stoi(mode);
    else if(mode == "-l")
        userLimit = 0;
    else if (mode == "o")
        
        // take channel operator privilege
        std::cout << "Operator mode"<<std::endl;
    else if(mode == "-o")
        // remove channel operator privilege
        std::cout << "Operator mode"<<std::endl;
    else
        std::cout << "Mode not found"<<std::endl;
}

void Channel::inviteUser(std::string userName){
    inviteUser2.push_back(userName);
    std::cout << "User "<<userName<<"has been invited to the channel"<< this->name <<std::endl;
}

void Channel::setTopic(const std::string &topic){
    this->topic = topic;
}
const std::string &Channel::getTopic()const{
    return this->topic;
}

bool    Channel::isOperator(int fd) {
    std::vector<int>::iterator it = operators.begin();
    while (it != operators.end()) {
        if (*it == fd) {
            return true;
        }
        it++;
    }
    return false;
}

std::string const &Channel::getName()const{
    return this->name;
}

const std::vector<int>  &Channel::getUsers()const{
    return clients;
}

bool    Channel::getMode(ChannelMode KEY)const {
    return mode & (1 << KEY); // check if the mode is set.
}

const std::string Channel::getPassword() const {
    return password;
}

void    Channel::addClient(int fd) {
    if (!hasClient(fd)) {
        // add the client to the channel
        clients.push_back(fd);
    } else {
        std::cerr << "Client already in the channel" << std::endl;
    }
}

void    Channel::removeInv(int fd) {
    std::vector<int>::iterator it = clients.begin();
    while (it != invites.end()) {
        if (*it == fd) {
            // remove the client from the channel
            invites.erase(it);
            break;
        }
        it++;
    }
}   

bool    Channel::hasPlusV(int fd) {
    std::vector<int>::iterator it = invites.begin();
    while (it != invites.end()) {
        if (*it == fd) {
            return true;
        }
        it++;
    }
    return false;
}

bool    Channel::hasClient(int fd) const {
    std::vector<int>::const_iterator it = clients.begin();
    while (it != clients.end()) {
        if (*it == fd) {
            return true;
        }
        it++;
    }
    return false;
}

void    Channel::broadcastMessage(std::string message) {
    for (std::vector<int>::iterator it = clients.begin(); it != clients.end(); it++) {
        // send the message to all clients in the channel
        server->sendMessageCommand(*it, message);
    }
}

void    Channel::brodcastMessage(std::string message, int fd) {
    for (std::vector<int>::iterator it = clients.begin(); it != clients.end(); it++) {
        // send the message to all clients in the channel except the sender
        if (*it != fd) {
            server->sendMessageCommand(*it, message);
        }
    }
}

std::string Channel::getModes() const {
    std::string modes = "+n";
    if (getMode(invit_ONLY)) {
        modes += "i";
    } if (getMode(Key)) {
        modes += "k";
    } if (getMode(ToPic)) {
        modes += "t";
    } if (getMode(Limit)) {
        modes += "l";
    } if (getMode(Moderated)) {
        modes += "m";
    } if (getMode(Secret)) {
        modes += "s";
    }
    return modes;
}

void    Channel::removeClient(int fd) {
    std::vector<int>::iterator it = clients.begin();
    while (it != clients.end()) {
        if (*it == fd) {
            // remove the client from the channel
            clients.erase(it);
            break;
        }
        it++;
    }
}

void    helperOperator(Channel &channel, Client &client, Server &server) {
    int fd = client.getFd();
    if (channel.isOperator(fd) && channel.getUsers().size() == 1) {
        std::vector<int> users = channel.getUsers();
        for (size_t i = 0; i < users.size(); i++) {
            if (users[i] != fd) {
                channel.addOperator(users[i]);
                channel.broadcastMessage(":" + client.getNick() + "!" + client.getUserName() + "@" + client.getHostname() + " MODE " + channel.getName() + " +o " + server.getClient(users[i]).getNick());
                break;
            }
        }
    }
}