#ifndef __sane_h__
#define __sane_h__

#include <cstdint>
#include <string>

namespace SANE
{
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
}

#endif
