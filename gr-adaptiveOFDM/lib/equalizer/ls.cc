/*
 * Copyright (C) 2016 Bastian Bloessl <bloessl@ccs-labs.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ls.h"
#include <cstring>
#include <iostream>

using namespace gr::adaptiveOFDM::equalizer;

void ls::equalize(gr_complex *in, int n, gr_complex *symbols, uint8_t *bits, boost::shared_ptr<gr::digital::constellation> mod) {

	if(n == 0) {
		std::memcpy(d_H, in, 64 * sizeof(gr_complex));

	} else if(n == 1) {
		base::stimate_channel_state(in);
	} else {

		int c = 0;
		for(int i = 0; i < 64; i++) {
			if(base::isPilot(i)){
				continue;
			} else {
				symbols[c] = in[i] / d_H[i];
				bits[c] = mod->decision_maker(&symbols[c]);
				c++;
			}
		}
	}
}
