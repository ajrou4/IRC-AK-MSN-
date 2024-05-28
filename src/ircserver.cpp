/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omakran <omakran@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/18 18:39:05 by omakran           #+#    #+#             */
/*   Updated: 2024/05/28 18:20:04 by omakran          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ircserver.hpp"

Server::Server(int port, const std::string& password) : port(port), password(password) {
    initializeServer();
}

Server::~Server() {
    cleanUp();
}

// initiale the command
void    Server::InithandleComands(void) {
    // commands["PASS"] = &Server::PASS;
    // commands["NICK"] = &Server::NICK;
    // commands["USER"] = &Server::USER;
    // commands["LIST"] = &Server::LIST;
    // commands["JOIN"] = &Server::JOIN;
    // commands["PART"] = &Server::PART;
    // commands["WHO"] = &Server::WHO;
    // commands["WHOIS"] = &Server::WHOIS;
    // commands["PING"] = &Server::PING;
    // commands["PRIVMSG"] = &Server::PRIVMSG;
    // commands["QUIT"] = &Server::QUIT;
    // commands["KICK"] = &Server::KICK;
    // commands["INVITE"] = &Server::INVITE;
    // commands["TOPIC "] = &Server::TOPIC;
    // commands["ISON"] = &Server::ISON;
    // commands["MODE"] = &Server::MODE;
}

void    Server::initializeServer() {
    struct sockaddr_in  server_addr;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation error: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    // set socket options
    int opt = 1; // this value will be used as the option value for the function
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) { // SO_REUS: allow the socket to bind a port that is in a TIME_WAIT,
        std::cerr << "Setsockopt error: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // set the socket to non-blocking mode
    if (fcntl(server_fd, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Fcntl error: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // bind the socket to the address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Bind error: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // listen for incoming connections
    if (listen(server_fd, 10) == -1) {
        std::cerr << "Listen error: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // add the server socket to the polling list
    struct pollfd server_pollfd;
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN;
    fds.push_back(server_pollfd);
}

// main loop of the server:
void    Server::pollLoop() {
    while (true) {
        int poll_count = poll(fds.data(), fds.size(), -1);
        if (poll_count < 0) {
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        handleEvents();
    }
}

void    Server::handleEvents() {
    // check if the event is ready for reading.
    if (fds[0].revents & POLLIN)
        handleNewConnection();
    // iterate over all monitored file descriptors.
    for (size_t i = 1; i < fds.size(); i++)
    {
        if (fds[i].revents & (POLLERR | POLLHUP))
            QUIT(fds[i].fd, "Client disconnected");
        else if (fds[i].revents | POLLIN)
            handleClientMessage(fds[i].fd);
        else if (fds[i].revents | POLLOUT)
            WriteMsgToClient(fds[i].fd);
    }
}

// write message to a client:
void    Server::WriteMsgToClient(int socket)  {
    Client& client = getClient(socket);
    // if thre's no data to send, return
    if (!client.outBoundReady())
        return;
    
}

// accept a new client connection:
void    Server::handleNewConnection() {
    struct sockaddr_in  client_addr; // client address structure.
    socklen_t           client_len = sizeof(client_addr); // lenght of the client address structure.
    //                  accept the new connection.
    int                 client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        std::cerr << "Accept error: " << strerror(errno) << std::endl;
        return;
    }

    // set the new client socket to non-blocking mode.
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Fcntl error: " << strerror(errno) << std::endl;
        close (client_fd);
        return;
    }

    // add the new client to the list of file descriptors to poll
    struct pollfd       client_pollfd;
    client_pollfd.fd = client_fd; // client file descriptor.
    client_pollfd.events = POLLIN | POLLERR | POLLHUP; // interested in reading events
    fds.push_back(client_pollfd); // add to the list of poll file descriptors.
    std::string clientIp = inet_ntoa(client_addr.sin_addr);
    clients[client_fd] = new Client(client_fd, clientIp, clientIp);

    std::cout << "New connection from " << clientIp << std::endl;

}

// handle a message from a client:
void    Server::handleClientMessage(int client_fd) {
    char    buffer[4096]; // buffer to reading data.
    // read data from the client.
    int     bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read == -1) {
        std::cerr << "Recv error: " << strerror(errno) << std::endl;
        return;
    }
    Client& client = getClient(client_fd);
    client.appendToInboundBuffer(std::string(buffer, bytes_read));
    if (client.inboundReady()) {
        std::vector<std::string> commands = client.splitCommands();
        commandsProssed(commands, client_fd);
    }
}

Client& Server::getClient(int fd) {
    std::map<int, Client*>::iterator it = clients.find(fd);
    if (it == clients.end()) {
        throw std::runtime_error("Client not found for fd: " + std::to_string(fd));
    }
    return *it->second;
}

Client& Server::getClient(const std::string& nickname) {
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getUserName() == nickname) {
            return *it->second;
        }
    }
    throw std::runtime_error("Client not found for nickname: " + nickname);
}

void    Server::sendMessageToClient(int client_fd, const std::string& message) {
    Client& client = getClient(client_fd); // retrieve the client object associated with client_fd
    client.newMessage(message); // add the message to the client message queue
    std::cout << ">>>>> Sending into socket " << client_fd << ": " << message << std::endl;

    // mark the client_fd as ready for wrinting
    struct pollfd &client_pollfd = getPollfd(client_fd);
    client_pollfd.events |= POLLOUT;
}

struct pollfd& Server::getPollfd(int fd) {
    for (size_t i = 0; i < fds.size(); ++i) {
        if (fds[i].fd == fd) {
            return fds[i];
        }
    }
    throw std::runtime_error("Pollfd not found for fd: " + std::to_string(fd));
}

void    Server::commandsProssed(std::vector<std::string> cmds, int fd_client) {
    std::vector<std::string>::iterator it = cmds.begin();
    Client& client = getClient(fd_client);
    while (it != cmds.end()) {
        std::cout << "<<<<< Recieved from socket: " << fd_client << ": " << *it << std::endl;
        std::string command_name;
        std::string command_params;
        std::stringstream ss(*it);
        
        ss >> command_name >> std::ws;  // extract the command name
        std::transform(command_name.begin(), command_name.end(), command_name.begin(), ::toupper);
        std::getline(ss, command_params, '\0'); // extract the command parameters
        if (command_name != "PASS" && !client.isAuthenticated()) { // if the client is not authenticated
            sendMessageToClient(fd_client, ":irc 451 :You have not registered");
        } else if (command_name != "PASS" && command_name != "NICK" && command_name != "USER" && !client.isRegistered()) {
            sendMessageToClient(fd_client, ":irc 451 :You have not registered");
        } else if (commands.find(command_name) == commands.end()) {
            sendMessageToClient(fd_client, ":irc 421 " + command_name + " : Unknown command");
        } else if (command_name == "QUIT") {
            QUIT(fd_client, command_params);
        } else {
            (this->*commands[command_name])(fd_client, command_params); // call the command handler
        }
        it++;
    }
}

void    Server::sendMessageToClient(int client_fd, const std::string& message) {
    Client& client = getClient(client_fd); // retrieve the client object associated with client_fd
    client.newMessage(message); // add the message to the client message queue
    std::cout << ">>>>> Sending into socket " << client_fd << ": " << message << std::endl;

    // mark the client_fd as ready for wrinting
    struct pollfd &client_pollfd = getPollfd(client_fd);
    client_pollfd.events |= POLLOUT;
}

struct pollfd& Server::getPollfd(int fd) {
    for (size_t i = 0; i < fds.size(); ++i) {
        if (fds[i].fd == fd) {
            return fds[i];
        }
    }
    throw std::runtime_error("Pollfd not found for fd: " + std::to_string(fd));
}

void    Server::cleanUp() {
    close (server_fd);
    for (size_t i = 0; i < fds.size(); i++)
    {
        close(fds[i].fd);
    }
    fds.clear();
    clients.clear();
}

Client* Server::getClientByNick(const std::string& nick) {
    for (std::map<int, Client*>::iterator it = clients.begin(); it != clients.end(); ++it) {
        if (it->second->getNick() == nick) {
            return (it->second);
        }
    }
    return NULL;
}
