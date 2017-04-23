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
#include <sane/fpinfo.h>

#include <cctype>
#include <cmath>
#include <stdlib.h>
 
#include <numeric>
#include <algorithm>
#include <stdexcept>

using std::abs;
using std::fpclassify;
using std::isinf;
using std::isnan;
using std::signbit;


namespace SANE {

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
			uint32_t type;

			type = std::accumulate(sig.begin() + 1, sig.end(), 0, [](uint32_t akk, char c){
				if (isdigit(c)) { akk = (akk << 4) + c - '0'; }
				else if (isxdigit(c)) { akk = (akk << 4) + (c | 0x20) - 'a'; }
				return akk;
			});
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

				if (exp < 0)
					nn.append(-exp, '0');
				nn.append(sig);
				nn.resize(digits, '0'); // todo -- should round...
				s.append(nn);
			}

			// if > 80 in length, return '?'
			if (s.length() > 80) s = "?";
		}
	}


	long double dec2x(const decimal &d) {
		if (d.sig.empty() || d.sig[0] == '0') {
			return d.sgn ? -0.0 : 0.0;
		}
		if (d.sig[0] == 'I') {
			return d.sgn? -INFINITY : INFINITY;
		}
		if (d.sig[0] == 'N') {
			// todo -- NaN type?
			return d.sgn ? -NAN : NAN;
		}


		long double tmp = 0;
		try {
			size_t pos;
			tmp = std::stold(d.sig, &pos);
			int exp = d.exp;

			while (exp > 0) {
				tmp = tmp * 10.0;
				exp--;
			}

			while (exp < 0) {
				tmp = tmp / 10.0;
				exp++;
			}

		} catch(std::out_of_range &e) {
			tmp = INFINITY;
			if (d.exp < 0) tmp = 0;
		} catch(std::invalid_argument &e) {
			tmp = NAN;
		}

		if (d.sgn) tmp = -tmp;
		return tmp;
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
		if (precision > 19) precision = 19;

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
		 * SANE pp 30, 31
		 *
		 * Floating style:
		 * [-| ]m[.nnn]e[+|-]dddd
		 *
		 * Fixed style:
		 * [-]mmm[.nnn]
		 */

		decimal d;
		int digits = df.digits;
		if (digits < 0) digits = 0;
		if (digits > 19) digits = 19;

		fpinfo fpi(x);
		//fprintf(stderr, "%02x %02x %d %016llx\n", fpi.sign, fpi.one, fpi.exp, fpi.sig);


		d.sgn = signbit(x);

		// handle infinity, nan as a special case.
		switch (fpclassify(x))
		{
			case FP_ZERO:
				d.sig = "0";
				return d;

			case FP_NAN: {
				// NAN type encoded in the sig.
					char buffer[20]; // 16 + 2 needed
					// todo -- use 4 hex digits if possible...
					snprintf(buffer, 20, "N%016llx", fpi.sig);
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


		if (x < 1.0 && df.style == decform::FLOATDECIMAL)
		{
			std::string mm;
			std::string nn;

			format_e(x, digits - 1, mm, nn, d.exp);

			d.sig = mm + nn;

			// better be < 0...
			if (d.exp < 0)
				d.exp -= nn.length();

			return d;
		}
		else // x > 1
		{

			std::string mm;
			std::string nn;

			format_f(x, digits, mm, nn);

			if (mm.empty() && nn.empty())
			{
				// very large 0.
				d.sig = "0";
				return d;
			}

			// if nn is empty (or 0s), this is a large number,
			// and we don't have to worry about the fraction.
			if (nn.empty())
			{
				d.exp = 0;

				if (df.style == decform::FIXEDDECIMAL) digits = 19; // todo - SIGDIGITS-1?

				// limit the length.
				if (mm.length() > digits)
				{
					d.exp = mm.length() - digits;
					mm.resize(digits);
				}
				d.sig = std::move(mm);

			}
			else
			{
				if (df.style == decform::FIXEDDECIMAL)
				{
					// digits is the total size, mm + nn
					// re-format with new precision.
					// this is getting repetitive...

					if (mm.length())
					{
						int precision = digits - mm.length();
						if (precision < 0) precision = 1;	

						format_f(x, precision, mm, nn);					
					}
				}
				// todo -- if mm is empty and nn has leading 0s, 
				// drop the leading 0s and adjust the exponent
				// accordingly.

				d.sig = mm + nn;
				d.exp = -nn.length();

				if (d.sig.length() > 19)
				{
					d.exp += (d.sig.length() - 19);
					d.sig.resize(19);
				}
			}

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




} // namespace
