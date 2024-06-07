/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_user_info.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 18:46:00 by haguezou          #+#    #+#             */
/*   Updated: 2024/06/07 02:54:45 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "ircserver.hpp"

/* ----------------------------  WHO command ------------------------------
                Provides information about users.
                If no parameters are given, it lists all visible users.
   ------------------------------------------------------------------------- */

void    Server::WHO(int socket, std::string who) {
    Client& client = getClient(socket);
    std::stringstream ss(who);
    std::string mask;
    ss >> mask;
    if (mask.empty()) {
        sendMessageCommand(socket, intro() + "431 WHO : No target given");
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
            msg << intro() << "352 " << client.getNick() << " " << channel.getName() << " " << user.getUserName() << " " << user.getHostname() << " " << "*" << " " << user.getNick() << " " << mod << " " << ":0 " << user.getRealName();
            sendMessageCommand(socket, msg.str());   
        }
        sendMessageCommand(socket, intro() + "315 " + client.getNick() + " " + channel.getName() + " : End of /WHO list");
    }
    catch (std::runtime_error& e) {
        sendMessageCommand(socket, intro() + "403 " + mask + " :No such channel"); // send an error message if the channel does not exist
    }
}

/* ----------------------------  WHOIS command ------------------------------
                Returns detailed information about the user,
                such as their hostname and channels they are in.
   -------------------------------------------------------------------------- */

void    Server::WHOIS(int socket, std::string whois) {
    Client& client = getClient(socket);
    std::stringstream ss(whois);
    std::string mask;
    ss >> mask;
    if (mask.empty()) {
        sendMessageCommand(socket, intro() + "431 WHOIS  : No target given");
        return;
    }
    try {
        Client &target = getClientByNick(mask);
        std::stringstream msg;
        msg << intro() + "311 " << client.getNick() << " " << mask  << " " << client.getUserName() << " " << client.getHostname() << " * : " << target.getRealName();
        sendMessageCommand(socket, msg.str());
    }
    catch (std::runtime_error& e) {
        sendMessageCommand(socket, intro() + "401 " + mask + " : No such target"); // send an error message if the channel does not exist
    }
}
