#ifndef __sane_h__
#define __sane_h__

#include <cstdint>
#include <string>

namespace SANE
{

	/* NaN codes.  See pp41, table 5-1 */
	enum {
		NANSQRT = 1,       /* Invalid square root, such as sqrt(-1)   */
		NANADD = 2,        /* Invalid addition, such as +INF - +INF   */
		NANDIV = 4,        /* Invalid division, such as 0/0           */
		NANMUL = 8,        /* Invalid multiply, such as 0 * INF       */
		NANREM = 9,        /* Invalid rem or mod, such as x REM 0     */
		NANASCBIN = 17,    /* Conversion of invalid ASCII string     */
		NANCOMP = 20,      /* Comp NaN converted to floating         */
		NANZERO = 21,      /* Attempt to create a NaN with zero code */
		NANTRIG = 33,      /* Invalid argument to trig routine       */
		NANINVTRIG = 34,   /* Invalid arg to inverse trig routine    */
		NANLOG = 36,       /* Invalid argument to log routine        */
		NANPOWER = 37,     /* Invalid argument to x^i or x^y routine */
		NANFINAN = 38,     /* Invalid argument to financial function */	
	};

	struct decimal {
		enum {
			SIGDIGLEN = 32,
		};

		int16_t sgn = 0;
		int16_t exp = 0;
		std::string sig;


		decimal(int16_t a, int16_t b, const std::string &c) : sgn(a), exp(b), sig(c)
		{}

		decimal() = default;
		decimal(const decimal &) = default;
		decimal(decimal &&) = default;

	};

	struct decform {
		enum {
			FLOATDECIMAL = 0,
			FIXEDDECIMAL = 1,
		};

		uint16_t style = 0;
		int16_t digits = 0;

		decform(uint16_t a, int16_t b) : style(a), digits(b)
		{}

		decform() = default;
		decform(const decform &) = default;
		decform(decform &&) = default;
	};


	void str2dec(const std::string &s, uint16_t &index, decimal &d, uint16_t &vp);
	void dec2str(const decform &f, const decimal &d, std::string &s);

	long double dec2x(const decimal &d);
	decimal x2dec(long double x, const decform &df);

	void truncate(decimal &d, int digits);
}

#endif
