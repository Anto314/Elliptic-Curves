/** @file Network.h
 * Basic server / client functions.
 */
#ifndef H_NETWORK_H
#define H_NETWORK_H

/** Create an IPv4 TCP server.
 * @param String_IP_Address The server address.
 * @param Port The server port.
 * @return a non-negative value if the server was successfully created (it is the file descriptor of the server socket),
 * @return -1 if the server socket could not be created,
 * @return -2 if the address or the port could not be bound.
 */
int NetworkServerCreate(char *String_IP_Address, unsigned short Port);

/** Listen for a client to connect to a previously created server.
 * @param Socket_Server The server socket.
 * @return a non-negative value if a client has been accepted (this is the client's socket),
 * @return -1 if the server failed to listen for a client,
 * @return -2 if the server failed to accept a client.
 */
int NetworkServerListen(int Socket_Server);

#endif