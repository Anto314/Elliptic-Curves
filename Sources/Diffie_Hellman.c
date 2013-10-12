/** @file Diffie_Hellman.c
 * An elliptic curves implementation of the key exchange algorithm.
 */
#include <stdio.h>
#include <gmp.h>
#include <string.h>
#include "Elliptic_Curves.h"
#include "Network.h"

/*void DiffieHellman(char Is_Alice, TEllipticCurve *Pointer_Curve,
				   
				   printf("1. Choosing a random number 'r' :\n");
	// TODO
	
	printf("2. Sending g^r : \n");
	// TODO
	
	printf("3. Receiving */

int main(int argc, char *argv[])
{
	char Is_Alice, *String_Parameter_Character, *String_Parameter_File_Name, *String_Parameter_IP_Address;
	TEllipticCurve Curve;
	int Socket_Alice, Socket_Bob;
	
	// Check parameters
	if (argc != 4)
	{
		printf("Error : bad parameters.\n" \
			"Usages :\n" \
			"%s -alice ServerIPAddressToBind EllipticCurveFile.gp\n" \
			"%s -bob IPAddressToConnectTo EllipticCurveFile.gp\n" \
			"Remember that Alice must be launched first (she will provide the server Bob can connect to).\n", argv[0], argv[0]);
		return -1;
	}
	String_Parameter_Character = argv[1];
	String_Parameter_IP_Address = argv[2];
	String_Parameter_File_Name = argv[3];
	
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
	
	// Start server if this is Alice
	if (Is_Alice)
	{
		// Wait for Bob
		Socket_Alice = NetworkServerCreate(String_Parameter_IP_Address, 1234);
		if (Socket_Alice < 0)
		{
			printf("Error : could not create the server.\n");
			ECFree(&Curve);
			return -4;
		}
		printf("Waiting for Bob... ");
		fflush(stdout);
		
		// Accept him
		Socket_Bob = NetworkServerListen(Socket_Alice);
		if (Socket_Bob < 0)
		{
			printf("Error : server could not accept Bob.\n");
			ECFree(&Curve);
			return -5;
		}
		printf("Bob connected.\n");
		
		// Exchange keys
	}
	// Connect to server if this is Bob
	/*else
	{
		NetworkClientConnect(1234);*/
	
	return 0;
}