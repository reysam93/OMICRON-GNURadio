/*
 * Copyright (C) 2016 	Samuel Rey <samuel.rey.escudero@gmail.com>
 *                  	Bastian Bloessl <bloessl@ccs-labs.org>
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
#include "utils.h"

#include <cassert>
#include <cstring>
#include <math.h>
#include <sstream>

/*ofdm_param::ofdm_param() {
	resource_blocks_e = std::vector<int>(4, BPSK);
	punct = P_1_2;
	n_dbps = 0;

	for (int i = 0; i < 4; i++) {
		n_bpcrb[i] = 1;
		n_bpsc += 1;
		n_cbps += 12;
		n_dbprb[i] = 6;
		n_dbps += 6;
	}

	// Mean of the four resource blocks
	n_bpsc = n_bpsc / 4;
}*/

ofdm_param::ofdm_param(std::vector<int> pilots_enc, int puncturing) {
	resource_blocks_e = pilots_enc;
	punct = puncturing;
	n_bpsc = 0;
	n_cbps = 0;
	n_dbps = 0;

	if (punct != P_1_2 && punct != P_3_4 && punct != P_2_3) {
		throw std::invalid_argument("OFDM_PARAM: wrong puncturing");
	}

	// Rate field will not be used. The header sends the codification of each resource blocks directly.
	// Each resource block have 12 carriers
	for (int i = 0; i < 4; i++) {
		switch(pilots_enc[i]) {
		case BPSK:
			n_bpcrb[i] = 1;
			n_bpsc += 1;
			n_cbps += 12;

			if(punct == P_1_2) {
				n_dbprb[i] = 6;
				n_dbps += 6;
			}else if(punct == P_3_4){
				n_dbprb[i] = 9;
				n_dbps += 9;
			}
			break;

		case QPSK:
			n_bpcrb[i] = 2;
			n_bpsc += 2;
			n_cbps += 24;

			if(punct == P_1_2) {
				n_dbprb[i] = 12;
				n_dbps += 12;
			}else if(punct == P_3_4){
				n_dbprb[i] = 18;
				n_dbps += 18;
			}
			break;

		case QAM16:
			n_bpcrb[i] = 4;
			n_bpsc += 4;
			n_cbps += 48;

			if(punct == P_1_2) {
				n_dbprb[i] = 24;
				n_dbps += 24;
			}else if(punct == P_3_4){
				n_dbprb[i] = 36;
				n_dbps += 36;
			}
			break;

		case QAM64:
			n_bpcrb[i] = 6;
			n_bpsc += 6;
			n_cbps += 72;

			if(punct == P_1_2) {
				n_dbprb[i] = 36;
				n_dbps += 36;
			}else if(punct == P_3_4){
				n_dbprb[i] = 54;
				n_dbps += 54;
			} else if (punct == P_2_3){
				n_dbprb[i] == 48;
				n_dbps += 48;
			}
			break;

		default:
			std::cerr << "ERROR: encodin: " << pilots_enc[i] << "\n";
			throw std::invalid_argument("OFDM_PARAM: wrong encoding");
			break;
		}
	}
	// Mean of the four resource blocks
	n_bpsc = n_bpsc / 4;
}

int
ofdm_param::rb_index_from_symbols(int n_symb) {
	if ((n_symb % 48) < 12){
			return 0;
		}else if ((n_symb % 48) < 24){
			return 1;
		}else if((n_symb % 48) < 36){
			return 2;
		}else if((n_symb % 48) < 48){
			return 3;
		}else{
			return -1;
		}
}

std::string
ofdm_param::toFileFormat() {
	std::stringstream ss;
	for (int i = 0; i < 4; i++) {
		ss << resource_blocks_e[i] << ", ";
	}
	ss << punct << "\n";
	return ss.str();
}

void
ofdm_param::print() {
	std::cout << std::endl;
	std::cout << "OFDM Symbol Parameters:" << std::endl;
	std::cout << "n_bpsc: " << n_bpsc << std::endl;
	std::cout << "n_cbps: " << n_cbps << std::endl;
	std::cout << "n_dbps: " << n_dbps << std::endl;
	print_encoding();
	std::cout << std::endl;
}

void
ofdm_param::print_encoding() {
	std::string enc = "";
	std::string punct_str = "";

	for (int i = 0; i < 4; i++) {
		switch(resource_blocks_e[i]){
		case BPSK:
			enc = "BPSK";
			break;
		case QPSK:
			enc = "QPSK";
			break;
		case QAM16:
			enc = "16QAM";
			break;
		case QAM64:
			enc = "64QAM";
			break;
		default:
			enc = "Unknown modulation.";
			break;
		}
		std::cout << "Encoding Resource Block " << i << ": " << enc << " (" << n_bpcrb[i] << " bpcrb)\n";
	}
	switch (punct){
	case P_1_2:
		punct_str = "1/2";
		break;
	case P_3_4:
		punct_str = "3/4";
		break;
	case P_2_3:
		punct_str = "2/3";
		break;
	default:
		punct_str = "Unknown puncturing.";
		break;
	}
	std::cout << "Frame Puncturing: " << punct_str << std::endl;
	std::cout << std::endl;
}

frame_param::frame_param(ofdm_param &ofdm, int psdu_length) {
	psdu_size = psdu_length;

	// number of symbols (17-11)
	n_sym = (int) ceil((16 + 8 * psdu_size + 6) / (double) ofdm.n_dbps);

	n_data_bits = n_sym * ofdm.n_dbps;

	// number of padding bits (17-13)
	n_pad = n_data_bits - (16 + 8 * psdu_size + 6);

	n_encoded_bits = n_sym * ofdm.n_cbps;
}

void
frame_param::to_header_param() {
	psdu_size = 0;
	n_sym = 2;
	n_data_bits = 48;
	n_encoded_bits = 96;
	n_pad = n_data_bits - 22; //number of bits used in the header
	// Minimun 6 bits of padding needed
}

void
frame_param::print() {
	std::cout << std::endl;
	std::cout << "FRAME Parameters:" << std::endl;
	std::cout << "psdu_size (bytes): " << psdu_size << std::endl;
	std::cout << "n_sym: " << n_sym << std::endl;
	std::cout << "n_pad: " << n_pad << std::endl;
	std::cout << "n_encoded_bits: " << n_encoded_bits << std::endl;
	std::cout << "n_data_bits: " << n_data_bits << std::endl << std::endl;
}

void scramble(const char *in, char *out, frame_param &frame, char initial_state) {
    int state = initial_state;
    int feedback;

    for (int i = 0; i < frame.n_data_bits; i++) {
		feedback = (!!(state & 64)) ^ (!!(state & 8));
		out[i] = feedback ^ in[i];
		state = ((state << 1) & 0x7e) | feedback;
    }
}

void reset_tail_bits(char *scrambled_data, frame_param &frame) {
	memset(scrambled_data + frame.n_data_bits - frame.n_pad - 6, 0, 6 * sizeof(char));
}

int ones(int n) {
	int sum = 0;
	for(int i = 0; i < 8; i++) {
		if(n & (1 << i)) {
			sum++;
		}
	}
	return sum;
}

void convolutional_encoding(const char *in, char *out, frame_param &frame) {
	int state = 0;

	for(int i = 0; i < frame.n_data_bits; i++) {
		assert(in[i] == 0 || in[i] == 1);
		state = ((state << 1) & 0x7e) | in[i];
		out[i * 2]     = ones(state & 0155) % 2;
		out[i * 2 + 1] = ones(state & 0117) % 2;
	}
}

void puncturing(const char *in, char *out, frame_param &frame, ofdm_param &ofdm) {
	int mod;

	for (int i = 0; i < frame.n_data_bits * 2; i++) {
		switch(ofdm.punct) {
			case P_1_2:
				*out = in[i];
				out++;
				break;
			case P_3_4:
				mod = i % 6;
				if (!(mod == 3 || mod == 4)) {
					*out = in[i];
					out++;
				}
				break;
			case P_2_3:
				if (i % 4 != 3) {
					*out = in[i];
					out++;
				}
				break;
			default:
				throw std::invalid_argument("PUNCTURING: wrong modulation");
				break;
		}
	}
}

void interleave(const char *in, char *out, frame_param &frame, ofdm_param &ofdm, bool reverse) {
	int n_cbps = ofdm.n_cbps;
	int first[n_cbps];
	int second[n_cbps];
	int s = floor(std::max(int(ofdm.n_bpsc) / 2, 1));

	for(int j = 0; j < n_cbps; j++) {
		first[j] = s * (j / s) + ((j + int(floor(12.0 * j / n_cbps))) % s);
	}
	for(int i = 0; i < n_cbps; i++) {
		second[i] = 12 * i - (n_cbps - 1) * int(floor(12.0 * i / n_cbps));
	}

	for(int i = 0; i < frame.n_sym; i++) {
		for(int k = 0; k < n_cbps; k++) {
			if(reverse) {
				out[i * n_cbps + second[first[k]]] = in[i * n_cbps + k];
			} else {
				out[i * n_cbps + k] = in[i * n_cbps + second[first[k]]];
			}
		}
	}
}


void split_symbols(const char *in, char *out, frame_param &frame, ofdm_param &ofdm) {
	int symbols = frame.n_sym * 48;
	int bpsc;
	int rb_index;

	for (int i = 0; i < symbols; i++) {
		rb_index = ofdm.rb_index_from_symbols(i);
		if (rb_index < 0)
			throw std::invalid_argument("SPLIT_SYMBS: wrong rb index");
		bpsc = ofdm.n_bpcrb[rb_index];
		out[i] = 0;
		for(int k = 0; k < bpsc; k++) {
			assert(*in == 1 || *in == 0);
			out[i] |= (*in << k);
			in++;
		}
	}
}


void generate_bits(const char *psdu, char *data_bits, frame_param &frame) {
	// first 16 bits are zero (SERVICE/DATA field)
	memset(data_bits, 0, 16);
	data_bits += 16;

	for(int i = 0; i < frame.psdu_size; i++) {
		for(int b = 0; b < 8; b++) {
			data_bits[i * 8 + b] = !!(psdu[i] & (1 << b));
		}
	}
}

void
print_bytes(std::string tag, char bytes[], int size)
{
    std::cout << tag << std::endl;
    for(int i = 0; i < size; i++)
    {
    	std::cout << std::setw(2) << std::setfill('0') << int(bytes[i]);
        std::cout << " ";
 	}
    std::cout << std::endl << std::endl;
}
