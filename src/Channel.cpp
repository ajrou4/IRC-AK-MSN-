/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haguezou <haguezou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 22:45:45 by haguezou          #+#    #+#             */
/*   Updated: 2024/06/03 22:45:46 by haguezou         ###   ########.fr       */
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

void    Channel::addOperator(int socket) {
    if (!isOperator(socket)) {
        // add the client to the channel
        operators.push_back(socket);
    } else {
        std::cerr << "Client already in the channel" << std::endl;
    }
}

void Channel::setMode(ChannelMode KEY, bool value){
    if(value){
        mode |= (1 << KEY); // set the mode, means the mode is on
    }
    else{
        mode &= ~(1 << KEY); // unset the mode, means the mode is off
    }
}

void Channel::setPassword(const std::string password){
    this->password = password;
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
    std::vector<int>::iterator it = invites.begin();
    while (it != invites.end()) {
        if (*it == fd) {
            // remove the client from the channel
            invites.erase(it);
            break;
        }
        it++;
    }
}   

void    Channel::addInv(int fd) {
    if (!hasClient(fd)) {
        // add the client to the channel
        invites.push_back(fd);
    } else {
        std::cerr << "Client already in the channel" << std::endl;
    }
}

bool    Channel::hasPlusV(int fd) {
    std::vector<int>::iterator it = plusVoices.begin();
    while (it != plusVoices.end()) {
        if (*it == fd) {
            return true;
        }
        it++;
    }
    return false;
}

void    Channel::addPlusV(int fd) {
    if (!hasPlusV(fd)) {
        // add the client to the channel
        plusVoices.push_back(fd);
    } else {
        std::cerr << "Client already in the channel" << std::endl;
    }
}

void    Channel::removePlusV(int fd) {
    std::vector<int>::iterator it = plusVoices.begin();
    while (it != plusVoices.end()) {
        if (*it == fd) {
            // remove the client from the channel
            plusVoices.erase(it);
            break;
        }
        it++;
    }
}

void    Channel::setUserLimit(int limit) {
    userLimit = limit;
}

void    Channel::removeOperator(int fd) {
    std::vector<int>::iterator it = operators.begin();
    while (it != operators.end()) {
        if (*it == fd) {
            // remove the client from the channel
            operators.erase(it);
            break;
        }
        it++;
    }
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