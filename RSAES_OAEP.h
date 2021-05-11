#include "libraries.h"
#include "md5.cpp"
#include "util.cpp"

using namespace std;

class RSAES_OAEP
{
	private:
		MD5 md5;
		mpz_t p;
		mpz_t q;
		mpz_t n;
		mpz_t totient;
		mpz_t e;
		mpz_t d;
		int k;
		const int mLen = 10;
		const int hLen = 16;
		
		unsigned long seed;
		gmp_randstate_t rstate;
		
		ifstream inFile;
		ofstream outFile;

	public:
		RSAES_OAEP();
		void keys();
		void generatePrime(mpz_t);
		void calculateTotient();
		void calculateN();
		void generateE();
		void calculateD();
		void calculatek();
		string encrypt(string);
		string decrypt(string);
		void openInFile(string);
		void closeInFile();
		void openOutFile(string);
		void closeOutFile();
		string generatePS();
		string generatelHash(string);
		string generateDB(string, int);
		string generateRandomSeed();
		string generateMask(string, int);
		string maskString(string, string, int);
		string encodeBlock(string, int);
		string decodeBlock(string);
		void EME_OAEPencoding();
		void EME_OAEPdecoding();
};
