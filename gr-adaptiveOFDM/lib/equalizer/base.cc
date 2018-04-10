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

#include "base.h"
#include <cstring>
#include <iostream>

using namespace gr::adaptiveOFDM::equalizer;

const gr_complex base::LONG[] = {
	 0,  0,  0,  0,  0,  0,  1,  1, -1, -1,
	 1,  1, -1,  1, -1,  1,  1,  1,  1,  1,
	 1, -1, -1,  1,  1, -1,  1, -1,  1,  1,
	 1,  1,  0,  1, -1, -1,  1,  1, -1,  1,
	-1,  1, -1, -1, -1, -1, -1,  1,  1, -1,
	-1,  1, -1,  1, -1,  1,  1,  1,  1,  0,
	 0,  0,  0,  0
};

const gr_complex base::POLARITY[127] = {
	 1, 1, 1, 1,-1,-1,-1, 1,-1,-1,-1,-1, 1, 1,-1, 1,
	-1,-1, 1, 1,-1, 1, 1,-1, 1, 1, 1, 1, 1, 1,-1, 1,
	 1, 1,-1, 1, 1,-1,-1, 1, 1, 1,-1, 1,-1,-1,-1, 1,
	-1, 1,-1,-1, 1,-1,-1, 1, 1, 1, 1, 1,-1,-1, 1, 1,
	-1,-1, 1,-1, 1,-1, 1, 1,-1,-1,-1, 1, 1,-1,-1,-1,
	-1, 1,-1,-1, 1,-1, 1, 1, 1, 1,-1, 1,-1, 1,-1, 1,
	-1,-1,-1,-1,-1, 1,-1, 1, 1,-1, 1,-1, 1, 1, 1,-1,
	-1, 1,-1,-1,-1, 1, 1, 1,-1,-1,-1,-1,-1,-1,-1 };

bool
base::isPilot(int carrier){
	if( (carrier== 11) || (carrier == 25) || (carrier == 32) || (carrier == 39) || (carrier == 53) || (carrier < 6) || (carrier > 58)) {
		return true;
	}
	return false;
}

void
base::estimate_channel_state(gr_complex *in){
	double signal = 0;
	double noise = 0;
	double snr_inst = 0;
	double noise_inst = 0;
	double signal_inst = 0;
	d_snr_min = 10000;
	d_snr_max = -10000;

	for(int i = 0; i < 64; i++) {
		if (i == 32 || i < 6 || i > 58) {
			continue;
		}
		noise_inst = std::pow(std::abs(d_H[i] - in[i]), 2);
		signal_inst = std::pow(std::abs(d_H[i] + in[i]), 2);
		noise += noise_inst;
		signal += signal_inst;
		snr_inst = signal_inst / noise_inst;
		if(d_snr_min >= snr_inst) {
			d_snr_min = snr_inst;
		}
		if(d_snr_max<= snr_inst){
			d_snr_max = snr_inst;
		}
	}

	d_snr = 10 * std::log10(signal / noise / 2);

	for(int i = 0; i < 64; i++) {
		d_H[i] += in[i];
		d_H[i] /= LONG[i] * gr_complex(2, 0);
	}
}

double
base::get_snr() {
	return d_snr;
}

double
base::get_snr_min(){
	return (10 * std::log10(d_snr_min / 2));
}

double
base::get_snr_max(){
	return(10 * std::log10(d_snr_max / 2));
}
