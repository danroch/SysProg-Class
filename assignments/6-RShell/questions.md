1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

For this shell we use a termination character to indicate the full transmission of the message, which in this assignment was RDSH_EOF_CHAR (0x04).

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocol would need to defined bondaries for each command and response. In our case, we used a special delimiter (RDSH_EOF_CHAR) to tell a receiver to stop reading from the stream. If this is not handled correctly, and we assume a recv() call returns a complete command, we could run into errors when executing the command. We may also get multiple commands read together in one call which leads to incorrect parsing. A server may also wait indefinitely if it never sees an unexpected delimeter. Ultimately there's a lot that can go wrong if this is not handled correctly

3. Describe the general differences between stateful and stateless protocols.

A stateful protocol maintains information about the interaction between a client and server across multiple requests and keeps track of past interactions. For example SSH will keep track of authentication and session state. Stateless protocols do not retain information about previous requests. So each request is treated independently, with the prior requests not affecting the requests which follow.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP has less overhead involved making it a more "efficient" protocol. For example is we are dealing with an application that handles video data, it may not be a big deal or even perceptable to the human eye if a single frame isn't sent again, speed would be the main priority in this case and UDP would be the right choice due to the lower latency and overhead involved.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The OS provides sockets as the primary interface for communication over networks. Sockets allow us to communicate over a network in the same manner as we would read or write to a file (through file descriptors in C for example) while abstracting away much of the details and labor that goes into transmitting data across a network.
