#ifndef LOTOSPP_SECURITY_CRYPT_BLOWFISH_H
#define LOTOSPP_SECURITY_CRYPT_BLOWFISH_H

#include <string>
#include <openssl/blowfish.h>
#include <sys/types.h>


namespace LotosPP::Security {

// Number of Blowfish rounds, this is also hardcoded into a few places
typedef BF_LONG BF_key[BF_ROUNDS+2];

class Blowfish
{
public:
	static std::string crypt(const std::string& password, const std::string& algo="");
// @internal
	static int BF_decode(BF_LONG* dst, const char* src, int size);
	static void BF_encode(char* dst, const BF_LONG* src, int size);
	static void BF_swap(BF_LONG* x, int count);
	static void BF_set_key(const char* key, BF_key expanded, BF_key initial, unsigned char flags);
	static char* BF_crypt(const char* key, const char* setting, char* output, int size, BF_LONG min);
	static int _crypt_output_magic(const char* setting, char* output, int size);

private:
	/**
	 * @param key password
	 * @param setting salt "$2x$yy$ => x=[axy] (pref y), yy=<04-31>
	 * @param output
	 * @param size sizeof(output) -> 61
	 */
	static char* crypt_blowfish_rn(const char* key, const char* setting, char* output, int size);
};

	}

#endif
