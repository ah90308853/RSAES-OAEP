#include "libraries.h"
#ifndef util
#define util

using namespace std;

string octetToBinary(string octet)
{
	string binary;
	for (int i = 0; i < octet.length(); i++)
	{
		switch(octet[i])
		{
			case '0':
				binary.append("0000");
				break;
			case '1':
				binary.append("0001");
				break;
			case '2':
				binary.append("0010");
				break;
			case '3':
				binary.append("0011");
				break;
			case '4':
				binary.append("0100");
				break;
			case '5':
				binary.append("0101");
				break;
			case '6':
				binary.append("0110");
				break;
			case '7':
				binary.append("0111");
				break;
			case '8':
				binary.append("1000");
				break;
			case '9':
				binary.append("1001");
				break;
			case 'a':
				binary.append("1010");
				break;
			case 'b':
				binary.append("1011");
				break;
			case 'c':
				binary.append("1100");
				break;
			case 'd':
				binary.append("1101");
				break;
			case 'e':
				binary.append("1110");
				break;
			case 'f':
				binary.append("1111");
				break;
			default:
				break;
		}
	}
	return binary;
}

char digitDecToHex(int x)
{
	if (x < 10)
	{
		char hexDigit;
		hexDigit = '0' + x;
		return hexDigit;
	}
	switch(x)
	{
		case 10:
			return 'a';
		case 11:
			return 'b';
		case 12:
			return 'c';
		case 13:
			return 'd';
		case 14:
			return 'e';
		case 15:
			return 'f';
	}
}

string binaryStringToOctet(string binary)
{
	string hexDigit, octet;
	for (int i = 0; i < binary.length(); i += 4)
	{
		hexDigit = binary.substr(i, 4);
		if (!hexDigit.compare("0000"))
			octet += '0';
		else if (!hexDigit.compare("0001"))
			octet += '1';
		else if (!hexDigit.compare("0010"))
			octet += '2';
		else if (!hexDigit.compare("0011"))
			octet += '3';
		else if (!hexDigit.compare("0100"))
			octet += '4';
		else if (!hexDigit.compare("0101"))
			octet += '5';
		else if (!hexDigit.compare("0110"))
			octet += '6';
		else if (!hexDigit.compare("0111"))
			octet += '7';
		else if (!hexDigit.compare("1000"))
			octet += '8';
		else if (!hexDigit.compare("1001"))
			octet += '9';
		else if (!hexDigit.compare("1010"))
			octet += 'a';
		else if (!hexDigit.compare("1011"))
			octet += 'b';
		else if (!hexDigit.compare("1100"))
			octet += 'c';
		else if (!hexDigit.compare("1101"))
			octet += 'd';
		else if (!hexDigit.compare("1110"))
			octet += 'e';
		else if (!hexDigit.compare("1111"))
			octet += 'f';
	}
	return octet;
}

string xorString(string x, string y)
{
	int xLength, yLength;
	xLength = x.length();
	yLength = y.length();
	
	if (xLength < yLength)
	{
		for (int i = 0; i < yLength - xLength; i++)
		{
			x = '0' + x;
		}
	}
	if (yLength < xLength)
	{
		for (int i = 0; i < xLength - yLength; i++)
		{
			y = '0' + y;
		}
	}
	
	int length = max(xLength, yLength);
	string output;
	for (int i = 0; i < length; i++)
	{
		if (x[i] == y[i])
			output += '0';
		else
			output += '1';
	}
	return output;
}

string I2OSP(mpz_t x, int xLen)
{
	stringstream digits;
	digits << internal << hex << setfill('0') << setw(2);
	while(mpz_cmp_ui(x, 0) != 0)
	{
		mpz_t modX;
		mpz_init(modX);
		mpz_mod_ui(modX, x, 256);
		int intModX = mpz_get_ui(modX);
		digits << intModX;
		mpz_fdiv_q_ui(x, x, 256);
		mpz_clear(modX);
	}
	
	string stringDigits = digits.str();
	int digitsLength = stringDigits.length();
	string leadingZero;
	for (int i = 0; i < (xLen - digitsLength); i++)
	{
		leadingZero.append("00");
		digitsLength++;
	}
	
	stringDigits.insert(0, leadingZero);
	return stringDigits;
}

string I2OSP(int x, int xLen)
{
	stringstream digits;
	digits << internal << hex << setfill('0') << setw(2);
	while(x != 0)
	{
		int modX;
		modX = x % 256;
		digits << modX;
		x /= 256;
	}
	string stringDigits = digits.str();
	int length = stringDigits.length();
	string leadingZero;
	for (int i = 0; i < (xLen * 2) - length; i++)
	{
		leadingZero += '0';
	}
	stringDigits.insert(0, leadingZero);
	return stringDigits;
}

int countOctets(mpz_t x)
{
	int octetCount = 0;
	while(mpz_cmp_ui(x, 0) != 0)
	{
		mpz_fdiv_q_ui(x, x, 256);
		octetCount++;
	}
	return octetCount;
}

int countOctets(string input)
{
	return input.length() / 2;
}
#endif