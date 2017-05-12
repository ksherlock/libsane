/*
 * Copyright (c) 2013, Kelvin W Sherlock
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
#include <string>
#include <algorithm>

namespace SANE {

/*
 * %{} is a final transition.  fpc points to the next character
 * %!{} is a final transition to an error state. fpc " " " " 
 *
 */
%%{
	
	machine fpstr;
	

	action check { checkpoint = fpc; }

	nantype =
		'('
		digit* ${ nantype = nantype * 10 + fc - '0'; }
		')'
		$!{ nantype = 0; }
		%check
		;

	nan = 'NAN'i 
		>{ nan = true; } 
		%check
		%!check
		nantype?
		;

	infinity = 'INF'i
		>{ nan = true; }
		%{ nan = false; infinity = true; checkpoint = fpc; }
		%!{ nan = false; infinity = true; checkpoint = fpc; };

	exponent =
		[eE]
		[+\-]? ${ if (fc == '-') negative_exp = true; }
		digit+ ${ exp = exp * 10 + fc - '0'; }
		%check
		%!check
		;

	significand =
		(
			(
				digit+  ${
					// don't push leading 0s
					if (fc != '0' || siga.size())
						siga.push_back(fc);
				}
				( '.' digit* ${ sigb.push_back(fc); })?
			)
			| 
			(
				'.' 
				digit+ ${ sigb.push_back(fc); }
			)
		)
		%check
		%!check
	; 

	finite_number = significand  exponent?;

	unsigned_decimal = finite_number | infinity | nan;

	left_decimal = 
		[+\-]? ${ if (fc == '-') negative = true; } 
		unsigned_decimal
	;

	decimal_number = [ \t]* left_decimal;

	main := decimal_number;

}%%


std::string normalize(std::string &a, std::string &b, int &exponent)
{
	int pos;

	// 1 = 1e0 10 = 1e1
	// 12 = 12e0
	// 123 = 123e0

	// 1.1 = 11e-1
	// 0.1 = 1e-1

	// Sane816 doesn't do this but logic below depends on it.
	// remove trailing 0s
	#if 0
	while (b.size() && b.back() == '0')
		b.pop_back();
	#else
	if (std::all_of(b.begin(), b.end(), [](char c){ return c == '0'; }))
		b.clear();

	#endif


	int bits = 0;
	if (a.length()) bits |= 0x01;
	if (b.length()) bits |= 0x02;

	std::string out;
	switch(bits)
	{
	case 0x00:
		break;

	case 0x01:
		// a only.
		// remove trailing 0s and add 1 exp for each.
		#if 0
		while (a.length() && a.back() == '0') {
			a.pop_back();
			exponent++;
		}
		#endif
		out = a;
		break;

	case 0x02:
		// b only.
		//.001 = e1-3
		exponent -= b.length();
		pos = b.find_first_not_of('0');
		out = b.substr(pos);
		break;

	case 0x03:
		// a and b
		// 10.01 = 1001e-2
		exponent -= b.length();
		out = a;
		out += b;
		break;

	}
	if (out.empty()) out = "0";
	return out;
}




void str2dec(const std::string &s, uint16_t &index, decimal &d, uint16_t &vp)
{
%%write data;

		bool infinity = false;
		bool nan = false;
		int nantype = 0;
		bool negative = false;
		bool negative_exp = false;
		int exp = 0;
		std::string siga, sigb;

		if (index >= s.size()) return;

		/*
		char *p = s.c_str();
		char *pe = p + s.size();
		char *eof = pe;
		char *checkpoint = p;
		*/
		auto p = s.begin();
		auto checkpoint = s.begin();

		auto pe = s.end();
		auto eof = s.end();

		int cs;

		p += index;

	%%write init;

	%%write exec;

		d.sgn = negative ? 1 : 0;

		if (infinity)
		{
			d.sig = "I";
		}
		else if (nan)
		{
			d.sig = "N";
			if (nantype)
			{
				const char *hexstr = "0123456789abcdef";
				// 4-byte hex
				d.sig.push_back(hexstr[(nantype >> 12) & 0x0f]);
				d.sig.push_back(hexstr[(nantype >> 8) & 0x0f]);
				d.sig.push_back(hexstr[(nantype >> 4) & 0x0f]);
				d.sig.push_back(hexstr[(nantype >> 0) & 0x0f]);
			}
		}
		else
		{
			if (negative_exp) exp = -exp;
			d.sig = normalize(siga, sigb, exp);
			d.exp = exp;
		}

		vp = cs != fpstr_error;
		index = checkpoint - s.begin();

		return;
}

} // namespace

