#include "rsa.h"
#include "arc4.h"
#include "stdlib.h"

void build_decoding_table();
static char encoding_table[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
'w', 'x', 'y', 'z', '0', '1', '2', '3',
'4', '5', '6', '7', '8', '9', '+', '/' };
static char *decoding_table = NULL;
static int mod_table[] = { 0, 2, 1 };

typedef  unsigned int uint32_t;

char *base64_encode(const unsigned char *data,
	size_t input_length,
	size_t *output_length) {
	char *encoded_data;
	int i = 0,j=0;

	*output_length = 4 * ((input_length + 2) / 3); 

	encoded_data = (char*)malloc(*output_length);
	if (encoded_data == 0) return 0;
	
	for (i = 0, j = 0; i < input_length;) {

		uint32_t octet_a = i < input_length ? (unsigned char)data[i++] : 0;
		uint32_t octet_b = i < input_length ? (unsigned char)data[i++] : 0;
		uint32_t octet_c = i < input_length ? (unsigned char)data[i++] : 0;

		uint32_t triple = (octet_a << 0x10) + (octet_b << 0x08) + octet_c;

		encoded_data[j++] = encoding_table[(triple >> 3 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 2 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 1 * 6) & 0x3F];
		encoded_data[j++] = encoding_table[(triple >> 0 * 6) & 0x3F];
	}

	for (i = 0; i < mod_table[input_length % 3]; i++)
		encoded_data[*output_length - 1 - i] = '=';

	return encoded_data;
}


unsigned char *base64_decode(const char *data,
	int input_length,
	int *output_length) {
	unsigned char *decoded_data;
	int i = 0,j=0;
	if (decoding_table == NULL) build_decoding_table();

	if (input_length % 4 != 0) return NULL;

	*output_length = input_length / 4 * 3;
	if (data[input_length - 1] == '=') (*output_length)--;
	if (data[input_length - 2] == '=') (*output_length)--;

	decoded_data = malloc(*output_length);
	if (decoded_data == NULL) return NULL;
	
	for (i = 0, j = 0; i < input_length;) {

		uint32_t sextet_a = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_b = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_c = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];
		uint32_t sextet_d = data[i] == '=' ? 0 & i++ : decoding_table[data[i++]];

		uint32_t triple = (sextet_a << 3 * 6)
			+ (sextet_b << 2 * 6)
			+ (sextet_c << 1 * 6)
			+ (sextet_d << 0 * 6);

		if (j < *output_length) decoded_data[j++] = (triple >> 2 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 1 * 8) & 0xFF;
		if (j < *output_length) decoded_data[j++] = (triple >> 0 * 8) & 0xFF;
	}

	return decoded_data;
}


void build_decoding_table() {
	int i = 0;
	decoding_table = malloc(256); 	
	for (i = 0; i < 64; i++)
		decoding_table[(unsigned char)encoding_table[i]] = i;
}


void base64_cleanup() {
	free(decoding_table);
}

const char*publicKeyBase64 = "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDK8PkROEWWP7LuUFVXB3OebuspYbtSo7qlEt5kKyWi9kURMmM3gBVeOxcd+LtnIEG4zofsPF4skSJxxY6caDOfhtc0G5bm8EELWn1Cs7J3fDNaa4QQFC/CtaWq4KVfaPuoPSXiILajKcR++u1IhSmhTGrJy4HlT4SOmWusTuc3HwIDAQAB";

const char*localPublicKeyBase64 =
		"MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDK8PkROEWWP7LuUFVXB3Oebusp"
		"YbtSo7qlEt5kKyWi9kURMmM3gBVeOxcd+LtnIEG4zofsPF4skSJxxY6caDOfhtc0"
		"G5bm8EELWn1Cs7J3fDNaa4QQFC/CtaWq4KVfaPuoPSXiILajKcR++u1IhSmhTGrJ"
		"y4HlT4SOmWusTuc3HwIDAQAB";
const char*localPrivateKeyBase64 =
		"MIICXAIBAAKBgQDK8PkROEWWP7LuUFVXB3OebuspYbtSo7qlEt5kKyWi9kURMmM3"
		"gBVeOxcd+LtnIEG4zofsPF4skSJxxY6caDOfhtc0G5bm8EELWn1Cs7J3fDNaa4QQ"
		"FC/CtaWq4KVfaPuoPSXiILajKcR++u1IhSmhTGrJy4HlT4SOmWusTuc3HwIDAQAB"
		"AoGAaPpADK1MKc2nlGlPy+YZ49HCqbRZfwUc7nkVO0GqhWLZFW04L6R86TKt4Z4h"
		"6tWfyaZkX0pW+LzW0X19KOEqGiXCyJ+OCCMhDzS/DeFl7YKymNiIUPhyQuiTK4De"
		"xs20O82ZCKZ+tQvLOh4FRS2Pxu0IoGNrjOR9haGZccNYmdECQQD3K9SWEVkCi0PQ"
		"YohCqKz4PuTGdKZS4UO7Xt2g6niQmeqxB1b5y7p/SyOsg8f7r8M4jvTT5K6CnY3w"
		"JwhEqzf3AkEA0jCyn0ZKhgcivTuj3fQaqUnJSHilRutZ2y/WXPl7jxKRVc2mrjDQ"
		"zLmihYyszfmiLHrLESdGISKVC5gmIxdAGQJAdNUw/WZhzoQJn/yTL/8PE6CW/v+w"
		"hsiX/X9P/3oVVzSJ7PftMtIL65OW3zrDUsK34oogS/ByVKcoqnWowNy17QJAEG/Y"
		"eBYiG8sRD2VLkj7Vmul3QSCWJC6hbz+Vbt3fqLeV//S+FR8iJPBbYkA2/0CHSx0r"
		"JiNBG7Q9nZN/X/0geQJBAL47/IFHe2DWY2KH+6PzNWt1s4iGpBb+aWBSESfLE6en"
		"+SJUM87dhk1LP4Y9g+d4oPsLjpoiBQTBtKC9g5mLAZM=";

#define FOURK_BUF 4096

//int arc4_test(void);

// int main(int argc, char** argv)
// {
// 	return 0;
// }
#ifdef __cplusplus
    extern "C" {
#endif

unsigned char*RSAEncrypt(unsigned char*input,int inLen,int *outLen)
{
	//arc4_test();
	byte*   tmp;
//	size_t bytes;
	RsaKey key;
//	RsaKey key1;
	RNG    rng;
	word32 idx = 0;
	int    ret;
	int len;
	int bufferLen = 1024;
	unsigned char*out;
	int encryptLen;
	//byte   in[] = "Everyone gets Friday off.";
	//word32 inLen = len
	if(inLen > 64 || inLen <= 0){
		return 0;
	}

	ret = wc_InitRsaKey(&key, 0);
	if (ret != 0) {
		return 0;
	}
	
	tmp = base64_decode(publicKeyBase64, strlen(publicKeyBase64), &len);
	ret = wc_RsaPublicKeyDecode(tmp, &idx, &key, len);
	if (ret != 0) {
		free(tmp);
		return 0;
	}

	ret = wc_InitRng(&rng);
	if (ret != 0) {
		free(tmp);
		return 0;
	}
	
	out = malloc(bufferLen);
	encryptLen = wc_RsaPublicEncrypt(input, inLen, out, bufferLen, &key, &rng);
	if (encryptLen < 0) {
		free(tmp);
		free(out);
		return 0;
	}
	*outLen = encryptLen;
	return out;
}

char*getEncryptedPasswordHex(char*password)
{
	byte*  in = (byte*)password;
	word32 inLen = strlen(password);

	byte*   tmp;
	RsaKey key;
	RNG    rng;
	word32 idx = 0;
	int    ret;
	byte   out[256];

	ret = wc_InitRsaKey(&key, 0);
	if (ret != 0) {
		return 0;
	}
	int len;
	tmp = base64_decode(localPublicKeyBase64, strlen(localPublicKeyBase64), &len);
	ret = wc_RsaPublicKeyDecode(tmp, &idx, &key, len);

	ret = wc_InitRng(&rng);
	if (ret != 0) {
		free(tmp);
		return 0;
	}
	int encryptLen = wc_RsaPublicEncrypt(in, inLen, out, sizeof(out), &key, &rng);
	if (ret < 0) {
		free(tmp);
		return 0;
	}
	size_t hexLen = 0;
	char*hex = base64_encode(out,encryptLen,&hexLen);
	char*str = (char*)malloc(hexLen+1);
	memcpy(str,hex,hexLen);
	str[hexLen] = 0;
	free(hex);
	return str;
}

char*getStringFromEncryptedHex(char*hex)
{
	byte*   tmp;
	RsaKey key1;
	word32 idx = 0;
	int    ret;
	byte   plain[256];

	int binLen;
	byte* encryptedBin = base64_decode(hex, strlen(hex), &binLen);;
	ret = wc_InitRsaKey(&key1, 0);
	if (ret != 0) {
		return 0;
	}
	int len;
	tmp = base64_decode(localPrivateKeyBase64, strlen(localPrivateKeyBase64), &len);
	idx = 0;
	ret = wc_RsaPrivateKeyDecode(tmp, &idx, &key1, len);

	ret = wc_RsaPrivateDecrypt(encryptedBin, binLen, plain, sizeof(plain), &key1);
	if (ret < 0) {
		free(tmp);
		return 0;
	}
	//plain[ret] = 0;
	free(tmp);
	char*str = (char*)malloc(ret+1);
	memcpy(str,plain,ret);
	str[ret] = 0;
	return (char*)str;
}

int rsa_test_password()
{
	char*hex = getEncryptedPasswordHex("11111111");
	char*password = getStringFromEncryptedHex(hex);
	return 0;
}
struct Arc4*InitRC4(unsigned char*keys,int keylen){
	Arc4*enc = malloc(sizeof(Arc4));
	wc_Arc4SetKey(enc, keys, keylen);
	return enc;
}

unsigned char*RC4Encrypt(struct Arc4*key,unsigned char*input,int inLen)
{
//	Arc4 key;// = malloc(sizeof(Arc4));
//	wc_Arc4SetKey(&key, password, strlen(password));
	unsigned char*cipher = malloc(inLen);
	wc_Arc4Process(key, cipher, input,inLen);
	memcpy(input,cipher,inLen);
	free(cipher);
	return input;
}

#ifdef __cplusplus
    }
#endif
// 
// int rsa_test()
// {
// 	//return testsuite_test(argc, argv);
// 	byte*   tmp;
// //	size_t bytes;
// 	RsaKey key;
// 	RsaKey key1;
// 	RNG    rng;
// 	word32 idx = 0;
// 	int    ret;
// 	byte   in[] = "Everyone gets Friday off.";
// 	word32 inLen = (word32)strlen((char*)in);
// 	byte   out[256];
// 	byte   plain[256];
// 
// 	arc4_test();
// 
// 	ret = wc_InitRsaKey(&key, 0);
// 	if (ret != 0) {
// 		return -39;
// 	}
// 	int len;
// 	tmp = base64_decode(publicKeyBase64, strlen(publicKeyBase64), &len);
// 	ret = wc_RsaPublicKeyDecode(tmp, &idx, &key, len);
// 
// 	ret = wc_InitRng(&rng);
// 	if (ret != 0) {
// 		free(tmp);
// 		return -42;
// 	}
// 	int encryptLen = wc_RsaPublicEncrypt(in, inLen, out, sizeof(out), &key, &rng);
// 	if (ret < 0) {
// 		free(tmp);
// 		return -43;
// 	}
// 
// 	ret = wc_InitRsaKey(&key1, 0);
// 	if (ret != 0) {
// 		return -39;
// 	}
// 
// 	tmp = base64_decode(privateKeyBase64, strlen(privateKeyBase64), &len);
// 	idx = 0;
// 	ret = wc_RsaPrivateKeyDecode(tmp, &idx, &key1, len);
// 
// 	ret = wc_RsaPrivateDecrypt(out, encryptLen, plain, sizeof(plain), &key1);
// 	if (ret < 0) {
// 		free(tmp);
// 		return -44;
// 	}
// 
// 	return 0;
// }
// 
// 
// int arc4_test(void)
// {
// 	byte cipher[64];
// 	byte plain[64];
// 	memset(cipher,0,sizeof(cipher));
// 	memset(plain, 0, sizeof(plain));
// 	const char*plainStr = "I love this game ~!@#43532";
// 	int len = strlen(plainStr);
// 	//memcpy(plain, plainStr, len);
// 	const char* keys = "mypassword";
// 
// 	//const char* plain
// 	Arc4 enc;
// 	Arc4 dec;
// 	int  keylen = strlen(keys);  /* strlen with key 0x00 not good */
// 
// 	wc_Arc4SetKey(&enc, keys, keylen);
// 	wc_Arc4SetKey(&dec, keys, keylen);
// 
// 	int newLen = 4;
// 	wc_Arc4Process(&enc, cipher, plainStr,newLen);
// 	wc_Arc4Process(&dec, plain, cipher, newLen);
// 
// 	wc_Arc4Process(&enc, cipher+newLen, plainStr+newLen,len-newLen);
// 	wc_Arc4Process(&dec, plain+newLen, cipher+newLen, len-newLen);
// 
// 	return 0;
// }
