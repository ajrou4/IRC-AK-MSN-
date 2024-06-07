/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/20 00:45:48 by omakran           #+#    #+#             */
/*   Updated: 2024/06/06 23:00:41 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/client.hpp"

Client::Client(int _fd, std::string _ip, std::string _hostname)
: fd(_fd), ip(_ip), hostname(_hostname), inboundBuffer(""), outboundBuffer(""), realname("unknown"), authenticated(false), registered(false) {
    if (hostname.empty()) {
        hostname = ip;
    }
}

Client::~Client() {}

bool    Client::outBoundReady() const {
    return outboundBuffer.str().size(); // check if there's data in the buffer
}

std::vector<std::string> Client::splitMessage(const std::string& message) {
    std::vector<std::string>    result; 
    std::stringstream stream(message); // create a stream from the message
    std::string                 token;

    // split the message by whitespace and store each part in the `result` vector.
    while (stream >> token) {
        //  if there's data to read from the stream
        result.push_back(token);
    } 
    return result;
}

void    Client::handleMessage(const std::string& message) {
    std::cout << "Handling message: " << message << std::endl;
    std::vector<std::string>    parts = splitMessage(message);

    if (parts.empty())
        return ; // ignore empty messages.

    //                          extract the command from the message
    std::string                 command = parts[0];
    if (command == "NICK") {
        if (parts.size() > 1)
            handleNick(parts[1]);
    } else if (command == "USER") {
        if (parts.size() > 1)
            handleUser(parts[1]);
    } else if (command == "JOIN") {
        if (parts.size() > 1)
            handleJoin(parts[1]);
    } else if (command == "PRIVMSG") {
        if (parts.size() > 2)
            handlePrivmsg(parts[1], message.substr(message.find(parts[2])));
    } 
    else {
        std::cerr << "Unknowon command: " << command << std::endl;
    }
}

void    Client::handleNick(const std::string& nick) {
    nickname = nick; // set the client's nickname.
    std::cout << "Nickname set to : " << nickname <<std::endl;
}

void    Client::handleUser(const std::string& user) {
    username = user;
    std::cout << "Username set to: " << username << std::endl;
}

void    Client::handleJoin(const std::string& channel) {
    channels.push_back(channel); // add the client to the channel
    std::cout << "Joined channel: " << channel << std::endl;
}

void    Client::handlePrivmsg(const std::string& target, const std::string& message) {
    // print the private message to the target.
    std::cout << "Private messge to " << target << ": " << message << std::endl; 
}

const std::string&   Client::getRealName() const {
    return realname;
}

const std::string&   Client::getUserName() const {
    return username;
}

const std::string&   Client::getHostname() const {
    return hostname;
}

int Client::getFd() const {
    return fd;
}

bool    Client::isAuthenticated(void) const {
    return authenticated;
}

void    Client::setAuthenticated(bool authenticat) {
    authenticated = authenticat;
}

void    Client::newMessage(const std::string &message) {
    outboundBuffer << message << "\r\n"; // append the message to the buffer
}

void    Client::appendToInboundBuffer(std::string data) { // data is comming from a client
    inboundBuffer << data;
}

std::string Client::getOutboundBuffer() {
    return outboundBuffer.str();
}

void    Client::advOutboundBuffer(size_t n) {
    std::string data = outboundBuffer.str(); // read the data from the buffer
    outboundBuffer.str(data.substr(n)); // remove the data from the buffer
}

bool    Client::inboundReady() const {
    return inboundBuffer.str().find("\r\n") != std::string::npos;
}

bool    Client::isRegistered() const {
    return registered;
}

void    Client::setRegistered(bool registered) {
    this->registered = registered;
}

void    Client::setNick(const std::string& nick) {
    nickname = nick;
}

std::vector<std::string> Client::splitCommands() {
    std::vector<std::string>    result;
    std::string                 line = inboundBuffer.str();  // read a line from the buffer
    size_t                      pos; // find the end of the line
    while ((pos = line.find("\r\n")) != std::string::npos) {
        std::string command = line.substr(0, pos); // extract the command
        if (command.size() > 0) {
            result.push_back(line.substr(0, pos));
        }
        line.erase(0, pos + 2); // remove the command from the buffer
    }
    inboundBuffer.str(line); // update the buffer
    return result;
}

void    Client::setRealName(std::string realName) {
    this->realname = realName;
}

void    Client::setUserName(std::string userName) {
    this->username = userName;
}

const std::string&    Client::getNick() const {
    return nickname;
}

std::string Client::getIdentifierNetwork() const {
    return nickname + "!" + username + "@" + hostname;
}

std::string Client::intro() const {
    return ":" + getIdentifierNetwork() + " ";
}