/*
 * Copyright (C) 2015 Bastian Bloessl <bloessl@ccs-labs.org>
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

#ifndef  INCLUDED_ADAPTIVEOFDM_EQUALIZER_BASE_H
#define  INCLUDED_ADAPTIVEOFDM_EQUALIZER_BASE_H

#include <gnuradio/gr_complex.h>
#include <gnuradio/digital/constellation.h>

namespace gr {
namespace adaptiveOFDM {
namespace equalizer {

class base {
public:
	virtual ~base() {};
	virtual void equalize(gr_complex *in, int n, gr_complex *symbols, uint8_t *bits, boost::shared_ptr<gr::digital::constellation> mod) = 0;

	static const gr_complex POLARITY[127];
	bool isPilot(int carrier);
	void estimate_channel_state(gr_complex *in);
	double get_snr();
	double get_snr_min();


	gr_complex d_H[64];
	double d_snr;
    double d_snr_min;

protected:
	static const gr_complex LONG[64];
};

} /* namespace channel_estimation */
} /* namespace adaptiveOFDM */
} /* namespace gr */

#endif /* INCLUDED_ADAPTIVEOFDM_EQUALIZER_BASE_H */