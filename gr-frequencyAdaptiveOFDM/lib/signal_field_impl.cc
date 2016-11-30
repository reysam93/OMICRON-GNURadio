/* -*- c++ -*- */
/* 
 * Copyright 2016 	Samuel Rey <samuel.rey.escudero@gmail.com>
 *                  Bastian Bloessl <bloessl@ccs-labs.org>
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "signal_field_impl.h"
#include "utils.h"
#include <gnuradio/digital/lfsr.h>

using namespace gr::frequencyAdaptiveOFDM;

signal_field::sptr
signal_field::make() {
	return signal_field::sptr(new signal_field_impl());
}

signal_field::signal_field() : packet_header_default(48, "packet_len") {}

signal_field_impl::signal_field_impl() : packet_header_default(48, "packet_len") {}

signal_field_impl::~signal_field_impl() {}

int signal_field_impl::get_bit(int b, int i) {
	return (b & (1 << i) ? 1 : 0);
}

void signal_field_impl::generate_signal_field(char *out, frame_param &frame, ofdm_param &ofdm) {

	//data bits of the signal header
	char *signal_header = (char *) malloc(sizeof(char) * 28);

	//signal header after...
	//convolutional encoding
	char *encoded_signal_header = (char *) malloc(sizeof(char) * 48);
	//interleaving
	char *interleaved_signal_header = (char *) malloc(sizeof(char) * 48);

	int length = frame.psdu_size;


	// NOW ASSUMING ALWAIS BPSK AMD IGNORING ENCODING
	// TODO: USE ENCODING
	// first 8 bits represent the modulation of the 4 resource blocks
	signal_header[ 0] = 0;//get_bit(ofdm.rate_field, 3);
	signal_header[ 1] = 0;//get_bit(ofdm.rate_field, 2);
	signal_header[ 2] = 0;
	signal_header[ 3] = 0;
	signal_header[ 4] = 0;
	signal_header[ 5] = 0;
	signal_header[ 6] = 0;
	signal_header[ 7] = 0;
	
	// 8th bit is reserved and must be set to 0
	signal_header[ 8] = 0;
	// then 12 bits represent the length
	signal_header[ 9] = get_bit(length,  0);
	signal_header[10] = get_bit(length,  1);
	signal_header[11] = get_bit(length,  2);
	signal_header[12] = get_bit(length,  3);
	signal_header[13] = get_bit(length,  4);
	signal_header[14] = get_bit(length,  5);
	signal_header[15] = get_bit(length,  6);
	signal_header[16] = get_bit(length,  7);
	signal_header[17] = get_bit(length,  8);
	signal_header[18] = get_bit(length,  9);
	signal_header[19] = get_bit(length, 10);
	signal_header[20] = get_bit(length, 11);

	//21-st bit is the parity bit for the first 20 bits
	int sum = 0;
	for(int i = 0; i < 20; i++) {
		if(signal_header[i]) {
			sum++;
		}
	}
	signal_header[21] = sum % 2;

	// last 6 bits must be set to 0
	for (int i = 0; i < 6; i++) {
		signal_header[22 + i] = 0;
	}

	std::vector<int> resource_block_e (4, 0);
	ofdm_param signal_ofdm(resource_block_e);
	frame_param signal_param(signal_ofdm, 0);

	// convolutional encoding (scrambling is not needed)
	convolutional_encoding(signal_header, encoded_signal_header, signal_param);
	// interleaving
	interleave(encoded_signal_header, out, signal_param, signal_ofdm);

	free(signal_header);
	free(encoded_signal_header);
	free(interleaved_signal_header);
}

bool signal_field_impl::header_formatter(long packet_len, unsigned char *out, const std::vector<tag_t> &tags)
{
	bool encoding_found = false;
	bool len_found = false;
	std::vector<int> encoding;
	int len = 0;

	// read tags
	for (int i = 0; i < tags.size(); i++) {
		if(pmt::eq(tags[i].key, pmt::mp("encoding"))) {
			encoding_found = true;
			encoding = pmt::s32vector_elements(tags[i].value);
		} else if(pmt::eq(tags[i].key, pmt::mp("psdu_len"))) {
			len_found = true;
			len = pmt::to_long(tags[i].value);
		}
	}

	// check if all tags are present
	if( (!encoding_found) || (!len_found)) {
		return false;
	}

	ofdm_param ofdm(encoding);
	frame_param frame(ofdm, len);

	generate_signal_field((char*)out, frame, ofdm);
	return true;
}

bool signal_field_impl::header_parser(
		const unsigned char *in, std::vector<tag_t> &tags) {

	throw std::runtime_error("not implemented yet");
	return false;
}