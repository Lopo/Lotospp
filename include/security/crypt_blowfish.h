#ifndef LOTOSPP_SECURITY_CRYPT_BLOWFISH_H
#define LOTOSPP_SECURITY_CRYPT_BLOWFISH_H


#include <string>


namespace lotospp {
	namespace security {

class Bcrypt
{
public:
	static std::string crypt(const std::string& password, const std::string algo="");

private:
	/**
	 * @param key password
	 * @param setting salt "$2x$yy$ => x=[axy] (pref y), yy=<04-31>
	 * @param output
	 * @param size sizeof(output) -> 61
	 */
	static char *crypt_blowfish_rn(const char *key, const char *setting, char *output, int size);
};

	} // namespace security
} // namespace lotospp

#endif
