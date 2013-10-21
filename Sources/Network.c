/** @file Network.c
 * Basic server / client functions.
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <gmp.h>
#include "Point.h"
#include "Network.h"

int NetworkServerCreate(char *String_IP_Address, unsigned short Port)
{
	int Socket, Is_Enabled = 1;
	struct sockaddr_in Address;
	
	// Create the socket
	Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Socket == -1) return -1;
	
	// Allow address to be reused instantly
	setsockopt(Socket, SOL_SOCKET, SO_REUSEADDR, &Is_Enabled, sizeof(Is_Enabled));
	
	// Bind the specified port
	Address.sin_family = AF_INET;
	Address.sin_port = htons(Port);
	Address.sin_addr.s_addr = inet_addr(String_IP_Address);
	if (bind(Socket, (const struct sockaddr *) &Address, sizeof(Address)) == -1)
	{
		close(Socket);
		return -2;
	}
	
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

int NetworkClientConnect(char *String_IP_Address, unsigned short Port)
{
	int Socket;
	struct sockaddr_in Address;
	
	// Create the socket
	Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (Socket == -1) return -1;
	
	// Fill server address
	Address.sin_family = AF_INET;
	Address.sin_port = htons(Port);
	Address.sin_addr.s_addr = inet_addr(String_IP_Address);
	
	// Connect to server
	if (connect(Socket, (const struct sockaddr *) &Address, sizeof(Address)) == -1)
	{
		close(Socket);
		return -2;
	}
	
	return Socket;
}

void NetworkSendMPZ(int Socket_Destination, mpz_t Number)
{
	char String[NETWORK_MAXIMUM_STRINGIFIED_NUMBER_SIZE];
	int Length;
	
	// Send a string to avoid architecture specific binary encoding issues
	Length = gmp_snprintf(String, NETWORK_MAXIMUM_STRINGIFIED_NUMBER_SIZE, "%Zd", Number) + 1; // +1 for terminating zero
	// Send size
	write(Socket_Destination, &Length, sizeof(Length));
	write(Socket_Destination, String, Length);
}

void NetworkReceiveMPZ(int Socket_Source, mpz_t Number)
{
	char String[NETWORK_MAXIMUM_STRINGIFIED_NUMBER_SIZE];
	int Length;
	
	// Retrieve string length
	read(Socket_Source, &Length, sizeof(Length));
	
	// Receive string
	read(Socket_Source, String, Length);
	mpz_set_str(Number, String, 10);
}

void NetworkSendPoint(int Socket_Destination, TPoint *Pointer_Point)
{
	char Is_Infinite;
	
	// Send infinity flag first to avoid sending coordinates if the point is infinite
	Is_Infinite = Pointer_Point->Is_Infinite; // Force cast to a single byte to avoid compiler optimizations issues
	write(Socket_Destination, &Is_Infinite, sizeof(Is_Infinite));
	if (Is_Infinite) return;
	
	// Send coordinates
	NetworkSendMPZ(Socket_Destination, Pointer_Point->X);
	NetworkSendMPZ(Socket_Destination, Pointer_Point->Y);
}

void NetworkReceivePoint(int Socket_Source, TPoint *Pointer_Point)
{
	char Is_Infinite;
	
	// Receive infinity flag
	read(Socket_Source, &Is_Infinite, sizeof(Is_Infinite));
	Pointer_Point->Is_Infinite = Is_Infinite;
	if (Is_Infinite) return;
	
	// Receive coordinates
	NetworkReceiveMPZ(Socket_Source, Pointer_Point->X);
	NetworkReceiveMPZ(Socket_Source, Pointer_Point->Y);
}