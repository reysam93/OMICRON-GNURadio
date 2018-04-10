/*
 * Copyright (C) 2015 Samuel Rey <samuel.rey.escudero@gmail.com>
 *						Bastian Bloessl <bloessl@ccs-labs.org>
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

#ifndef INCLUDED_FREQUENCYADAPTIVEOFDM_EQUALIZER_BASE_H
#define INCLUDED_FREQUENCYADAPTIVEOFDM_EQUALIZER_BASE_H

#include <gnuradio/gr_complex.h>
#include <gnuradio/digital/constellation.h>

namespace gr {
namespace frequencyAdaptiveOFDM {
namespace equalizer {

class base {
public:
	virtual ~base() {};
	virtual void equalize(gr_complex *in, int n, gr_complex *symbols, uint8_t *bits, boost::shared_ptr<gr::digital::constellation> mod[4]) = 0;

	std::vector<double> resource_blocks_snr();
	std::vector<double> min_rb_snr();
	std::vector<double> max_rb_snr();

	static const gr_complex POLARITY[127];

protected:
	static const gr_complex LONG[64];

	std::vector<double> d_resource_block_snr;
	std::vector<double> d_min_rb_snr;
	std::vector<double> d_max_rb_snr;
	gr_complex d_H[64];

	int rb_index_from_carrier(int n_carrier);
	void estimate_channel_state(gr_complex *in);
};

} /* namespace channel_estimation */
} /* namespace frequencyAdaptiveOFDM */
} /* namespace gr */

#endif /* INCLUDED_FREQUENCYADAPTIVEOFDM_EQUALIZER_BASE_H */
