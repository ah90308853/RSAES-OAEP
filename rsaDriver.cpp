#include "RSAES_OAEP.cpp"

int main()
{
	RSAES_OAEP rsa;
	rsa.EME_OAEPencoding();
	rsa.EME_OAEPdecoding();
}