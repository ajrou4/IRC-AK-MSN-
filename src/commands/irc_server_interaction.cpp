/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_server_interaction.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haguezou <haguezou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 17:03:42 by haguezou          #+#    #+#             */
/*   Updated: 2024/06/03 17:04:44 by haguezou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserver.hpp"
#include "client.hpp"

/* ---------------------------- PING command ------------------------------
    The server sends a PING message to check if the client is responsive.
    The client must respond with a PONG message.
   -----------------------------------------------------------------------*/
//      checks the connection between the client and the server.
void    Server::PING(int socket, std::string ping) {
    std::string server = ping.substr(ping.find(' ') + 1);
    sendMessageCommand(socket, ":ircserver PONG " + server);
}