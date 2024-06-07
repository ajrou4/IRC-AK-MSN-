/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_cnx_auth.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 16:35:49 by haguezou          #+#    #+#             */
/*   Updated: 2024/06/06 23:59:45 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"   
#include "ircserver.hpp"


/* ---------------- PASS command ---------------
    Used before registering a connection. 
    If the server requires a password,
    It must be sent before the NICK/USER commands.
   ----------------------------------------------- */
//      sets a password for the connection.
void    Server::PASS(int socket, std::string pass) {
    Client& client = getClient(socket);
    // Check if the client is already authenticated
    if (client.isAuthenticated()) {
        sendMessageCommand(socket, intro() +  "462 :You may not reregister");
        return;
    } else if (pass != password) {
        sendMessageCommand(socket, intro() +  "464 :Password incorrect");
        return;
    } else {
        client.setAuthenticated(true);
    }
}


/* ---------------- NICK command -----------------
    Essential for user registration; 
    Identifies a user with a unique nickname.
   ----------------------------------------------- */
//      sets or changes the nickname of a user.
void    Server::NICK(int socket, std::string nickname) {
    Client& client = getClient(socket);
    // Check if the nickname is valid
    if (nickname.size() < 1 || nickname.size() > 9 || nickname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\\[]{}|`_-^") != std::string::npos
        || nickname.find_first_of("0123456789-", 0, 1) == 0) {
            sendMessageCommand(socket, intro() + "432 " + nickname + " : Erroneous nickname");
            return;
    }
    try {
        getClientByNick(nickname);
        sendMessageCommand(socket, intro() + "433 " + nickname + " : Nickname is already in use");
    } catch (std::runtime_error& e) {
        std::stringstream broadcastMessage;
        broadcastMessage << client.intro() + "NICK " << nickname; // broadcast the new nickname
        if (client.getUserName() != "" && !client.isRegistered()) // if the client is not registered, register them
            registerNewClient(socket);
        client.setNick(nickname);
        sendMessageToClientChannels(socket, broadcastMessage.str()); // send the message to all the channels the client is in
    }
}

/* -------------------- USER command -------------------------
    Used to specify the username, hostname, servername,
    and real name of the user.
    This command, along with NICK, completes the registration.
   ------------------------------------------------------------ */
//      sends user information to the server.
void    Server::USER(int socket, std::string params) {
    Client& client = getClient(socket);
    std::string username, realname, skipChar;
    std::stringstream ss(params);
    ss >> username >> std::ws; // get the username and get rid of the leading whitespace
    std::getline(ss, skipChar, ':'); // get rid of the colon
    ss >> std::ws; // get rid of the leading whitespace
    std::getline(ss, realname, '\0');
    //                                          https://modern.ircdocs.horse/#userlen-parameter
    if (username.size() < 1 || username.size() > 12
        || username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789") != std::string::npos
        || username.find_first_of("0123456789", 0, 1) == 0) {
            sendMessageCommand(socket, intro() + "432 " + username + " : Erroneous username");
            return;
    }
    if (!realname.empty() && (realname.size() > 50 || realname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]{}\\|^`_- ") != std::string::npos)) {
        sendMessageCommand(socket, intro() + "501 " + realname + " : Invalid Realname");
        return;
    }
    std::stringstream broadcastMessage;
    broadcastMessage << client.intro() << "USER " << params; // broadcast the new username
    client.setUserName(username);
    client.setRealName(realname);
    if (client.getNick() != "" && !client.isRegistered()) // if the client is not registered, register them
        registerNewClient(socket);
    sendMessageToClientChannels(socket, broadcastMessage.str()); // send the message to all the channels the client is in
}