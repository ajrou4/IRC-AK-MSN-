/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 18:39:08 by omakran           #+#    #+#             */
/*   Updated: 2024/05/19 16:34:03 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserver.hpp"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <port> <password" << std::endl;
        return 1;
    }

    int         port = atoi(argv[1]); // convert port argument to integer
    std::string password = argv[2]; // get the password

    Server      ircServer(port, password); // create a server object
    //        start the server
    ircServer.start();

    return 0;
}