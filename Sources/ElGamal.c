/** @file ElGamal.c
 * ElGamal cryptosystem.
 */
#include <gmp.h>
#include <stdio.h>
#include "Elliptic_Curves.h"
#include "Point.h"
#include "Network.h"
#include "Utils.h"

/** Generate a random number.
 * @param Pointer_Curve The curve used to provide modulo.
 * @param Output_Key On output, hold the generated private key.
 */
static void GeneratePrivateKey(TEllipticCurve *Pointer_Curve, mpz_t Output_Key)
{
	gmp_randstate_t Random_State;
	
	// Initialize random number generator
	gmp_randinit_default(Random_State);
	gmp_randseed_ui(Random_State, UtilsGenerateRandomSeed());
	
	// Generate secret key
	mpz_init(Output_Key);
	mpz_urandomm(Output_Key, Random_State, Pointer_Curve->p); // Choose a number modulus the order of the curve
}

static void ElGamalAlice(TEllipticCurve *Pointer_Curve, mpz_t Private_Key, int Socket_Bob)
{
	TPoint Point_Public_Key;
	
	// Wait for Bob public key
	
	// Compute public key Q
	PointCreate(0, 0, &Point_Public_Key);
	ECMultiplication(Pointer_Curve, Pointer_Curve->Point_Generator, Private_Key, &Point_Public_Key);
	
	// Send Alice's public key to Bob
	NetworkSendPoint(Socket_Bob, &Point_Public_Key);
	
	PointFree(&Point_Public_Key);
}

static void ElGamalBob(TEllipticCurve *Pointer_Curve, mpz_t Private_Key, int Socket_Alice)
{
	TPoint Point_C1;
	
	// Compute C1
	PointCreate(0, 0, &Point_C1);
	
	
	// Compute public key Q
	PointCreate(0, 0, &Point_Public_Key);
	ECMultiplication(Pointer_Curve, Pointer_Curve->Point_Generator, Private_Key, &Point_Public_Key);
	
	// Send Alice's public key to Bob
	NetworkSendPoint(Socket_Bob, &Point_Public_Key);
	
}

int main(int argc, char *argv[])
{
	char Is_Alice, *String_Parameter_Character, *String_Parameter_File_Name, *String_Parameter_IP_Address;
	unsigned short Port;
	TEllipticCurve Curve;
	int Socket_Alice, Socket_Bob;
	mpz_t Private_Key_Alice, Private_Key_Bob;
	
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
		// Generate private once
		GeneratePrivateKey(&Curve, Private_Key_Alice);
		
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
		ElGamalAlice(&Curve, Private_Key_Alice, Socket_Bob);
		
		close(Socket_Bob);
	}
	// Bob
	else
	{
		// Generate private once
		GeneratePrivateKey(&Curve, Private_Key_Bob);
		
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
		ElGamalmanBob(&Curve, Private_Key_Bob, Socket_Alice);
	}
	
	close(Socket_Alice);
	ECFree(&Curve);
	return 0;
}