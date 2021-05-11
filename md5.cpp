#include "md5.h"

MD5::MD5()
{
	a0 = 0x67452301;
	b0 = 0xefcdab89;
	c0 = 0x98badcfe;
	d0 = 0x10325476;
}

void MD5::resetVariables()
{
	a0 = 0x67452301;
	b0 = 0xefcdab89;
	c0 = 0x98badcfe;
	d0 = 0x10325476;
}

std::string MD5::textToBinary(std::string input)
{
	if (input.empty())
		return input;
	const int length = input.length();
	std::string output;
	
	for (int i = 0; i < length; i++)
	{
		int ascii = (int)input[i];
		while (ascii != 0)
		{
			if (ascii % 2 == 1)
				output += '1';
			else
				output += '0';
			ascii /= 2;
		}
	}
	return output;
}

void MD5::hashString(std::string inputMessage)
{
	message = inputMessage;
	formatInput();
	MD5Hash();
}

std::string MD5::maskString(std::string Z, int l, int hLen)
{
	std::string T, concatenateZC, mask;
	if (l <= hLen)
	{
		std::string C;
		C = I2OSP(0, 4);
		concatenateZC.append(Z);
		concatenateZC.append(C);
		message = concatenateZC;
		formatInput();
		resetVariables();
		MD5Hash();
		genDigest();
		T.append(getDigest());
		
		for (int i = 0; i < l; i++)
		{
			mask += T[i];
		}
		return mask;
	}
	
	for (int i = 0; i < ((l/hLen) - 1); i++)
	{
		std::string C;
		C = I2OSP(i, 4);
		concatenateZC.append(Z);
		concatenateZC.append(C);
		message = concatenateZC;
		formatInput();
		resetVariables();
		MD5Hash();
		genDigest();
		T.append(getDigest());
	}
	
	for (int i = 0; i < l; i++)
	{
		mask += T[i];
	}
	return mask;
}

void MD5::formatInput()
{
	message = textToBinary(message);
	//cout << "binary message: " << message << endl;
	message.append("1");
	
    const int MESSAGE_LEN = message.length();
    
    std::string stringMESSAGE_LEN = toBinary(MESSAGE_LEN);
    int stringMESSAGE_LEN_Length = stringMESSAGE_LEN.length();
	
    if (stringMESSAGE_LEN_Length < 64)
    {
        for (int i = 0; i < (64 - stringMESSAGE_LEN_Length); i++)
        {
            stringMESSAGE_LEN.insert(0, "0");
        }
    }
	
	int paddedMessageLength = MESSAGE_LEN;
    while (paddedMessageLength % 512 != 448)
    {
        message.append("0");
        paddedMessageLength = message.length();
    }
	
    message.append(stringMESSAGE_LEN);
    paddedMessageLength = message.length();

    std::string messageChunk;
    int numberOfChunks = paddedMessageLength / 512;
	
    //std::cout << "number of chunks: " << numberOfChunks << std::endl;
    //std::cout << "padded length: " << paddedMessageLength << std::endl;
	
	messageChunks.clear();
    for (int i = 0; i < numberOfChunks; i++)
    {
        messageChunk = message.substr((i * 512), 512);
        messageChunks.push_back(messageChunk);
    }
}

void MD5::genDigest()
{
	digest.clear();
    digest.append(formatOutput(a0));
    digest.append(formatOutput(b0));
    digest.append(formatOutput(c0));
    digest.append(formatOutput(d0));
}


std::string MD5::toBinary(std::uint32_t n)
{
    std::string output;
    while (n != 0)
    {
        if (n % 2 == 0)
        {
            output.insert(0, "0");
        }
        else
        {
            output.insert(0, "1");
        }
        n /= 2;
    }
    return output;
}

std::uint32_t MD5::toInt(std::string binaryString)
{
    int numberOfDigits = binaryString.length();
    std::uint32_t output = 0;
    for (int i = 0; i < numberOfDigits; i++)
    {
        if (binaryString[numberOfDigits - (i + 1)] == '1')
        {
            output += (std::uint32_t)pow(2, i);
        }
    }
    return output;
}

std::uint32_t MD5::leftRotate(std::uint32_t x, std::uint32_t c)
{
    return (x << c) | (x >> (32 - c));
}

std::string MD5::formatOutput(std::uint32_t input)
{
    std::ostringstream output;
    output << std::internal << std::hex << std::setfill('0') <<
        std::setw(8) << input;
    return output.str();
}

void MD5::MD5Hash()
{
	for (int x = 0; x < 1000; x++)
	{
		for (int i = 0; i < messageChunks.size(); i++)
		{
			std::uint32_t A = a0;
			std::uint32_t B = b0;
			std::uint32_t C = c0;
			std::uint32_t D = d0;

			std::uint32_t M[16];
			std::string segment;
			for (int j = 0; j < 16; j++)
			{
				segment = (messageChunks[i]).substr((j * 32), 32);
				M[j] = toInt(segment);
			}

			for (int k = 0; k < 64; k++)
			{
				std::uint32_t F;
				int g;
				if (k >= 0 && k <= 15)
				{
					F = ((B & C) | ((~B) & D)) % 0xFFFFFFFF;
					g = k;
				}
				else if (k >= 16 && k <= 31)
				{
					F = ((D & B) | ((~D) & C)) % 0xFFFFFFFF;
					g = (5 * k + 1) % 16;
				}
				else if (k >= 32 && k <= 47)
				{
					F = (B ^ C ^ D) % 0xFFFFFFFF;
					g = (3 * k + 5) % 16;
				}
				else if (k >= 48 && k <= 63)
				{
					F = (C ^ (B | (~D))) % 0xFFFFFFFF;
					g = (7 * k) % 16;
				}
				F = (F + A + sinesOfIntegers_K[k] + M[g]) % 0xFFFFFFFF;
				A = D;
				D = C;
				C = B;
				B = (B + leftRotate(F, shiftAmount_s[k])) % 0xFFFFFFFF;
			}

			a0 = (a0 + A) % 0xFFFFFFFF;
			b0 = (b0 + B) % 0xFFFFFFFF;
			c0 = (c0 + C) % 0xFFFFFFFF;
			d0 = (d0 + D) % 0xFFFFFFFF;
		} 
	}
	for (int i = 0; i < messageChunks.size(); i++)
	{
		std::uint32_t A = a0;
		std::uint32_t B = b0;
		std::uint32_t C = c0;
		std::uint32_t D = d0;

		std::uint32_t M[16];
		std::string segment;
		for (int j = 0; j < 16; j++)
		{
			segment = (messageChunks[i]).substr((j * 32), 32);
			M[j] = toInt(segment);
		}

		for (int k = 0; k < 64; k++)
		{
			std::uint32_t F;
			int g;
			if (k >= 0 && k <= 15)
			{
				F = ((B & C) | ((~B) & D)) % 0xFFFFFFFF;
				g = k;
			}
			else if (k >= 16 && k <= 31)
			{
				F = ((D & B) | ((~D) & C)) % 0xFFFFFFFF;
				g = (5 * k + 1) % 16;
			}
			else if (k >= 32 && k <= 47)
			{
				F = (B ^ C ^ D) % 0xFFFFFFFF;
				g = (3 * k + 5) % 16;
			}
			else if (k >= 48 && k <= 63)
			{
				F = (C ^ (B | (~D))) % 0xFFFFFFFF;
				g = (7 * k) % 16;
			}
			F = (F + A + sinesOfIntegers_K[k] + M[g]) % 0xFFFFFFFF;
			A = D;
			D = C;
			C = B;
			B = (B + leftRotate(F, shiftAmount_s[k])) % 0xFFFFFFFF;
		}

		a0 = (a0 + A) % 0xFFFFFFFF;
		b0 = (b0 + B) % 0xFFFFFFFF;
		c0 = (c0 + C) % 0xFFFFFFFF;
		d0 = (d0 + D) % 0xFFFFFFFF;
	} 
}

std::string MD5::getDigest()
{
    return this->digest;
}


