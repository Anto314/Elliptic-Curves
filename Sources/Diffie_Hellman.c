/** @file Diffie_Hellman.c
 * An elliptic curves implementation of the key exchange algorithm.
 */
#include <stdio.h>
#include <gmp.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "Elliptic_Curves.h"
#include "Network.h"
#include "Point.h"

static void DiffieHellmanAlice(TEllipticCurve *Pointer_Curve, int Socket_Bob)
{
	mpz_t Private_Key;
	static gmp_randstate_t Random_State;
	TPoint Point_Temp, Point_Received;
	
	gmp_randinit_default(Random_State);
	gmp_randseed_ui(Random_State, (unsigned long int) time(NULL));
	
	// Choose a random number
	printf("1. Choosing a random private key 'a' :\n");
	mpz_init(Private_Key);
	mpz_urandomm(Private_Key, Random_State, Pointer_Curve->p); // Choose a number modulus the order of the curve
	//mpz_set_ui(Private_Key, 2);
	gmp_printf("a = %Zd\n\n", Private_Key);
	
	// Receive Bob's part of the key (so Bob can send it when he wants)
	printf("2. Receiving b.G from Bob\n");
	PointCreate(0, 0, &Point_Received);
	NetworkReceivePoint(Socket_Bob, &Point_Received);
	gmp_printf("b.G = (%Zd, %Zd)\n\n", Point_Received.X, Point_Received.Y);
	
	// Compute a.G
	printf("3. Sending a.G to Bob: \n");
	PointCreate(0, 0, &Point_Temp);
	ECMultiplication(Pointer_Curve, &Pointer_Curve->Point_Generator, Private_Key, &Point_Temp);
	NetworkSendPoint(Socket_Bob, &Point_Temp);
	gmp_printf("a.G = (%Zd, %Zd)\n\n", Point_Temp.X, Point_Temp.Y);
	
	// Add 'a' to b.G
	ECMultiplication(Pointer_Curve, &Point_Received, Private_Key, &Point_Temp);
	gmp_printf("Key a.b.G : (%Zd, %Zd)\n",  Point_Temp.X, Point_Temp.Y);
	
	PointFree(&Point_Temp);
	PointFree(&Point_Received);
}

static void DiffieHellmanBob(TEllipticCurve *Pointer_Curve, int Socket_Alice)
{
	mpz_t Private_Key;
	static gmp_randstate_t Random_State;
	TPoint Point_Temp, Point_Received;
	
	gmp_randinit_default(Random_State);
	gmp_randseed_ui(Random_State, (unsigned long int) time(NULL));
	
	// Choose a random number
	printf("1. Choosing a random private key 'b' :\n");
	mpz_init(Private_Key);
	mpz_urandomm(Private_Key, Random_State, Pointer_Curve->p); // Choose a number modulus the order of the curve
	//mpz_set_ui(Private_Key, 4);
	gmp_printf("b = %Zd\n\n", Private_Key);
	
	// Compute b.G
	printf("2. Sending b.G to Alice: \n");
	PointCreate(0, 0, &Point_Temp);
	ECMultiplication(Pointer_Curve, &Pointer_Curve->Point_Generator, Private_Key, &Point_Temp);
	NetworkSendPoint(Socket_Alice, &Point_Temp);
	gmp_printf("b.G = (%Zd, %Zd)\n\n", Point_Temp.X, Point_Temp.Y);
	
	// Receive Alice's part of the key
	printf("3. Receiving a.G from Alice\n");
	PointCreate(0, 0, &Point_Received);
	NetworkReceivePoint(Socket_Alice, &Point_Received);
	gmp_printf("a.G = (%Zd, %Zd)\n\n", Point_Received.X, Point_Received.Y);
	
	// Add 'a' to b.G
	ECMultiplication(Pointer_Curve, &Point_Received, Private_Key, &Point_Temp);
	gmp_printf("Key b.a.G : (%Zd, %Zd)\n",  Point_Temp.X, Point_Temp.Y);
	
	PointFree(&Point_Temp);
	PointFree(&Point_Received);
}

int main(int argc, char *argv[])
{
	char Is_Alice, *String_Parameter_Character, *String_Parameter_File_Name, *String_Parameter_IP_Address;
	unsigned short Port;
	TEllipticCurve Curve;
	int Socket_Alice, Socket_Bob;
	
	// Check parameters
	if (argc != 5)
	{
		printf("Error : bad parameters.\n" \
			"Usages :\n" \
			"%s -alice ServerIPAddressToBind ServerPort EllipticCurveFile.gp\n" \
			"%s -bob IPAddressToConnectTo PortToConnectTo EllipticCurveFile.gp\n" \
			"Remember that Alice must be launched first (she will provide the server Bob can connect to).\n", argv[0], argv[0]);
		return -1;
	}
	String_Parameter_Character = argv[1];
	String_Parameter_IP_Address = argv[2];
	Port = atoi(argv[3]);
	String_Parameter_File_Name = argv[4];
	
	// Set server or client mode according to choosen character
	if (strcmp(String_Parameter_Character, "-alice") == 0) Is_Alice = 1;
	else if (strcmp(String_Parameter_Character, "-bob") == 0) Is_Alice = 0;
	else
	{
		printf("Error : unknown character. You must select Alice or Bob.\n");
		return -2;
	}
	
	// Load elliptic curve file
	if (!ECLoadFromFile(String_Parameter_File_Name, &Curve))
	{
		printf("Error : can't load curve file.\n");
		return -3;
	}
	
	// Alice
	if (Is_Alice)
	{
		// Start server
		Socket_Alice = NetworkServerCreate(String_Parameter_IP_Address, Port);
		if (Socket_Alice < 0)
		{
			printf("Error : could not create the server.\n");
			ECFree(&Curve);
			return -4;
		}
		
		// Wait for Bob
		printf("Waiting for Bob... ");
		fflush(stdout);
		Socket_Bob = NetworkServerListen(Socket_Alice);
		if (Socket_Bob < 0)
		{
			printf("Error : server could not accept Bob.\n");
			close(Socket_Alice);
			ECFree(&Curve);
			return -5;
		}
		printf("Bob is connected.\n");
		
		// Exchange keys
		DiffieHellmanAlice(&Curve, Socket_Bob);
		
		close(Socket_Bob);
	}
	// Bob
	else
	{
		// Connect to server
		Socket_Alice = NetworkClientConnect(String_Parameter_IP_Address, Port);
		if (Socket_Alice < 0)
		{
			printf("Error : could not connect to server.\n");
			ECFree(&Curve);
			return 0;
		}
		printf("Connected to Alice.\n");
		
		// Exchange keys
		DiffieHellmanBob(&Curve, Socket_Alice);
	}
	
	close(Socket_Alice);
	ECFree(&Curve);
	return 0;
}