/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_miscellaneous_cmd.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haguezou <haguezou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 18:53:09 by haguezou          #+#    #+#             */
/*   Updated: 2024/06/03 18:56:13 by haguezou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "client.hpp"
#include "ircserver.hpp"

/* ----------------------------  ISON command --------------------------------
                Returns a list of online users matching the given nicknames.
   --------------------------------------------------------------------------- */
//      checks if a user is online.
void    Server::ISON(int socket, std::string ison) {
    Client& client = getClient(socket);
    std::string nickname;
    std::vector<std::string> nicknames;
    std::stringstream   message;
    std::stringstream ss(ison);

    ss >> nickname;
    if (nickname.empty()) {
        sendMessageCommand(socket, ":ircserver 461 " + client.getNick() + " ISON :Not enough parameters");
        return;
    }
    message << ":ircserver 303 " << client.getNick() << " :";
    while (!ss.eof()) {
        ss >> nickname;
        try {
            getClientByNick(nickname);
            message << nickname << " ";
        }
        catch (std::runtime_error& e) {
            continue; // ignore nicknames that do not exist
        }
    }
    sendMessageCommand(socket, message.str());
}



/* ----------------------------  MODE command --------------------------------
            Can be used to set or check user modes (like operator status) 
            or channel modes (like moderation).
   --------------------------------------------------------------------------- */
//      sets or checks modes for a user or channel.
void    Server::MODE(int socket, std::string mode){
    Client& client = getClient(socket);
    Channel *channel;

    std::string target, modeStr, modeParams;
    std::stringstream ss(mode); // create a stringstream from the message
    ss >> target >> modeStr >> modeParams; // get the target, mode, and mode parameters
    if (target.empty()) {
        sendMessageCommand(socket, ":ircserver 461 " + client.getNick() + " MODE :Not enough parameters");
        return;
    }
    try {
        channel = &getChannel(target); // try to get the channel
    }
    catch (std::runtime_error& e) {
        sendMessageCommand(socket, ":ircserver 403 " + client.getNick() + " " + target + " :No such channel"); // send an error message if the channel does not exist
        return;
    }
    if (mode.empty()) {
        sendMessageCommand(socket, ":ircserver 324 " + client.getNick() + " " + target + " " + channel->getModes());
        return;
    }
    if (!channel->isOperator(socket) || !channel->hasClient(socket)) { // if the client is not an operator or not in the channel
        sendMessageCommand(socket, ":ircserver 482 " + client.getNick() + " :You're not a channel operator");
        return;
    }
    bool addMode = true;
    if (modeStr[0] == '+') {
        addMode = true; // set addMode to true if the mode is a plus
        modeStr = modeStr.substr(1); // remove the plus
    }
    if (modeStr.length() != 1) { // if the mode is not a single character
        sendMessageCommand(socket, ":ircserver 472 " + client.getNick() + " " + modeStr + " :is unknown mode char to me");
        return;
    }
    switch (modeStr[0]) {
        case 'o': // means give/take channel operator privileges
            if (modeParams.empty()) {
                sendMessageCommand(socket, ":ircserver 461 " + client.getNick() + " MODE :Not enough parameters");
                return;
            }
            try {
                Client& targetClient = getClientByNick(modeParams);
                if (addMode) { // if the mode is a plus
                    channel->addOperator(targetClient.getFd()); // add the client as an operator
                } else {
                    channel->removeOperator(targetClient.getFd()); // remove the client as an operator
                }
            }
            catch (std::runtime_error& e) {
                sendMessageCommand(socket, ":ircserver 401 " + client.getNick() + " " + modeParams + " :No such nick/channel");
            }
            break;
        case 'v': // means give/take the voice privilege
            if (modeParams.empty()) {
                sendMessageCommand(socket, ":ircserver 461 " + client.getNick() + " MODE :Not enough parameters");
                return;
            }
            try {
                Client& targetClient = getClientByNick(modeParams);
                if (addMode) { // if the mode is a plus
                    channel->addPlusV(targetClient.getFd()); // add the client as a voiced user
                } else {
                    channel->removePlusV(targetClient.getFd()); // remove the client as a voiced user
                }
            }
            catch (std::runtime_error& e) {
                sendMessageCommand(socket, ":ircserver 401 " + client.getNick() + " " + modeParams + " :No such nick/channel");
            }
            break;
        case 'k': // means set the channel key
            if (modeParams.empty()) {
                sendMessageCommand(socket, ":ircserver 461 " + client.getNick() + " MODE :Not enough parameters");
                return;
            }
            channel->setMode(Key, addMode);
            if (addMode) {
                channel->setPassword(modeParams);
            } else {
                channel->setPassword("");
            }
            break;
        case 'l': // means set the user limit
            if (modeParams.empty() && addMode) {
                sendMessageCommand(socket, ":ircserver 461 " + client.getNick() + " MODE :Not enough parameters");
                return;
            }
            if (addMode) {
                channel->setUserLimit(std::stoi(modeParams)); // stoi converts the string to an integer
            } else {
                channel->setUserLimit(0); // set the user limit to 0
            }
            break;
        case 'i': // means set the invite-only mode
            channel->setMode(invit_ONLY, addMode);
            break;
        case 'm': // means set the moderated mode
            channel->setMode(Moderated, addMode);
            break;
        case 's': // means set the secret mode
            channel->setMode(Secret, addMode);
            break;
        case 't': // means set the topic protection mode
            channel->setMode(ToPic, addMode);
            break;
        default:
            sendMessageCommand(socket, ":ircserver 472 " + client.getNick() + " " + modeStr + " :is unknown mode char to me");
            break;
    }
    channel->broadcastMessage(":" + client.getNick() + "!" + client.getUserName() + "@" + client.getHostname() + " MODE " + channel->getName() + " "  );
}

