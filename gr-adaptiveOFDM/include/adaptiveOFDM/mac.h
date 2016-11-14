/*
 * Copyright (C) 2013, 2016 Bastian Bloessl <bloessl@ccs-labs.org>
 *                          Samuel Rey Escudero <samuel.rey.escudero@gmail.com>
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
#ifndef INCLUDED_ADAPTIVEOFD_MAC_H
#define INCLUDED_ADAPTIVEOFD_MAC_H

#include <adaptiveOFDM/api.h>
#include <gnuradio/block.h>
#include <unistd.h>

namespace gr {
namespace adaptiveOFDM {

class ADAPTIVEOFDM_API mac : virtual public block
{
public:

	/* 
	 * Min SNR has been calculated considering a maximum bit error rate probability
	 * of 0.001 and using the approximation:
	 *
	 *		Pbmax = 0.2*e(-1'5*SNR_MIN/(M-1))
	 *
	 * where M is the bits per simbol in the modulation.
	 */
	static const float MIN_SNR_BPSK = 3.532211577698691;
	static const float MIN_SNR_QPSK = 3.532211577698691*3;
	static const float MIN_SNR_16QAM = 3.532211577698691*15;
	static const float MIN_SNR_64QAM = 3.532211577698691*63;

	// Time in usecs
	static const unsigned int SIFS = 10;
	static const unsigned int SLOT_TIME = 9;
	static const unsigned int TIME_OUT = 30;

	int d_encoding;
	pthread_mutex_t d_mutex;

	int get_encoding();

  	typedef boost::shared_ptr<mac> sptr;
  	static sptr make(std::vector<uint8_t> src_mac,
      	std::vector<uint8_t> dst_mac,
      	std::vector<uint8_t> bss_mac);
};

}  // namespace adaptiveOFDM
}  // namespace gr

#endif /* INCLUDED_ADAPTIVEOFD_MAC_H */
