/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircImplementation.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: majrou <majrou@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 17:36:11 by omakran           #+#    #+#             */
/*   Updated: 2024/05/26 23:59:31 by majrou           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserver.hpp"
#include "client.hpp"


void Server::sendCommand(int socket, const std::string& command){
        std::string comNewLine = command + "\r\n";
        size_t comlength= comNewLine.length();
        ssize_t byteSent= send(socket, comNewLine.c_str(), comlength, 0);
        if(byteSent = -1)
                std::cerr<< " Error sending command: "<< strerror(errno)<<std::endl;
        else if(byteSent < static_cast<ssize_t>(comlength))
                std::cerr << "Command is sent "<< comlength<<" but only sent : "<< byteSent<< " byte"<<std::endl;

}

void Server:: PASS(int socket, const std::string& pass) {
    sendCommand(socket, "PASS " + pass);
}

void Server:: NICK(int socket, const std::string& nickname) {
    sendCommand(socket, "NICK " + nickname);
}

void Server:: USER(int socket, const std::string& user) {
    sendCommand(socket, "USER " + user);
}

void Server:: LIST(int socket, const std::string& list) {
    sendCommand(socket, "LIST " + list);
}

void Server:: JOIN(int socket, const std::string& join) {
    sendCommand(socket, "JOIN " + join);
}

void Server:: PART(int socket, const std::string& part) {
    sendCommand(socket, "PART " + part);
}

void Server:: WHO(int socket, const std::string& who) {
    sendCommand(socket, "WHO " + who);
}

void Server:: WHOIS(int socket, const std::string& whois) {
    sendCommand(socket, "WHOIS " + whois);
}

void Server:: PING(int socket, const std::string& ping) {
    sendCommand(socket, "PING " + ping);
}

void Server:: PRIVMSG(int socket, const std::string& privmsg) {
    sendCommand(socket, "PRIVMSG " + privmsg);
}

void Server:: QUIT(int socket, const std::string& quit) {
    sendCommand(socket, "QUIT " + quit);
}

void Server:: KICK(int socket, const std::string& kick) {
    sendCommand(socket, "KICK " + kick);
}

void Server:: INVITE(int socket, const std::string& invite) {
    sendCommand(socket, "INVITE " + invite);
}

void Server:: TOPIC(int socket, const std::string& topic) {
    sendCommand(socket, "TOPIC " + topic);
}

void Server:: ISON(int socket, const std::string& ison) {
    sendCommand(socket, "ISON " + ison);
}

void Server:: MODE(int socket, const std::string& mode) {
    sendCommand(socket, "MODE " + mode);
}

