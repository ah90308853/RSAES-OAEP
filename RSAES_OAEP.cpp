#include "RSAES_OAEP.h"


RSAES_OAEP::RSAES_OAEP()
{
	seed = time(NULL);
	srand(seed);
	gmp_randinit_mt(rstate);
	gmp_randseed_ui(rstate, seed);
	
	mpz_inits(p, q, n, totient, e, d, NULL);
	keys();
}

void RSAES_OAEP::keys()
{
	generatePrime(p);
	generatePrime(q);
	calculateTotient();
	calculateN();
	generateE();
	calculateD();
	calculatek();
}

void RSAES_OAEP::generatePrime(mpz_t target)
{
	mpz_t low, random;
	mpz_inits(low, random, NULL);
	
	mpz_ui_pow_ui(low, 10, 100);
	
	do {
			mpz_urandomb(random, rstate, 300);
			mpz_add(random, random, low);
		
		} while ((mpz_probab_prime_p(random, 25)) == 0);
	
	mpz_set(target, random);
	mpz_clears(low, random, NULL);
}

void RSAES_OAEP::calculateTotient()
{
	mpz_t pMinus, qMinus;
	mpz_inits(pMinus, qMinus, NULL);
	
	mpz_sub_ui(pMinus, p, 1);
	mpz_sub_ui(qMinus, q, 1);
	
	mpz_mul(totient, pMinus, qMinus);
	mpz_clears(pMinus, qMinus, NULL);
}

void RSAES_OAEP::generateE()
{
	mpz_t GCD;
	mpz_init(GCD);
	
	mpz_set_ui(e, 0);
	do {
			mpz_nextprime(e, e);
			mpz_gcd(GCD, e, totient);
			
		} while (mpz_cmp_ui(GCD, 1) != 0);
	mpz_clear(GCD);
}

void RSAES_OAEP::calculateD()
{
	mpz_invert(d, e, totient);
}

void RSAES_OAEP::calculatek()
{
	mpz_t tempn;
	mpz_init(tempn);
	mpz_set(tempn, n);
	k = countOctets(tempn);
	mpz_clear(tempn);
}

void RSAES_OAEP::calculateN()
{
	mpz_mul(n, p, q);
}

string RSAES_OAEP::encrypt(string toEncrypt)
{
	mpz_t encrypted, x;
	mpz_inits(encrypted, x, NULL);
	mpz_set_str(x, toEncrypt.c_str(), 16);

	mpz_powm(encrypted, x, e, n);
	
	char buffer[200];
	mpz_get_str(buffer, 16, encrypted);
	string stringEncrypted(buffer);
	
	mpz_clears(encrypted, x, NULL);
	return stringEncrypted;
}

string RSAES_OAEP::decrypt(string toDecrypt)
{
	mpz_t decrypted, x;
	mpz_inits(decrypted, x, NULL);
	mpz_set_str(x, toDecrypt.c_str(), 16);
	
	mpz_powm(decrypted, x, d, n);
	
	char buffer[200];
	mpz_get_str(buffer, 16, decrypted);
	string stringDecrypted(buffer);
	
	mpz_clears(decrypted, x, NULL);
	return stringDecrypted;
}

void RSAES_OAEP::openInFile(string inFileName)
{
	inFile.open(inFileName);
	if(!inFile)
	{
		cout << "Error\n";
	}
}

void RSAES_OAEP::closeInFile()
{
	inFile.close();
}

void RSAES_OAEP::openOutFile(string outFileName)
{
	outFile.open(outFileName);
	if (!outFile)
	{
		cout << "Error\n";
	}
}

void RSAES_OAEP::closeOutFile()
{
	outFile.close();
}

string RSAES_OAEP::generatePS()
{
	int octetLength;
	octetLength = 0;
	string PS;
	
	while (octetLength < (k - mLen - (2 * hLen) - 2))
	{
		PS.append("00");
		octetLength++;
	}
	
	return PS;
}


string RSAES_OAEP::generatelHash(string L)
{
	string lHash;
	md5.resetVariables();
	md5.hashString(L);
	md5.genDigest();
	lHash = md5.getDigest();
	return lHash;
}

string RSAES_OAEP::generateDB(string L, int x)
{
	string lHash, PS, M;
	lHash = generatelHash(L);
	cout << "	lHash: " << lHash << endl;
	PS = generatePS();
	M = I2OSP(x, mLen);
	cout << "	plaintext char as int M: " << M << endl;
	
	string DB;
	DB = lHash + PS + "01" + M;
	cout << "	unmasked DB: " << DB << endl;
	
	return DB;
}

string RSAES_OAEP::generateRandomSeed()
{
	stringstream constructSeed;
	constructSeed << internal << hex;
	for (int i = 0; i < hLen; i++)
	{
		int random = rand() % 256;
		constructSeed << setfill('0') << setw(2) << random;
	}
	return constructSeed.str();
}

string RSAES_OAEP::generateMask(string input, int length)
{
	string mask;
	mask = md5.maskString(input, length, hLen);
	return mask;
}

string RSAES_OAEP::maskString(string mask, string input, int length)
{
	string inputMask;
	inputMask = generateMask(mask, length);
	
	string binaryInput, binaryInputMask, binaryMaskedInput;
	binaryInput = octetToBinary(input);
	binaryInputMask = octetToBinary(inputMask);
	binaryMaskedInput = xorString(binaryInput, binaryInputMask);
	
	string maskedInput;
	maskedInput = binaryStringToOctet(binaryMaskedInput);
	return maskedInput;
}




string RSAES_OAEP::encodeBlock(string L, int x)
{
	string DB;
	DB = generateDB(L, x);
	
	string randomSeed;
	randomSeed = generateRandomSeed();
	cout << "	randomSeed: " << randomSeed << endl;
	
	string maskedDB;
	maskedDB = maskString(randomSeed, DB, (k - hLen - 1) * 2);
	
	string maskedRandomSeed;
	maskedRandomSeed = maskString(maskedDB, randomSeed, (hLen * 2));
	
	string EM;
	EM = "00" + maskedRandomSeed + maskedDB;
	cout << "	masked EM: " << EM << endl;
	
	string octetCiphertext;
	octetCiphertext = encrypt(EM);
	cout << "	encrypted EM (final ciphertext value): " << octetCiphertext << endl << endl;
	
	return octetCiphertext;
}

void RSAES_OAEP::EME_OAEPencoding()
{
	openInFile("toEncrypt.txt");
	openOutFile("toDecrypt.txt");
	string line, C;
	int intM;
	intM = 0;
	while(getline(inFile, line))
	{
		for (int i = 0; i < line.length(); i++)
		{
			cout << "encrypting: " << line[i] << endl;
			intM = (int)line[i];
			C = encodeBlock("", intM);
			outFile << C << endl;
		}
	}
	closeInFile();
	closeOutFile();
}

string RSAES_OAEP::decodeBlock(string toDecode)
{
	cout << "decoding: " << toDecode << endl;
	string decryptedEM;
	decryptedEM = decrypt(toDecode);
	
	string leadingZeros;
	for (int i = 0; i < ((k * 2) - decryptedEM.length()); i++)
	{
		leadingZeros += '0';
	}
	decryptedEM.insert(0, leadingZeros);
	cout << "	decryptedEM: " << decryptedEM << endl;
	
	string maskedSeed;
	maskedSeed = decryptedEM.substr(2, hLen * 2);
	cout << "	masked seed: " << maskedSeed << endl;
		
	string maskedDB;
	maskedDB = decryptedEM.substr(2 + (hLen * 2));
	
	string extractedSeed;
	extractedSeed = maskString(maskedDB, maskedSeed, (hLen * 2));
	cout << "	unmasked seed: " << extractedSeed << endl;
	
	string extractedDB;
	extractedDB = maskString(extractedSeed, maskedDB, ((k - hLen -1) * 2));
	
	string extractedlHash, extractedM;
	extractedlHash = extractedDB.substr(0, 32);
	cout << "	unmasked lHash: " << extractedlHash << endl;
		
	size_t index;
	index = extractedDB.find("01", 31);
		
	extractedM = extractedDB.substr(index + 2);
	cout << "	unmasked M: " << extractedM << endl;
	
	return extractedM;
}



void RSAES_OAEP::EME_OAEPdecoding()
{
	openInFile("toDecrypt.txt");
	openOutFile("decrypted.txt");
	string line;
	mpz_t x, decrypted;
	mpz_inits(x, decrypted, NULL);
	while(getline(inFile, line))
	{
		string extractedM;
		extractedM = decodeBlock(line);
		int characterConversion;
		characterConversion = stoi(extractedM, nullptr, 16);
		char character;
		character = (char)characterConversion;
		cout << "	decrypted character: " << character << endl << endl;
		outFile << character;
	}
}