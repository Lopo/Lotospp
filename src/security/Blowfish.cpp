#include "security/Blowfish.h"

#include <string.h>
#include <errno.h>

#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include "external/bf_pi.h"


namespace lotospp {
	namespace security {

typedef signed int BF_word_signed;

std::string Blowfish::crypt(const std::string& password, const std::string& algo/*=""*/)
{
	std::string setting;
	char out[61];
	if (algo=="") {
		setting="$2y$10$";
		std::string chars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890./");
		boost::random::random_device rng;
		boost::random::uniform_int_distribution<> index_dist(0, chars.size()-1);
		for (int i=0; i<22; ++i) {
			setting+=chars[index_dist(rng)];
			}
		}
	else {
		setting=algo;
		}
	crypt_blowfish_rn(password.c_str(), setting.c_str(), out, 61);
	return std::string(out);
}

#ifndef __set_errno
#	define __set_errno(val) errno=(val)
#endif

#ifdef __i386__
#	define BF_SCALE 1
#elif defined(__x86_64__) || defined(__alpha__) || defined(__hppa__)
#	define BF_SCALE 1
#else
#	define BF_SCALE 0
#endif

/**
 * Magic IV for 64 Blowfish encryptions that we do at the end.
 * The string is "OrpheanBeholderScryDoubt" on big-endian.
 */
static BF_LONG BF_magic_w[6]={
	0x4F727068, 0x65616E42, 0x65686F6C,
	0x64657253, 0x63727944, 0x6F756274
	};

static unsigned char BF_itoa64[64+1]="./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
static unsigned char BF_atoi64[0x60]={
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 0, 1,
	54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 64, 64, 64, 64, 64,
	64, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 64, 64, 64, 64, 64,
	64, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
	43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 64, 64, 64, 64, 64
	};

#define BF_safe_atoi64(dst, src) \
{ \
	tmp=(unsigned char)(src); \
	if (tmp=='$') break; /* PHP hack */ \
	if ((unsigned int)(tmp -= 0x20) >= 0x60) return -1; \
	tmp=BF_atoi64[tmp]; \
	if (tmp>63) return -1; \
	(dst)=tmp; \
}

int Blowfish::BF_decode(BF_LONG *dst, const char *src, int size)
{
	unsigned char *dptr=(unsigned char *)dst;
	unsigned char *end=dptr+size;
	const unsigned char *sptr=(const unsigned char *)src;
	unsigned int tmp, c1, c2, c3, c4;

	do {
		BF_safe_atoi64(c1, *sptr++);
		BF_safe_atoi64(c2, *sptr++);
		*dptr++ = (c1 << 2) | ((c2 & 0x30) >> 4);
		if (dptr>=end) {
			break;
			}

		BF_safe_atoi64(c3, *sptr++);
		*dptr++ = ((c2 & 0x0F) << 4) | ((c3 & 0x3C) >> 2);
		if (dptr >= end) {
			break;
			}

		BF_safe_atoi64(c4, *sptr++);
		*dptr++ = ((c3 & 0x03) << 6) | c4;
		} while (dptr<end);

	while (dptr<end) { // PHP hack
		*dptr++ = 0;
		}

	return 0;
}

void Blowfish::BF_encode(char *dst, const BF_LONG *src, int size)
{
	const unsigned char *sptr=(const unsigned char *)src;
	const unsigned char *end=sptr+size;
	unsigned char *dptr=(unsigned char *)dst;
	unsigned int c1, c2;

	do {
		c1= *sptr++;
		*dptr++ = BF_itoa64[c1>>2];
		c1= (c1 & 0x03)<<4;
		if (sptr>=end) {
			*dptr++ = BF_itoa64[c1];
			break;
			}

		c2= *sptr++;
		c1 |= c2>>4;
		*dptr++ = BF_itoa64[c1];
		c1= (c2 & 0x0f)<<2;
		if (sptr>=end) {
			*dptr++ = BF_itoa64[c1];
			break;
			}

		c2= *sptr++;
		c1 |= c2>>6;
		*dptr++ = BF_itoa64[c1];
		*dptr++ = BF_itoa64[c2 & 0x3f];
		} while (sptr<end);
}

void Blowfish::BF_swap(BF_LONG *x, int count)
{
	static int endianness_check=1;
	char *is_little_endian=(char *)&endianness_check;
	BF_LONG tmp;

	if (*is_little_endian) {
		do {
			tmp=*x;
			tmp= (tmp << 16) | (tmp >> 16);
			*x++ = ((tmp & 0x00FF00FF) << 8) | ((tmp >> 8) & 0x00FF00FF);
			} while (--count);
		}
}

#if BF_SCALE
// Architectures which can shift addresses left by 2 bits with no extra cost
#	define BF_ROUND(L, R, N) \
		tmp1= L & 0xFF; \
		tmp2= L >> 8; \
		tmp2 &= 0xFF; \
		tmp3= L >> 16; \
		tmp3 &= 0xFF; \
		tmp4= L >> 24; \
		tmp1=data.ctx.S[0x300+tmp1]; \
		tmp2=data.ctx.S[0x200+tmp2]; \
		tmp3=data.ctx.S[0x100+tmp3]; \
		tmp3+=data.ctx.S[tmp4]; \
		tmp3^=tmp2; \
		R^=data.ctx.P[N+1]; \
		tmp3+=tmp1; \
		R^=tmp3;
#else
// Architectures with no complicated addressing modes supported
#	define BF_INDEX(S, i) (*((BF_LONG *)(((unsigned char *)S)+(i))))
#	define BF_ROUND(L, R, N) \
		tmp1= L & 0xFF; \
		tmp1<<=2; \
		tmp2= L>>6; \
		tmp2 &= 0x3FC; \
		tmp3= L>>14; \
		tmp3 &= 0x3FC; \
		tmp4= L>>22; \
		tmp4 &= 0x3FC; \
		tmp1=BF_INDEX(data.ctx.S[0x300], tmp1); \
		tmp2=BF_INDEX(data.ctx.S[0x200], tmp2); \
		tmp3=BF_INDEX(data.ctx.S[0x100], tmp3); \
		tmp3+=BF_INDEX(data.ctx.S[0], tmp4); \
		tmp3^=tmp2; \
		R^=data.ctx.P[N+1]; \
		tmp3+=tmp1; \
		R^=tmp3;
#endif

/*
 * Encrypt one block, BF_ROUNDS is hardcoded here.
 */
#define BF_ENCRYPT_1B \
	L ^= data.ctx.P[0]; \
	BF_ROUND(L, R, 0); \
	BF_ROUND(R, L, 1); \
	BF_ROUND(L, R, 2); \
	BF_ROUND(R, L, 3); \
	BF_ROUND(L, R, 4); \
	BF_ROUND(R, L, 5); \
	BF_ROUND(L, R, 6); \
	BF_ROUND(R, L, 7); \
	BF_ROUND(L, R, 8); \
	BF_ROUND(R, L, 9); \
	BF_ROUND(L, R, 10); \
	BF_ROUND(R, L, 11); \
	BF_ROUND(L, R, 12); \
	BF_ROUND(R, L, 13); \
	BF_ROUND(L, R, 14); \
	BF_ROUND(R, L, 15); \
	tmp4=R; \
	R=L; \
	L= tmp4 ^ data.ctx.P[BF_ROUNDS+1];

#define BF_body() \
	L= R= 0; \
	ptr=data.ctx.P; \
	do { \
		ptr+=2; \
		BF_ENCRYPT_1B; \
		*(ptr-2)=L; \
		*(ptr-1)=R; \
		} while (ptr < &data.ctx.P[BF_ROUNDS+2]); \
\
	ptr=data.ctx.S; \
	do { \
		ptr+=2; \
		BF_ENCRYPT_1B; \
		*(ptr-2)=L; \
		*(ptr-1)=R; \
		} while (ptr < &data.ctx.S[0x300+0xFF]);

void Blowfish::BF_set_key(const char *key, BF_key expanded, BF_key initial, unsigned char flags)
{
	const char *ptr=key;
	unsigned int bug, i, j;
	BF_LONG safety, sign, diff, tmp[2];

/*
 * There was a sign extension bug in older revisions of this function. While
 * we would have liked to simply fix the bug and move on, we have to provide
 * a backwards compatibility feature (essentially the bug) for some systems and
 * a safety measure for some others. The latter is needed because for certain
 * multiple inputs to the buggy algorithm there exist easily found inputs to
 * the correct algorithm that produce the same hash. Thus, we optionally
 * deviate from the correct algorithm just enough to avoid such collisions.
 * While the bug itself affected the majority of passwords containing
 * characters with the 8th bit set (although only a percentage of those in a
 * collision-producing way), the anti-collision safety measure affects
 * only a subset of passwords containing the '\xff' character (not even all of
 * those passwords, just some of them). This character is not found in valid
 * UTF-8 sequences and is rarely used in popular 8-bit character encodings.
 * Thus, the safety measure is unlikely to cause much annoyance, and is a
 * reasonable tradeoff to use when authenticating against existing hashes that
 * are not reliably known to have been computed with the correct algorithm.
 *
 * We use an approach that tries to minimize side-channel leaks of password
 * information - that is, we mostly use fixed-cost bitwise operations instead
 * of branches or table lookups. (One conditional branch based on password
 * length remains. It is not part of the bug aftermath, though, and is
 * difficult and possibly unreasonable to avoid given the use of C strings by
 * the caller, which results in similar timing leaks anyway.)
 *
 * For actual implementation, we set an array index in the variable "bug"
 * (0 means no bug, 1 means sign extension bug emulation) and a flag in the
 * variable "safety" (bit 16 is set when the safety measure is requested).
 * Valid combinations of settings are:
 *
 * Prefix "$2a$": bug = 0, safety = 0x10000
 * Prefix "$2b$": bug = 0, safety = 0
 * Prefix "$2x$": bug = 1, safety = 0
 * Prefix "$2y$": bug = 0, safety = 0
 */
	bug= (unsigned int)flags & 1;
	safety= ((BF_LONG)flags & 2)<<15;
	sign= diff= 0;

	for (i=0; i<BF_ROUNDS+2; i++) {
		tmp[0]= tmp[1]= 0;
		for (j=0; j<4; j++) {
			tmp[0]<<=8;
			tmp[0] |= (unsigned char)*ptr; /* correct */
			tmp[1]<<=8;
			tmp[1] |= (BF_word_signed)(signed char)*ptr; /* bug */
/*
 * Sign extension in the first char has no effect - nothing to overwrite yet,
 * and those extra 24 bits will be fully shifted out of the 32-bit word. For
 * chars 2, 3, 4 in each four-char block, we set bit 7 of "sign" if sign
 * extension in tmp[1] occurs. Once this flag is set, it remains set.
 */
			if (j) {
				sign |= tmp[1] & 0x80;
				}
			if (!*ptr) {
				ptr=key;
				}
			else {
				ptr++;
				}
			}
		diff |= tmp[0] ^ tmp[1]; /* Non-zero on any differences */

		expanded[i]=tmp[bug];
		initial[i]= bf_init.P[i] ^ tmp[bug];
		}

/*
 * At this point, "diff" is zero iff the correct and buggy algorithms produced
 * exactly the same result. If so and if "sign" is non-zero, which indicates
 * that there was a non-benign sign extension, this means that we have a
 * collision between the correctly computed hash for this password and a set of
 * passwords that could be supplied to the buggy algorithm. Our safety measure
 * is meant to protect from such many-buggy to one-correct collisions, by
 * deviating from the correct algorithm in such cases. Let's check for this.
 */
	diff |= diff>>16; /* still zero iff exact match */
	diff &= 0xffff; /* ditto */
	diff += 0xffff; /* bit 16 set iff "diff" was non-zero (on non-match) */
	sign<<=9; /* move the non-benign sign extension flag to bit 16 */
	sign &= ~diff & safety; /* action needed? */

/*
 * If we have determined that we need to deviate from the correct algorithm,
 * flip bit 16 in initial expanded key. (The choice of 16 is arbitrary, but
 * let's stick to it now. It came out of the approach we used above, and it's
 * not any worse than any other choice we could make.)
 *
 * It is crucial that we don't do the same to the expanded key used in the main
 * Eksblowfish loop. By doing it to only one of these two, we deviate from a
 * state that could be directly specified by a password to the buggy algorithm
 * (and to the fully correct one as well, but that's a side-effect).
 */
	initial[0]^=sign;
}

static const unsigned char flags_by_subtype[26]={
	2, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 0
	};

char *Blowfish::BF_crypt(const char *key, const char *setting, char *output, int size, BF_LONG min)
{
	struct {
		BF_KEY ctx;
		BF_key expanded_key;
		union {
			BF_LONG salt[4];
			BF_LONG output[6];
			} binary;
		} data;
	BF_LONG L, R;
	BF_LONG tmp1, tmp2, tmp3, tmp4;
	BF_LONG *ptr;
	BF_LONG count;
	int i;

	if (size < 7+22+31+1) {
		__set_errno(ERANGE);
		return NULL;
		}

	if (setting[0]!='$'
		|| setting[1]!='2'
		|| setting[2]<'a' || setting[2]>'z'
		|| !flags_by_subtype[(unsigned int)(unsigned char)setting[2]-'a']
		|| setting[3]!='$'
		|| setting[4]<'0' || setting[4]>'3'
		|| setting[5]<'0' || setting[5]>'9'
		|| (setting[4]=='3' && setting[5]>'1')
		|| setting[6]!='$'
		) {
		__set_errno(EINVAL);
		return NULL;
		}

	count= (BF_LONG)1 << ((setting[4]-'0')*10+(setting[5]-'0'));
	if (count<min || Blowfish::BF_decode(data.binary.salt, &setting[7], 16)) {
		__set_errno(EINVAL);
		return NULL;
		}
	Blowfish::BF_swap(data.binary.salt, 4);

	Blowfish::BF_set_key(key, data.expanded_key, data.ctx.P, flags_by_subtype[(unsigned int)(unsigned char)setting[2]-'a']);

	memcpy(data.ctx.S, bf_init.S, sizeof(data.ctx.S));

	L= R= 0;
	for (i=0; i<BF_ROUNDS+2; i+=2) {
		L ^= data.binary.salt[i&2];
		R ^= data.binary.salt[(i&2)+1];
		BF_ENCRYPT_1B;
		data.ctx.P[i]=L;
		data.ctx.P[i+1]=R;
		}

	ptr=data.ctx.S;
	do {
		ptr+=4;
		L ^= data.binary.salt[(BF_ROUNDS+2) & 3];
		R ^= data.binary.salt[(BF_ROUNDS+3) & 3];
		BF_ENCRYPT_1B;
		*(ptr-4)=L;
		*(ptr-3)=R;

		L ^= data.binary.salt[(BF_ROUNDS+4) & 3];
		R ^= data.binary.salt[(BF_ROUNDS+5) & 3];
		BF_ENCRYPT_1B;
		*(ptr-2)=L;
		*(ptr-1)=R;
		} while (ptr< &data.ctx.S[0x300+0xFF]);

	do {
		int done;

		for (i=0; i<BF_ROUNDS+2; i+=2) {
			data.ctx.P[i] ^= data.expanded_key[i];
			data.ctx.P[i+1] ^= data.expanded_key[i+1];
			}

		done=0;
		do {
			BF_body();
			if (done) {
				break;
				}
			done=1;

			tmp1=data.binary.salt[0];
			tmp2=data.binary.salt[1];
			tmp3=data.binary.salt[2];
			tmp4=data.binary.salt[3];
			for (i=0; i<BF_ROUNDS; i+=4) {
				data.ctx.P[i] ^= tmp1;
				data.ctx.P[i+1] ^= tmp2;
				data.ctx.P[i+2] ^= tmp3;
				data.ctx.P[i+3] ^= tmp4;
				}
			data.ctx.P[16] ^= tmp1;
			data.ctx.P[17] ^= tmp2;
			} while (1);
		} while (--count);

	for (i=0; i<6; i+=2) {
		L=BF_magic_w[i];
		R=BF_magic_w[i+1];

		count=64;
		do {
			BF_ENCRYPT_1B;
			} while (--count);

		data.binary.output[i]=L;
		data.binary.output[i+1]=R;
		}

	memcpy(output, setting, 7+22-1);
	output[7+22-1]=BF_itoa64[(int)BF_atoi64[(int)setting[7+22-1]-0x20] & 0x30];

	// This has to be bug-compatible with the original implementation, so only encode 23 of the 24 bytes. :-)
	Blowfish::BF_swap(data.binary.output, 6);
	Blowfish::BF_encode(&output[7+22], data.binary.output, 23);
	output[7+22+31]='\0';

	return output;
}

int Blowfish::_crypt_output_magic(const char *setting, char *output, int size)
{
	if (size<3) {
		return -1;
		}

	output[0]='*';
	output[1]='0';
	output[2]='\0';

	if (setting[0]=='*' && setting[1]=='0') {
		output[1]='1';
		}

	return 0;
}

/**
 * Please preserve the runtime self-test. It serves two purposes at once:
 *
 * 1. We really can't afford the risk of producing incompatible hashes e.g.
 * when there's something like gcc bug 26587 again, whereas an application or
 * library integrating this code might not also integrate our external tests or
 * it might not run them after every build. Even if it does, the miscompile
 * might only occur on the production build, but not on a testing build (such
 * as because of different optimization settings). It is painful to recover
 * from incorrectly-computed hashes - merely fixing whatever broke is not
 * enough. Thus, a proactive measure like this self-test is needed.
 *
 * 2. We don't want to leave sensitive data from our actual password hash
 * computation on the stack or in registers. Previous revisions of the code
 * would do explicit cleanups, but simply running the self-test after hash
 * computation is more reliable.
 *
 * The performance cost of this quick self-test is around 0.6% at the "$2a$08"
 * setting.
 */
char* Blowfish::crypt_blowfish_rn(const char *key, const char *setting, char *output, int size)
{
	const char *test_key="8b \xd0\xc1\xd2\xcf\xcc\xd8";
	const char *test_setting="$2a$00$abcdefghijklmnopqrstuu";
	static const char* const test_hashes[2]={
		"i1D709vfamulimlGcq0qq3UvuUasvEa\0\x55", /* 'a', 'b', 'y' */
		"VUrPmXD6q/nVSSp7pNDhCR9071IfIRe\0\x55"}; /* 'x' */
	const char* test_hash=test_hashes[0];
	char* retval;
	const char* p;
	int save_errno, ok;
	struct {
		char s[7+22+1];
		char o[7+22+31+1+1+1];
		} buf;

	// Hash the supplied password
	Blowfish::_crypt_output_magic(setting, output, size);
	retval=Blowfish::BF_crypt(key, setting, output, size, 16);
	save_errno=errno;

	/*
	 * Do a quick self-test. It is important that we make both calls to BF_crypt() from the same scope such that they
	 * likely use the same stack locations, which makes the second call overwrite the first call's sensitive data
	 * on the stack and makes it more likely that any alignment related issues would be detected by the self-test.
	 */
	memcpy(buf.s, test_setting, sizeof(buf.s));
	if (retval) {
		unsigned int flags=flags_by_subtype[(unsigned int)(unsigned char)setting[2]-'a'];
		test_hash=test_hashes[flags & 1];
		buf.s[2]=setting[2];
		}
	memset(buf.o, 0x55, sizeof(buf.o));
	buf.o[sizeof(buf.o)-1]=0;
	p=Blowfish::BF_crypt(test_key, buf.s, buf.o, sizeof(buf.o)-(1+1), 1);

	ok= (p==buf.o
		&& !memcmp(p, buf.s, 7+22)
		&& !memcmp(p+(7+22), test_hash, 31+1+1+1)
		);

	{
		const char *k="\xff\xa3" "34" "\xff\xff\xff\xa3" "345";
		BF_key ae, ai, ye, yi;
		Blowfish::BF_set_key(k, ae, ai, 2); /* $2a$ */
		Blowfish::BF_set_key(k, ye, yi, 4); /* $2y$ */
		ai[0] ^= 0x10000; /* undo the safety (for comparison) */
		ok= ok
			&& ai[0]==0xdb9c59bc
			&& ye[17]==0x33343500
			&& !memcmp(ae, ye, sizeof(ae))
			&& !memcmp(ai, yi, sizeof(ai));
	}

	__set_errno(save_errno);
	if (ok) {
		return retval;
		}

	// Should not happen
	Blowfish::_crypt_output_magic(setting, output, size);
	__set_errno(EINVAL); // pretend we don't support this hash type
	return NULL;
}

	} // namespace security
} // namespace lotospp
