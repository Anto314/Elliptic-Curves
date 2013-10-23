/** @file Diffie_Hellman.c
 * An elliptic curves implementation of the key exchange algorithm.
 */
#include <stdio.h>
#include <gmp.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "Elliptic_Curves.h"
#include "Network.h"
#include "Point.h"
#include "Utils.h"

/** Server part of the Diffie-Hellman key exchanging.
 * @param Pointer_Curve The curve used to make calculations.
 * @param Socket_Bob Client's socket.
 * @param Private_Key On output, hold the private key.
 * @param Pointer_Output_Point On output, hold the shared key.
 */
static void DiffieHellmanAlice(TEllipticCurve *Pointer_Curve, int Socket_Bob, mpz_t Private_Key, TPoint *Pointer_Output_Point)
{
	TPoint Point_Temp;

	// Initialize variables
	PointCreate(0, 0, &Point_Temp);
	
	// Choose a random number
	printf("Choosing a random private key 'a'...\n");
	UtilsGenerateRandomNumber(Pointer_Curve->p, Private_Key); // Choose a number modulus the order of the curve
	gmp_printf("a = %Zd\n\n", Private_Key);
	
	// Receive Bob's part of the key (so Bob can send it when he wants)
	printf("Receiving b.G from Bob...\n");
	NetworkReceivePoint(Socket_Bob, Pointer_Output_Point);
	PointShow(Pointer_Output_Point);
	
	// Compute a.G
	printf("Sending a.G to Bob...\n");
	ECMultiplication(Pointer_Curve, &Pointer_Curve->Point_Generator, Private_Key, &Point_Temp);
	NetworkSendPoint(Socket_Bob, &Point_Temp);
	PointShow(&Point_Temp);
	
	// Multiply 'a' to b.G
	ECMultiplication(Pointer_Curve, Pointer_Output_Point, Private_Key, Pointer_Output_Point);
	PointShow(Pointer_Output_Point);
	
	// Free resources
	PointFree(&Point_Temp);
}

/** Client part of the Diffie-Hellman key exchanging.
 * @param Pointer_Curve The curve used to make calculations.
 * @param Socket_Bob Server's socket.
 * @param Private_Key On output, hold the private key.
 * @param Pointer_Output_Point On output, hold the shared key.
 */
static void DiffieHellmanBob(TEllipticCurve *Pointer_Curve, int Socket_Alice, mpz_t Private_Key, TPoint *Pointer_Output_Point)
{
	TPoint Point_Temp;
	
	// Initialize variables
	PointCreate(0, 0, &Point_Temp);
	
	// Choose a random number
	printf("Choosing a random private key 'b'...\n");
	UtilsGenerateRandomNumber(Pointer_Curve->p, Private_Key); // Choose a number modulus the order of the curve
	gmp_printf("b = %Zd\n\n", Private_Key);
	
	// Compute b.G
	printf("Sending b.G to Alice...\n");
	ECMultiplication(Pointer_Curve, &Pointer_Curve->Point_Generator, Private_Key, &Point_Temp);
	NetworkSendPoint(Socket_Alice, &Point_Temp);
	PointShow(&Point_Temp);
	putchar('\n');
	
	// Receive Alice's part of the key
	printf("Receiving a.G from Alice...\n");
	NetworkReceivePoint(Socket_Alice, Pointer_Output_Point);
	PointShow(Pointer_Output_Point);
	putchar('\n');
	
	// Multiply 'a' to b.G
	ECMultiplication(Pointer_Curve, Pointer_Output_Point, Private_Key, Pointer_Output_Point);
	PointShow(Pointer_Output_Point);
	
	// Free resources
	PointFree(&Point_Temp);
}

int main(int argc, char *argv[])
{
	char Is_Alice, *String_Parameter_Character, *String_Parameter_File_Name, *String_Parameter_IP_Address;
	unsigned short Port;
	TEllipticCurve Curve;
	int Socket_Alice, Socket_Bob;
	mpz_t Private_Key;
	TPoint Point_Shared_Key;
	
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
	
	UtilsInitializeRandomGenerator();
	
	// Initialize variables
	PointCreate(0, 0, &Point_Shared_Key);
	mpz_init(Private_Key);
	
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
		printf("Bob is connected.\n\n");
		
		// Exchange keys
		DiffieHellmanAlice(&Curve, Socket_Bob, Private_Key, &Point_Shared_Key);
		
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
		printf("Connected to Alice.\n\n");
		
		// Exchange keys
		DiffieHellmanBob(&Curve, Socket_Alice, Private_Key, &Point_Shared_Key);
	}
	
	close(Socket_Alice);
	PointFree(&Point_Shared_Key);
	ECFree(&Curve);
	return 0;
}