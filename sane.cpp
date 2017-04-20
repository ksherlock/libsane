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


#include "sane.h"


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

		if (df.style == decform::FLOATDECIMAL) {
			// [-| m[.nnn]e[+|-]dddd

			// - or space.
			if (d.sgn) s.push_back('-');
			else s.push_back(' ');

			if (sig[0] == 'I') {
				s.append("INF");
				return;
			}
			if (sig[0] == 'N') {
				// todo -- include actual nan code.
				s.append("NAN(000)");
				return;	
			}

			// 1 leading digit.
			s.push_back(sig[0]);
			if (sig.length() > 1) {
				s.push_back('.');
				s.append(sig.substr(1));
			}
			s.push_back('e');
			if (exp < 0) {
				// to_string() will include the -
				s.append(std::to_string(exp));
			} else {
				s.push_back('+');
				s.append(std::to_string(exp));
			}
			return;
		}
		else
		{
			// [-] mmmm [. nnn]
			if (d.sgn) s.push_back('-');

			std::string mm;
			std::string nn;

			if (sig[0] == 'I') {
				s.append("INF");
				return;
			}
			if (sig[0] == 'N') {
				// todo -- include actual nan code.
				// check how SANE format it (hex/dec)
				s.append("NAN(000)");
				return;	
			}

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

} // namespace
