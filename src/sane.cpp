/*
 * Copyright (c) 2013, 2017 Kelvin W Sherlock
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met: 
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


#include <sane/sane.h>
#include <sane/floating_point.h>
#include <sane/comp.h>

#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
 
#include <numeric>
#include <algorithm>
#include <stdexcept>

#include "sane_config.h"

#ifndef HAVE_ASPRINTF

#include <cstdarg>

namespace {

	int vasprintf(char **out, const char *fmt, va_list ap) {
		int size;
		char *cp;
		char buffer[256];

		size = vsnprintf(buffer, sizeof(buffer), fmt, ap);
		if (size < 0) return size;

		if (size < sizeof(buffer)) {
			cp = strdup(buffer);
			*out = cp;
			if (!cp) return -1;

			return size;
		}

		cp = (char *)malloc(size + 1);
		*out = cp;

		if (!cp) return -1;

		return vsnprintf(cp, size+1, fmt, ap);
	}

	int asprintf(char **out, const char *fmt, ...) {
		int size;
		va_list ap;
		va_start(ap, fmt);

		size = vasprintf(out, fmt, ap);
		va_end(ap);
		return size;
	}

}


#endif


namespace SANE {

	namespace fp = floating_point;


	using std::abs;
	using std::fpclassify;
	using std::isinf;
	using std::isnan;
	using std::signbit;


	namespace {
		// Nxxxx -> int. 0 if empty.
		unsigned nan_type(const std::string &s) {

			return std::accumulate(s.begin() + 1, s.end(), 0, [](uint32_t akk, char c){
				if (std::isdigit(c)) { akk = (akk << 4) + c - '0'; }
				else if (std::isxdigit(c)) { akk = (akk << 4) + (c | 0x20) - 'a'; }
				return akk;
			});

		}
	}


	template<>
	comp make_nan<comp>(unsigned code) {
		return comp(UINT64_C(0x8000000000000000));
	}

	template<>
	decimal make_nan<decimal>(unsigned code) {
		std::string s = "N";
		code = code & 0xffff;
		if (code) {

			const char *hexstr = "0123456789abcdef";
			// 4-byte hex
			s.push_back(hexstr[(code >> 12) & 0x0f]);
			s.push_back(hexstr[(code >> 8) & 0x0f]);
			s.push_back(hexstr[(code >> 4) & 0x0f]);
			s.push_back(hexstr[(code >> 0) & 0x0f]);

		}
		decimal d{ 0, 0, s};
		return d;
	}

	template<>
	float make_nan<float>(unsigned code) {
		if (!code) code = NANZERO;

		floating_point::info i;
		i.nan = true;
		i.sig = code;
		return (float)i;
	}

	template<>
	double make_nan<double>(unsigned code) {
		if (!code) code = NANZERO;

		floating_point::info i;
		i.nan = true;
		i.sig = code;
		return (double)i;
	}

	template<>
	long double make_nan<long double>(unsigned code) {
		if (!code) code = NANZERO;

		floating_point::info i;
		i.nan = true;
		i.sig = code;
		return (long double)i;
	}




	void dec2str(const decform &df, const decimal &d, std::string &s) {

		s.clear();

		/*
		 * "Negative values for digits are treated as 0 for fixed formatting, but
		 *  give unspecified results in floating format."
		 */

		int digits = df.digits;
		if (digits < 0) digits = 0;

		std::string sig = d.sig;
		if (sig.empty()) sig = "0";

		int exp = d.exp;




		if (d.sgn) s.push_back('-');
		else if (df.style == decform::FLOATDECIMAL) s.push_back(' ');


		// handle INF/NAN early.
		if (sig[0] == 'I') {
			s.append("INF");
			return;
		}
		if (sig[0] == 'N') {
			unsigned type = nan_type(sig);
			std::string tmp;
			tmp.reserve(3);
			if (type > 0 && type < 1000) {
				while (type) { 
					tmp.push_back('0' + (type % 10));
					type /= 10;
				}
				while (tmp.size() < 3) tmp.push_back('0');
				std::reverse(tmp.begin(), tmp.end());
			}
			else tmp = "000";

			s += "NAN(" + tmp + ")";
			return;	
		}


		if (df.style == decform::FLOATDECIMAL) {
			// [-| m[.nnn]e[+|-]dddd

			// 1 leading digit.
			s.push_back(sig[0]);
			if (sig.length() > 1 || digits > 1) {
				s.push_back('.');
				s.append(sig.substr(1));
				int fudge = sig.length() - 1;
				exp += fudge;
				digits -= fudge;
			}
			if (digits > 80) { s = "?"; return; }
			while (digits > 1) { s.push_back('0'); --digits; }

			s.push_back('e');
			if (exp < 0) {
				// to_string() will include the -
				s.append(std::to_string(exp));
			} else {
				s.push_back('+');
				s.append(std::to_string(exp));
			}

			// if > 80 in length, return '?'
			if (s.length() > 80) s = "?";

			return;
		}
		else
		{
			// [-] mmmm [. nnn]

			std::string mm;
			std::string nn;

			//
			if (exp >= 0) {
				// 0, 5 == 5
				// 1, 12 = 120

				mm = std::move(sig);
				mm.append(exp, '0');
				sig.clear();
			}
			else {
				// -1, 12 = 1.2
				// -2, 12 = 0.12
				// -3  12 = 0.012

				int m = sig.length() + exp;
				if (m > 0) {
					mm = sig.substr(0, m);
					sig.erase(0, m);
					exp = 0;
				}
				else {
					mm = "0";
				}
			}

			s.append(mm);

			if (digits > 0) {
				s.push_back('.');

				// if negative exp, need to put in leading 0s. 
				// for a pathological case like -32768, this would
				// be silly.

				// 500 e-3 == .5
				// 5 e-1 = .5
				// 5 e-2 = .05

				if (exp < 0) {
					long tmp = -(sig.length() + exp);
					if (tmp > 0)
						nn.append(tmp, '0');
				}
				nn.append(sig);
				nn.resize(digits, '0'); // todo -- should round...
				s.append(nn);
			}

			// if > 80 in length, return '?'
			if (s.length() > 80) s = "?";
		}
	}


	long double dec2x(const decimal &d) {
		// todo -- if sig is empty, NAN or 0?

		if (d.sig.empty() || d.sig[0] == '0') {
			return d.sgn ? -0.0 : 0.0;
		}
		if (d.sig[0] == 'I') {
			return d.sgn? -INFINITY : INFINITY;
		}
		if (d.sig[0] == 'N') {
			// todo -- NaN type?
			long double tmp = make_nan<long double>(nan_type(d.sig));
			return d.sgn ? -tmp : tmp;
		}


		std::string str = d.sig + "e" + std::to_string(d.exp);
		const char *cstr = str.c_str();
		char *end;
		long double tmp = std::strtold(cstr, &end);
		if (end-cstr != str.length()) {
			tmp = make_nan<long double>(NANASCBIN);
		}

		if (d.sgn) tmp = -tmp;
		return tmp;
	}

	static bool all_zero(const std::string &s) {
		return std::all_of(s.begin(), s.end(), [](char c){ return c == '0'; });
	}


	static void format_f(long double x, int precision, std::string &mm, std::string &nn)
	{
		char *buffer = nullptr;

		mm.clear();
		nn.clear();

		if (precision < 0) precision = 0;
		int length = asprintf(&buffer, "%.*Lf", precision, x);
		std::string tmp(buffer, length);
		free(buffer);

		auto dot = tmp.find('.');
		if (dot == tmp.npos)
		{
			mm = std::move(tmp);
		}
		else
		{
			mm = tmp.substr(0, dot);
			nn = tmp.substr(dot + 1);
		}

		// skip mm if it's 0
		if (mm.length() == 1 && mm.front() == '0') mm.clear();

		// skip nn if it's 0000...
		if (std::all_of(nn.begin(), nn.end(), [](char c){ return c == '0'; }))
			nn.clear();
	}


	static void format_e(long double x, int precision, std::string &mm, std::string &nn, int16_t &exp)
	{
		char *buffer = nullptr;

		exp = 0;
		mm.clear();
		nn.clear();

		if (precision < 0) precision = 0;
		// if (precision > 19) precision = 19;

		int length = asprintf(&buffer, "%.*Le", precision, x);
		// output mm . nn e[+-]exp
		// mm e[+-]exp
		std::string tmp(buffer, length);
		free(buffer);

		auto dot = tmp.find('.');
		auto e = tmp.find('e');

		if (dot == tmp.npos)
		{
			mm = tmp.substr(0, e);
		}
		else
		{
			mm = tmp.substr(0, dot);
			nn = tmp.substr(dot + 1, e - dot - 1);
		}

		char sign = tmp[e+1];
		tmp = tmp.substr(e + 2);
		exp = std::stoi(tmp);
		if (sign == '-') exp = -exp;
	}



	decimal x2dec(long double x, const decform &df) {

		/*
		 * SANE pp 27 - 31
		 *
		 * Floating style (0):
		 * [-| ]m[.nnn]e[+|-]dddd
		 * digits is the number of significant digits
		 *
		 * Fixed style (1):
		 * [-]mmm[.nnn]
		 * digits is the number of digits to the right of the decimal point
		 * (left if negative)
		 */

		decimal d;
		int digits = df.digits;
		// SANE requires at least 18 digits.
		// IIgs sane allows 28 digits
		// MacOS sane allows 20 digits.
		// if (digits < 0) digits = 0;
		if (digits > 32) digits = 32;

		fp::info fpi(x);
		//fprintf(stderr, "%02x %02x %d %016llx\n", fpi.sign, fpi.one, fpi.exp, fpi.sig);


		d.sgn = signbit(x);

		// handle infinity, nan as a special case.
		switch (fpclassify(x))
		{
			case FP_ZERO:
				d.sig = "0";
				return d;

			case FP_NAN: {
				// TODO -- Macintosh returns N + 16-digit hex number.
				// TODO -- Apple IIgs returns N + 6-digit hex number.
				// NAN type encoded in the sig.
					char buffer[20]; // 16 + 2 needed
					// todo -- use 4 hex digits if possible...
					snprintf(buffer, sizeof(buffer), "N%04x", (unsigned)fpi.sig & 0xffff);
					d.sig = buffer;
					return d;
			}

			case FP_INFINITE:
				d.sig = "I";
				return d;				

			default:
				break;

		}

		// normal and subnormal handled here....

		// float decimal: df.digits refers to the total length
		// fixed decimal: df.digits refers to the fractional part only.

		x = abs(x);


		if (df.style == decform::FIXEDDECIMAL) {

			std::string mm;
			std::string nn;

			if (digits < 0) {
				d.sig = "0";
				d.exp = -digits;
			}

			format_f(x, digits, mm, nn);

			d.exp = -digits;

			if (mm.empty() && nn.empty())
			{
				// very large 0.
				d.sig = "0";
				return d;
			}

			if (mm.empty()) {
				// don't include leading 0s.
				auto iter = nn.begin();
				auto end = nn.end();
				while (iter != end && *iter == '0') ++iter;
				d.sig.assign(iter, end);
				return d;
			}

			while(nn.length() < digits) nn.push_back('0');
			d.sig = std::move(mm);
			d.sig.append(nn);
			return d;
		} else {

			// float

			std::string mm;
			std::string nn;
			int16_t exp = 0;

			int dm1 = digits - 1;
			if (dm1 < 0) dm1 = 0;

			format_e(x, dm1, mm, nn, exp);

			d.sig = std::move(mm);
			d.sig.append(nn);
			d.exp = exp - nn.length();
			return d;
		}

	}



	void truncate(decimal &d, int digits) {
		if (digits < 1) digits = 1;
		if (d.sig.length() <= digits) return;

		// special case for N, I
		if (d.sig.front() == 'I' || d.sig.front() == 'N') {
			d.sig.resize(digits);
			return;
		}

		bool ru = d.sig[digits] >= '5';
		d.exp += (d.sig.length() - digits);
		d.sig.resize(digits);

		// round up...
		// TODO -- should round towards even number
		while (!d.sig.empty() && ru) {
			auto &c = d.sig.back();
			++c;
			ru = (c > '9');
			if (ru) {
				d.exp++;
				d.sig.pop_back();
			}
		}

		// 99 -> (1) 00.  exp already bumped.
		if (ru && d.sig.empty()) d.sig = "1";

		// remove trailing 0s.
		while (d.sig.length() > 1 && d.sig.back() == '0') {
			d.sig.pop_back();
			d.exp++;
		}

	}


	int fpclassify(const decimal &d) {
		if (d.sig.empty() || d.sig.front() == '0') return FP_ZERO;
		if (d.sig.front() == 'N') return FP_NAN;
		if (d.sig.front() == 'I') return FP_INFINITE;
		return FP_NORMAL;
	}

	inline int isfinite(const decimal &d) {
		return d.sig.empty() || std::isdigit(d.sig.front());
	}

	inline int isnormal(const decimal &d) {
		if (d.sig.empty() || d.sig.front() == '0') return false;
		return std::isdigit(d.sig.front());
	}

	std::string to_string(const comp &c) {

		if (isnan(c)) return std::string("nan");
		return std::to_string((int64_t)c);
	}


} // namespace
