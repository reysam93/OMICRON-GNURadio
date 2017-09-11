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

using namespace gr::frequencyAdaptiveOFDM::equalizer;

std::vector<double>
base::resource_blocks_snr() {
	return d_resource_block_snr;
}

std::vector<double> 
base::min_rb_snr(){
	return d_min_rb_snr;
}

int
base::rb_index_from_carrier(int n_carrier) {
	if ((n_carrier == 32) || (n_carrier < 6) || (n_carrier > 58)){
		return -1;
	} else if (n_carrier < 19) {
		return 0;
	} else if (n_carrier < 32) {
		return 1;
	} else if (n_carrier < 46) {
		return 2;
	} else if (n_carrier < 59) {
		return 3;
	}
}

void
base::stimate_channel_state(gr_complex *in) {
	std::vector<double> rb_signal = std::vector<double>(4,0);
	std::vector<double> rb_noise = std::vector<double>(4,0);
	double signal, noise, carrier_snr = 0;
	int index;

	d_resource_block_snr = std::vector<double>(4,0);
	d_min_rb_snr = std::vector<double>(4,1000);
	
	for(int i = 0; i < 64; i++) {
		index = rb_index_from_carrier(i);
		if (index == -1){
			continue;
		} else if (index < -1) {
			throw std::runtime_error("wrong index");
		}
			
		noise = std::pow(std::abs(d_H[i] - in[i]), 2);
		signal = std::pow(std::abs(d_H[i] + in[i]), 2);
		carrier_snr = signal / noise / 2;
		if (carrier_snr <= d_min_rb_snr[index]){
			d_min_rb_snr[index] = carrier_snr;
		}
		
		rb_noise[index] += noise;
		rb_signal[index] += signal;
		d_H[i] += in[i];
		d_H[i] /= LONG[i] * gr_complex(2, 0);
	}

	for (int i = 0; i < d_resource_block_snr.size(); i++) {
		d_resource_block_snr[i] = 10 * std::log10(rb_signal[i] / rb_noise[i] / 2);
		d_min_rb_snr[i] = 10 * std::log10(d_min_rb_snr[i]);		
	}
}

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
