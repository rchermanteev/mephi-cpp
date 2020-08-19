#include "EncryptionRC2.h"

unsigned char* encriptRC2(unsigned char* text, const unsigned char* key, const int length_key)
{
	unsigned char out[9];
	//unsigned char iv[] = "";

	out[8] = 0;

	RC2_KEY wctx;

	RC2_set_key(&wctx, length_key, key, 8 * length_key);
	RC2_ecb_encrypt(text, out, &wctx, RC2_ENCRYPT);
	//RC2_cbc_encrypt(text, out, sizeof(out), &wctx, iv, RC2_ENCRYPT);

	return out;
}

unsigned char* decryptRC2(unsigned char* cipher, const unsigned char* key, const int length_key)
{
	unsigned char decout[9];

	decout[8] = 0;

	RC2_KEY wctx;

	RC2_set_key(&wctx, length_key, key, 8 * length_key);
	RC2_ecb_encrypt(cipher, decout, &wctx, RC2_DECRYPT);

	return decout;
}