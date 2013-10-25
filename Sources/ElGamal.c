/** @file ElGamal.c
 * ElGamal cryptosystem.
 */
#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Elliptic_Curves.h"
#include "Point.h"
#include "Network.h"
#include "Utils.h"

/** Send public key to Bob and decipher his message (server side of the protocol).
 * @param Pointer_Curve The curve used for computations.
 * @param Socket_Bob The way used to communicate with Bob.
 * @param Pointer_Point_Public_Key_Alice Alice's public key.
 * @param Private_Key_Alice Alice's private key.
 * @param Output_Message On output, contain the message sent by Bob.
 */
static void ElGamalAlice(TEllipticCurve *Pointer_Curve, int Socket_Bob, TPoint *Pointer_Point_Public_Key_Alice, mpz_t Private_Key_Alice, mpz_t Output_Message)
{
	TPoint Point_C1, Point_C2;
	
	// Initialize variables
	PointCreate(0, 0, &Point_C1);
	PointCreate(0, 0, &Point_C2);
				
	// Send Alice's public key to Bob
	printf("Alice is sending her public key to Bob... ");
	fflush(stdout);
	NetworkSendPoint(Socket_Bob, Pointer_Point_Public_Key_Alice);
	printf("done.\n\n");
	
	// Receive points from Bob
	// Get C1
	printf("Waiting for Bob's C1 point...\n");
	NetworkReceivePoint(Socket_Bob, &Point_C1);
	PointShow(&Point_C1);
	putchar('\n');
	
	// Get C2
	printf("Waiting for Bob's C2 point...\n");
	NetworkReceivePoint(Socket_Bob, &Point_C2);
	PointShow(&Point_C2);
	putchar('\n');
	
	// Retrieve Bob's message
	printf("Alice is deciphering the message...\n");
	// Compute a.C1 ('a' is Alice's private key)
	ECMultiplication(Pointer_Curve, &Point_C1, Private_Key_Alice, &Point_C1); // Store result in C1 as C1 value will no more be used
	// Substract C2 to the X coordinate of previous operation
	mpz_sub(Output_Message, Point_C2.X, Point_C1.X);
	mpz_mod(Output_Message, Output_Message, Pointer_Curve->p);
		
	// Free memory
	PointFree(&Point_C1);
	PointFree(&Point_C2);
}

/** Send a message to Alice (client side of the protocol).
 * @param Pointer_Curve The curve used for computations.
 * @param Socket_Alice The way used to communicate with Alice.
 * @param Message The message to send.
 */
static void ElGamalBob(TEllipticCurve *Pointer_Curve, int Socket_Alice, mpz_t Message)
{
	TPoint Point_Public_Key_Alice, Point_Temp;
	mpz_t Number_K, Number_Temp;
	
	// Initialize variables
	PointCreate(0, 0, &Point_Public_Key_Alice);
	PointCreate(0, 0, &Point_Temp);
	mpz_init(Number_K);
	mpz_init(Number_Temp);

	// Receive Alice's public key
	printf("Waiting for Alice's public key...\n");
	NetworkReceivePoint(Socket_Alice, &Point_Public_Key_Alice);
	PointShow(&Point_Public_Key_Alice);
	putchar('\n');
	
	// Compute C1
	printf("Bob is computing C1...\n");
	// Choose random number 'k'
	UtilsGenerateRandomNumber(Pointer_Curve->p, Number_K);
	// Do C1 computation
	ECMultiplication(Pointer_Curve, &Pointer_Curve->Point_Generator, Number_K, &Point_Temp);
	PointShow(&Point_Temp);
	// Send C1 to Alice
	NetworkSendPoint(Socket_Alice, &Point_Temp);
	printf("C1 sent to Alice.\n\n");
	
	// Compute C2
	printf("Bob is computing C2...\n");
	// Compute k.Q
	ECMultiplication(Pointer_Curve, &Point_Public_Key_Alice, Number_K, &Point_Temp);
	// Add message and the X coordinate of the previous result
	mpz_add(Number_Temp, Message, Point_Temp.X);
	mpz_mod(Point_Temp.X, Number_Temp, Pointer_Curve->p); // The number must stay into the group
	PointShow(&Point_Temp);
	// Send a "false" C2 to Alice where the only important coordinate is X
	NetworkSendPoint(Socket_Alice, &Point_Temp);
	printf("C2 sent to Alice.\n\n");
	
	// Free memory
	PointFree(&Point_Public_Key_Alice);
	PointFree(&Point_Temp);
	mpz_clear(Number_K);
	mpz_clear(Number_Temp);
}

int main(int argc, char *argv[])
{
	char Is_Alice, *String_Parameter_Character, *String_Parameter_File_Name, *String_Parameter_IP_Address;
	unsigned short Port;
	TEllipticCurve Curve;
	int Socket_Alice, Socket_Bob;
	mpz_t Private_Key_Alice, Message, Number_Temp;
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
	
	// Initialize variables
	PointCreate(0, 0, &Point_Public_Key_Alice);
	mpz_init(Message);
	
	// Alice
	if (Is_Alice)
	{
		// Start server
		Socket_Alice = NetworkServerCreate(String_Parameter_IP_Address, Port);
		if (Socket_Alice < 0)
		{
			printf("Error : could not create the server.\n");
			ECFree(&Curve);
			mpz_clear(Private_Key_Alice);
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
			mpz_clear(Private_Key_Alice);
			return -5;
		}
		printf("Bob is connected.\n\n");
		
		// Initialize variables
		mpz_init(Private_Key_Alice);
		
		// Generate Alice's key pair once
		UtilsGenerateRandomNumber(Curve.p, Private_Key_Alice);
		gmp_printf("Alice's private key is :\n%Zd\n\n", Private_Key_Alice);
		
		// Compute Alice's public key 'Q'
		printf("Alice is computing her public key...\n");
		ECMultiplication(&Curve, &Curve.Point_Generator, Private_Key_Alice, &Point_Public_Key_Alice);
		PointShow(&Point_Public_Key_Alice);
		putchar('\n');
		
		// Get Bob's message
		ElGamalAlice(&Curve, Socket_Bob, &Point_Public_Key_Alice, Private_Key_Alice, Message);
		gmp_printf("Message is : %Zd\n", Message);
		
		// Free resources
		mpz_clear(Private_Key_Alice);
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
		
		// Initialize variables
		mpz_init(Number_Temp);
		
		// Find a message to send
		mpz_set_ui(Number_Temp, 10000); // A number between 0 and 9999
		UtilsGenerateRandomNumber(Number_Temp, Message);
		gmp_printf("Message to send :\n%Zd\n\n", Message);
		
		// Send message to Alice
		ElGamalBob(&Curve, Socket_Alice, Message);
		
		// Free resources
		mpz_clear(Number_Temp);
	}
	
	// Free resources
	PointFree(&Point_Public_Key_Alice);
	mpz_clear(Message);
	close(Socket_Alice);
	ECFree(&Curve);
	return 0;
}