						IRC_MSN
Requirements:

    Multi-client handling: The server must manage connections from multiple clients without freezing.
    Non-blocking I/O: All input/output operations should be non-blocking to avoid resource bottlenecks.
    Single polling mechanism: Use poll() (or equivalent like select(), kqueue(), or epoll()) to handle all I/O operations.
    Client compatibility: Choose a reference IRC client that can connect and interact with your server.
    Supported features:
        User authentication (nickname, username)
        Joining and leaving channels
        Sending and receiving private messages
        Broadcasting messages to channel members
        User roles (operators and regular users)
        Channel operator commands:
            KICK: Eject a user
            INVITE: Invite a user
            TOPIC: Change or view channel topic
            MODE: Modify channel settings (invite-only, topic restrictions, channel key, operator privileges, user limit)
    Code quality: Maintain clean and well-structured code.

Additional Notes:

    Forking is not allowed.
    MacOS considerations: Use fcntl() only for setting non-blocking mode with F_SETFL and O_NONBLOCK.
    Thorough testing: Verify error handling for partial data, low bandwidth, and network issues.
    Test example: Use nc to send fragmented commands and simulate network delays.

This explanation provides a foundation for understanding your project requirements. Feel free to ask further questions about specific aspects of the implementation!
