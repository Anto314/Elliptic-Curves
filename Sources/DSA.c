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

/** Maximum number of bytes (including the trailing zero) of the message. */
#define MAXIMUM_MESSAGE_SIZE 2048

/** Check if the number is comprised between 1 and Number_Order - 1.
 * @param Number The number to check.
 * @param Number_Order The order of the group.
 * @return 1 if the number is in bounds or 0 if not.
 */
static int IsNumberInBounds(mpz_t Number, mpz_t Number_Order)
{
	if (mpz_cmp_ui(Number, 1) < 0) return 0;
	if (mpz_cmp(Number, Number_Order) >= 0) return 0;
	return 1;
}

static void DSAAlice(TEllipticCurve *Pointer_Curve, unsigned char *Pointer_Message, size_t Message_Length, mpz_t Private_Key_Alice, mpz_t Output_Number_U, mpz_t Output_Number_V)
{
	unsigned char Buffer_Hash[UTILS_HASH_LENGTH];
	mpz_t Number_Temp, Number_Hash, Number_Random;
	TPoint Point;
	
	// Initialize variables
	mpz_init(Number_Temp);
	mpz_init(Number_Hash);
	mpz_init(Number_Random);
	PointCreate(0, 0, &Point);
	
	// Compute message hash
	printf("Alice is computing message hash...\n");
	UtilsComputeHash(Pointer_Message, Message_Length, Buffer_Hash);
	mpz_set_str(Number_Hash, (char *) Buffer_Hash, 10);
	UtilsShowHash(Buffer_Hash);
	putchar('\n');
	
	// Generate signature pair (u, v)
	while (1)
	{
		// Get a random K between 1 et n - 1
		do
		{
			UtilsGenerateRandomNumber(Pointer_Curve->n, Number_Random);
		} while (!IsNumberInBounds(Number_Random, Pointer_Curve->n));
		
		// Compute a curve point
		ECMultiplication(Pointer_Curve, &Pointer_Curve->Point_Generator, Number_Random, &Point);
	
		// Calculate 'u'
		mpz_mod(Output_Number_U, Point.X, Pointer_Curve->n);
		// Retry if the computed 'u' is 0
		if (mpz_cmp_ui(Output_Number_U, 0) == 0) continue;
		
		// Calculate 'v'
		mpz_mul(Number_Temp, Output_Number_U, Private_Key_Alice); // u * s
		mpz_add(Number_Temp, Number_Temp, Number_Hash); // H(m) + (u * s)
		mpz_invert(Output_Number_V, Number_Random, Pointer_Curve->n); // Compute k^-1 mod n
		mpz_mul(Number_Temp, Output_Number_V, Number_Temp); // (k^-1) * (H(m) + (u * s))
		mpz_mod(Output_Number_V, Number_Temp, Pointer_Curve->n); // (k^-1) * (H(m) + (u * s)) mod n
		
		// Retry if the computed 'v' is 0
		if (mpz_cmp_ui(Output_Number_V, 0) != 0) break;
	}
	
	// Display signature pair
	gmp_printf("Signature :\nu = %Zd\nv = %Zd\n\n", Output_Number_U, Output_Number_V);
	
	// Free resources
	mpz_clear(Number_Temp);
	mpz_clear(Number_Hash);
	mpz_clear(Number_Random);
	PointFree(&Point);
}

static int DSABob(TEllipticCurve *Pointer_Curve, unsigned char *Pointer_Message, size_t Message_Length, TPoint *Pointer_Public_Key_Alice, mpz_t Number_U, mpz_t Number_V)
{
	unsigned char Buffer_Hash[UTILS_HASH_LENGTH];
	mpz_t Number_Hash, Number_Temp;
	TPoint Point_Temp, Point_Temp_2;
	int Return_Value;
	
	// Initialize variables
	mpz_init(Number_Hash);
	mpz_init(Number_Temp);
	PointCreate(0, 0, &Point_Temp);
	PointCreate(0, 0, &Point_Temp_2);
		
	// Check parameters correctness
	printf("Bob is checking parameters correctness... ");
	fflush(stdout);
	
	// 'u' must be between 1 and n - 1)
	if (!IsNumberInBounds(Number_U, Pointer_Curve->n))
	{
		printf("\nError : 'u' is not in [1; n - 1].\n");
		return 0;
	}

	// 'v' must be between 1 and n - 1)
	if (!IsNumberInBounds(Number_V, Pointer_Curve->n))
	{
		printf("\nError : 'v' is not in [1; n - 1].\n");
		return 0;
	}
	
	// Alice's public key (Q) must not be equal to (0, 0)
	if (Pointer_Public_Key_Alice->Is_Infinite)
	{
		printf("\nError : Alice's public key is equal to (0, 0).\n");
		return 0;
	}
	
	// n * Q must be equal to (0, 0)
	ECMultiplication(Pointer_Curve, Pointer_Public_Key_Alice, Pointer_Curve->n, &Point_Temp);
	if (!Point_Temp.Is_Infinite)
	{
		printf("\nError : n.Q != (0, 0).\n");
		PointShow(&Point_Temp);
		return 0;
	}
	printf("done.\n\n");
	
	// Compute message hash
	printf("Bob is computing message hash...\n");
	UtilsComputeHash(Pointer_Message, Message_Length, Buffer_Hash);
	mpz_set_str(Number_Hash, (char *) Buffer_Hash, 10);
	UtilsShowHash(Buffer_Hash);
	putchar('\n');
	
	printf("Bob is checking signature...\n");
	// Compute (H(m) / v) mod n using v^-1
	mpz_invert(Number_V, Number_V, Pointer_Curve->n); // Only v^-1 is used by other calculations
	mpz_mul(Number_Temp, Number_Hash, Number_V);
	mpz_mod(Number_Temp, Number_Temp, Pointer_Curve->n);

	// Compute (H(m) / v mod n) * P
	ECMultiplication(Pointer_Curve, &Pointer_Curve->Point_Generator, Number_Temp, &Point_Temp);
	
	// Compute u / v mod n using v^-1
	mpz_mul(Number_Temp, Number_U, Number_V);
	mpz_mod(Number_Temp, Number_Temp, Pointer_Curve->n);
	
	// Multiply to public key Q
	ECMultiplication(Pointer_Curve, Pointer_Public_Key_Alice, Number_Temp, &Point_Temp_2);
	
	// Add the two points
	ECAddition(Pointer_Curve, &Point_Temp, &Point_Temp_2, &Point_Temp);
	
	// Is resulting point X coordinate equal to u ?
	mpz_mod(Number_Temp, Point_Temp.X, Pointer_Curve->n);
	gmp_printf("u = %Zd\nResult = %Zd\n", Number_U, Number_Temp);
	if (mpz_cmp(Number_U, Number_Temp) == 0)
	{
		printf("\033[32mSUCCESS : signature matched.\n");
		Return_Value = 1;
	}
	else
	{
		printf("\033[31mFAILURE : bad signature.\n");
		Return_Value = 0;
	}
	// Reset color attributes
	printf("\033[0m");
	
	// Free resources
	mpz_clear(Number_Hash);
	mpz_clear(Number_Temp);
	PointFree(&Point_Temp);
	PointFree(&Point_Temp_2);
	
	return Return_Value;
}

int main(int argc, char *argv[])
{
	char Is_Alice, *String_Parameter_Character, *String_Parameter_File_Name, *String_Parameter_IP_Address, Message[MAXIMUM_MESSAGE_SIZE];
	unsigned short Port;
	TEllipticCurve Curve;
	int Socket_Alice, Socket_Bob, Message_Length;
	mpz_t Private_Key_Alice, Signature_Number_U, Signature_Number_V;
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
	mpz_init(Signature_Number_U);
	mpz_init(Signature_Number_V);
	PointCreate(0, 0, &Point_Public_Key_Alice);
	
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
		
		// Initialize variables
		mpz_init(Private_Key_Alice);
		
		// Generate Alice's private key 's' once
		printf("Computing Alice's private key...\n");
		do
		{
			UtilsGenerateRandomNumber(Curve.n, Private_Key_Alice);
		} while (!IsNumberInBounds(Private_Key_Alice, Curve.n));
		gmp_printf("%Zd\n\n", Private_Key_Alice);
		
		// Generate public key 'Q' = private key * curve point
		printf("Computing Alice's public key...\n");
		ECMultiplication(&Curve, &Curve.Point_Generator, Private_Key_Alice, &Point_Public_Key_Alice);
		PointShow(&Point_Public_Key_Alice);
		putchar('\n');
		
		// Send public key to Bob
		printf("Sending public key to Bob... ");
		fflush(stdout);
		NetworkSendPoint(Socket_Bob, &Point_Public_Key_Alice);
		printf("done.\n\n");
		
		// Create message
		strcpy(Message, "Ceci est un magnifique message de test.");
		Message_Length = strlen(Message) + 1; // +1 for terminating zero

		// Send message to Bob
		printf("Sending message to Bob...\n%s\n\n", Message);
		fflush(stdout);
		// Send message size
		write(Socket_Bob, &Message_Length, sizeof(Message_Length));
		// Send message content
		write(Socket_Bob, Message, Message_Length);
		
		// Compute 'u' and 'v' numbers used to sign the message
		DSAAlice(&Curve, (unsigned char *) Message, Message_Length, Private_Key_Alice, Signature_Number_U, Signature_Number_V);
				
		// Send 'u' and 'v' to Bob
		printf("Sending signature numbers to Bob... ");
		fflush(stdout);
		NetworkSendMPZ(Socket_Bob, Signature_Number_U);
		NetworkSendMPZ(Socket_Bob, Signature_Number_V);
		printf("done.\n");
		
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
		
		// Receive Alice's public key
		printf("Waiting for Alice's public key...\n");
		NetworkReceivePoint(Socket_Alice, &Point_Public_Key_Alice);
		PointShow(&Point_Public_Key_Alice);
		putchar('\n');
		
		// Receive message length
		printf("Receiving Alice's message...\n");
		read(Socket_Alice, &Message_Length, sizeof(Message_Length));
		if (Message_Length > MAXIMUM_MESSAGE_SIZE)
		{
			printf("Error : the message is bigger than the buffer.\n");
			goto Exit;
		}
		// Receive message content
		read(Socket_Alice, Message, Message_Length);
		printf("%s\n\n", Message);
		
		// Receive signature
		printf("Receiving signature...\n");
		NetworkReceiveMPZ(Socket_Alice, Signature_Number_U);
		NetworkReceiveMPZ(Socket_Alice, Signature_Number_V);
		gmp_printf("r = %Zd\ns = %Zd\n\n", Signature_Number_U, Signature_Number_V);
		
		DSABob(&Curve, (unsigned char *) Message, Message_Length, &Point_Public_Key_Alice, Signature_Number_U, Signature_Number_V);
	}
	
Exit:
	// Free resources
	close(Socket_Alice);
	ECFree(&Curve);
	mpz_clear(Signature_Number_U);
	mpz_clear(Signature_Number_V);
	PointFree(&Point_Public_Key_Alice);
	return 0;
}