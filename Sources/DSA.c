/** @file DSA.c
 * DSA signature algorithm.
 */
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "Elliptic_Curves.h"
#include "Network.h"
#include "Utils.h"

static void DSAAlice(TEllipticCurve *Pointer_Curve, unsigned char *Pointer_Message, size_t Message_Length, int Socket_Bob, mpz_t Private_Key_Alice)
{
	unsigned char Buffer_Hash[UTILS_HASH_LENGTH];
	mpz_t Number_Temp, Number_R, Number_S, Number_Hash;
	TPoint Point;
	
	// Initialize variables
	mpz_init(Number_Temp);
	mpz_init(Number_R);
	mpz_init(Number_S);
	mpz_init(Number_Hash);
	PointCreate(0, 0, &Point);
	
	// Compute message hash
	printf("Alice is computing message hash...\n");
	UtilsComputeHash(Pointer_Message, Message_Length, Buffer_Hash);
	mpz_set_str(Number_Hash, Buffer_Hash, 10);
	UtilsShowHash(Buffer_Hash);
	putchar('\n');
	
	while (1)
	{
		// Get a random K
		UtilsGenerateRandomNumber(Pointer_Curve->p, Number_R);
		
		// Compute a curve point
		ECMultiplication(Pointer_Curve, &Pointer_Curve->Point_Generator, Number_R, &Point);
	
		// Calculate R
		mpz_mod(Number_R, Point.X, &Pointer_Curve->p);
		// Retry if the computed R is 0
		if (mpz_cmp_ui(Number_R, 0) == 0) continue;
		
		// Calculate S
		mpz_mul(Number_Temp, Number_R, Private_Key_Alice); // r * dA
		mpz_add(Number_Hash, Number_Temp, Number_Hash); // z + (r * dA), recycle Hash variable
		mpz_invert(
		
		// Retry if the computed S is 0
		if (mpz_cmp_ui(Number_S, 0) != 0) break;
	}
	
	
	
	// Free resources
	mpz_clear(Number_Temp);
	mpz_clear(Number_R);
	mpz_clear(Number_S);
	mpz_clear(Number_Hash);
}

static void DSABob(TEllipticCurve *Pointer_Curve, int Socket_Alice)
{
	TPoint Point_Public_Key_Alice;
	
	// Initialize variables
	PointCreate(0, 0, &Point_Public_Key_Alice);
	
	// Receiving Alice's public key
	printf("Waiting for Alice's public key...\n");
	NetworkReceivePoint(Socket_Alice, &Point_Public_Key_Alice);
	PointShow(&Point_Public_Key_Alice);
	putchar('\n');
}

int main(int argc, char *argv[])
{
	char Is_Alice, *String_Parameter_Character, *String_Parameter_File_Name, *String_Parameter_IP_Address;
	unsigned short Port;
	TEllipticCurve Curve;
	int Socket_Alice, Socket_Bob;
	unsigned char Message[] = "Ciao, codesto messaggio sa di merda";
	mpz_t Private_Key_Alice;
	TPoint Point_Public_Key_Alice;
	
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
		
		// Generate Alice's private key once
		printf("Computing Alice's private key...\n");
		mpz_init(Private_Key_Alice);
		UtilsGenerateRandomNumber(Curve.p, Private_Key_Alice);
		gmp_printf("%Zd\n\n", Private_Key_Alice);
		
		// Generate public key = Private_Key * Curve_Generator
		printf("Computing Alice's public key...\n");
		PointCreate(0, 0, &Point_Public_Key_Alice);
		ECMultiplication(&Curve, &Curve.Point_Generator, Private_Key_Alice, &Point_Public_Key_Alice);
		PointShow(&Point_Public_Key_Alice);
		putchar('\n');
		
		// Send public key to Bob
		printf("Sending public key to Bob... ");
		fflush(stdout);
		NetworkSendPoint(Socket_Bob, &Point_Public_Key_Alice);
		printf("done.\n");
		s		
		DSAAlice(&Curve, Message, sizeof(Message) - 1, Socket_Bob, Private_Key_Alice);
		
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
		
		DSABob(&Curve, Socket_Alice);
	}
	
	close(Socket_Alice);
	ECFree(&Curve);
	return 0;
}