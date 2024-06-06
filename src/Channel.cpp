/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 22:45:45 by haguezou          #+#    #+#             */
/*   Updated: 2024/06/06 23:50:52 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"
#include "client.hpp"

Channel::Channel() : name(""), password(""), userLimit(0), mode(0), server(NULL) {}

Channel::Channel(std::string Name, std::string pass, Server *server):
name(Name), password(pass), userLimit(0), mode(0), server(server) {
    if (!password.empty()) {
        setMode(Key, true);
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

void Channel::setTopic(std::string topic){
    this->topic = topic;
}
const std::string &Channel::getTopic()const{
    return this->topic;
}

bool    Channel::isOperator(int fd) const {
    return std::find(operators.begin(), operators.end(), fd) != operators.end();
}

int Channel::getCountOperator() const{
    return operators.size();
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
    if (!hasInvet(fd)) {
        // add the client to the channel
        invites.push_back(fd);
    }
}

bool    Channel::hasInvet(int fd) const {
    return std::find(invites.begin(), invites.end(), fd) != invites.end();
}

bool    Channel::hasPlusV(int fd) {
    return std::find(plusVoices.begin(), plusVoices.end(), fd) != plusVoices.end();
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
    std::vector<int>::iterator it = std::find(plusVoices.begin(), plusVoices.end(), fd);
    if (it != plusVoices.end()) {
        // remove the client from the channel
        plusVoices.erase(it);
    }
}

void    Channel::setUserLimit(int limit) {
    userLimit = limit;
}

int     Channel::getCountClient() const {
    return clients.size();
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
    return std::find(clients.begin(), clients.end(), fd) != clients.end();
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

std::string Channel::getClientsNicks() const {
    std::string nicks;
    for (std::vector<int>::const_iterator it = clients.begin(); it != clients.end(); it++) {
        if (isOperator(*it)) { // check if the client is an operator
            nicks += "@" + server->getClient(*it).getNick() + " ";
        } else {
            nicks += server->getClient(*it).getNick() + " ";
        }
    }
    return nicks;
}

std::string Channel::getModes() const {
    std::string modes = "+n";
    if (getMode(invit_ONLY))
        modes += "i";
    if (getMode(Key))
        modes += "k";
    if (getMode(ToPic))
        modes += "t";
    if (getMode(Limit))
        modes += "l";
    if (getMode(Moderated))
        modes += "m";
    if (getMode(Secret))
        modes += "s";
    return modes;
}

int Channel::getLimit() const {
    return userLimit;
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
    if (channel.isOperator(fd) && channel.getCountOperator() == 1) {
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