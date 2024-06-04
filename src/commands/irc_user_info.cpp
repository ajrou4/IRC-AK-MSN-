/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_user_info.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 18:46:00 by haguezou          #+#    #+#             */
/*   Updated: 2024/06/04 21:07:32 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "ircserver.hpp"


/* ----------------------------  WHO command ------------------------------
                Provides information about users.
                If no parameters are given, it lists all visible users.
   ------------------------------------------------------------------------- */
//      lists users matching certain criteria.
void    Server::WHO(int socket, std::string who) {
    Client& client = getClient(socket);
    std::stringstream ss(who);
    std::string mask;
    ss >> mask;
    if (mask.empty()) {
        sendMessageCommand(socket, ":ircserver 431 " + client.getNick() + " :No nickname given");
        return;
    }
    try {
        Channel& channel = getChannel(mask);
        const std::vector<int>& users = channel.getUsers();
        for (size_t i = 0; i < users.size(); i++) {
            Client& user = getClient(users[i]);
            std::string mod = "H"; // set the mod to H for all users
            if (channel.isOperator(user.getFd()))
                mod += "@"; // add @ for operators
            std::stringstream msg; // create a stringstream to store the message
            msg << ":ircserver 352 " << client.getNick() << " " << channel.getName() << " " << user.getUserName() << " " << user.getHostname() << " " << "*" << " " << user.getNick() << " " << mod << " " << ":0 " << user.getRealName();
            sendMessageCommand(socket, msg.str());   
        }
        sendMessageCommand(socket, ":ircserver 315 " + client.getNick() + " " + mask + " :End of /WHO list");
    }
    catch (std::runtime_error& e) {
        sendMessageCommand(socket, ":ircserver 403 " + client.getNick() + " " + mask + " :No such channel"); // send an error message if the channel does not exist
    }
}


/* ----------------------------  WHOIS command ------------------------------
                Returns detailed information about the user,
                such as their hostname and channels they are in.
   -------------------------------------------------------------------------- */
//      gets information about a specific user.
void    Server::WHOIS(int socket, std::string whois) {
    Client& client = getClient(socket);
    std::stringstream ss(whois);
    std::string mask;
    ss >> mask;
    if (mask.empty()) {
        sendMessageCommand(socket, ":ircserver 431 " + client.getNick() + " :No nickname given");
        return;
    }
    try {
        Client &target = getClientByNick(mask);
        std::stringstream msg;
        msg << ":ircserver 311 " << client.getNick() << " " << target.getNick() << " " << target.getUserName() << " " << target.getHostname() << " * :" << target.getRealName();
        sendMessageCommand(socket, msg.str());
    }
    catch (std::runtime_error& e) {
        sendMessageCommand(socket, ":ircserver 401 " + client.getNick() + " " + mask + " :No such nick/channel"); // send an error message if the channel does not exist
    }
}