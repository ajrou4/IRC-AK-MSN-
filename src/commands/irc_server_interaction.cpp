/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc_server_interaction.cpp                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: haguezou <haguezou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/03 17:03:42 by haguezou          #+#    #+#             */
/*   Updated: 2024/06/08 18:20:04 by haguezou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ircserver.hpp"
#include "Client.hpp"

/* ---------------------------- PING command ------------------------------
    The server sends a PING message to check if the client is responsive.
    The client must respond with a PONG message.
   -----------------------------------------------------------------------*/

void    Server::PING(int socket, std::string ping) {
    sendMessageCommand(socket, intro() + "PONG " + ping);
}
