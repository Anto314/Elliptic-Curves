/** @file Network.h
 * Basic server / client functions.
 */
#ifndef H_NETWORK_H
#define H_NETWORK_H

/** Maximum size in characters of a stringified number. */
#define NETWORK_MAXIMUM_STRINGIFIED_NUMBER_SIZE 2048

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

/** Connect to an IPv4 TCP server.
 * @param String_IP_Address The server address.
 * @param Port The server port.
 * @return a non-negative number if the connection was successful (this is the socket value),
 * @return -1 if the client socket could not be created,
 * @return -2 if the connection failed.
 */
int NetworkClientConnect(char *String_IP_Address, unsigned short Port);

/** Send a GMP MPZ number over the network using a string format.
 * @param Socket_Destination Where to send the number.
 * @param Number The number to send.
 */
void NetworkSendMPZ(int Socket_Destination, mpz_t Number);

/** Receive a MPZ number from the network.
 * @param Socket_Source The socket from which to receive the number.
 * @param Number On output, the received MPZ number.
 */
void NetworkReceiveMPZ(int Socket_Source, mpz_t Number);

/** Send a point over the network.
 * @param Socket_Destination Where to send the number.
 * @param Pointer_Point The point to send.
 */
void NetworkSendPoint(int Socket_Destination, TPoint *Pointer_Point);

/** Receive a point from the network.
 * @param Socket_Source The socket from which to receive the number.
 * @param Pointer_Point On output, the received point.
 */
void NetworkReceivePoint(int Socket_Source, TPoint *Pointer_Point);

#endif