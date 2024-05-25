/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircImplementation.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/25 17:36:11 by omakran           #+#    #+#             */
/*   Updated: 2024/05/26 00:55:22 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserver.hpp"
#include "client.hpp"

/* ------------ this file will implement the logic of ARC --------------
|                        let's parss each command                       |
 --------------------------------------------------------------------- */ 

void    Server::PASS(int socket, std::string pass) {
    Client &client = getClient(socket);
    
    if (client.isAuthenticated()) {
        sendMessageToClient(socket, ":ircserver 462 You may not register");
    } else if (pass != password){
        sendMessageToClient(socket, ":ircserver 464 Invalid password");
    } else {
        client.setAuthenticated(true);
    }
}

