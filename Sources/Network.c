/** @file Network.c
 * Basic server / client functions.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include "Network.h"

int NetworkServerCreate(char *String_IP_Address, unsigned short Port)
{
	int Socket;
	struct sockaddr_in Address;
	
	// Create the socket
	Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Socket == -1) return -1;
	
	// Bind the specified port
	Address.sin_family = AF_INET;
	Address.sin_port = htons(Port);
	Address.sin_addr.s_addr = inet_addr(String_IP_Address);
	if (bind(Socket, (const struct sockaddr *) &Address, sizeof(Address)) == -1) return -2;
	
	return Socket;
}

int NetworkServerListen(int Socket_Server)
{
	int Socket_Client;
	
	// Listen for one client
	if (listen(Socket_Server, 1) == -1) return -1;
	
	// Accept him
	Socket_Client = accept(Socket_Server, NULL, NULL);
	if (Socket_Client == -1) return -2;
	
	return Socket_Client;
}