/*
 *  Open Space Data Link Protocol
 *
 *  Copyright (C) 2020 Libre Space Foundation (https://libre.space)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../osdlp/osdlp_clcw.h"

void
osdlp_clcw_pack(struct clcw_frame *clcw_params, uint8_t *packet)
{
	packet[0] 	= ((clcw_params->ctrl_word_type & 0x01) << 7);
	packet[0] 	|= ((clcw_params->clcw_version_num & 0x03) << 5);
	packet[0]	|= ((clcw_params->status_field & 0x07) << 2);
	packet[0]	|= (clcw_params->cop_in_effect & 0x03);

	packet[1] 	= ((clcw_params->vcid & 0x3f) << 2);
	packet[1]	|= (clcw_params->rsvd_spare1 & 0x03);

	packet[2]	= ((clcw_params->rf_avail & 0x01) << 7);
	packet[2]	|= ((clcw_params->bit_lock & 0x01) << 6);
	packet[2]	|= ((clcw_params->lockout & 0x01) << 5);
	packet[2]	|= ((clcw_params->wait & 0x01) << 4);
	packet[2]	|= ((clcw_params->rt & 0x01) << 3);
	packet[2]	|= ((clcw_params->farm_b_counter & 0x03) << 1);
	packet[2]	|= (clcw_params->rsvd_spare2 & 0x01);

	packet[3]	= clcw_params->report_value;
}

void
osdlp_clcw_unpack(struct clcw_frame *clcw_params, uint8_t *packet)
{
	clcw_params->ctrl_word_type = ((packet[0] >> 7) & 0x01);
	clcw_params->clcw_version_num = ((packet[0] >> 5) & 0x03);
	clcw_params->status_field = ((packet[0] >> 2) & 0x07);
	clcw_params->cop_in_effect = (packet[0] & 0x03);

	clcw_params->vcid = ((packet[1] >> 2) & 0x3f);
	clcw_params->rsvd_spare1 = (packet[1] & 0x03);

	clcw_params->rf_avail = ((packet[2] >> 7) & 0x01);
	clcw_params->bit_lock = ((packet[2] >> 6) & 0x01);
	clcw_params->lockout = ((packet[2] >> 5) & 0x01);
	clcw_params->wait = ((packet[2] >> 4) & 0x01);
	clcw_params->rt = ((packet[2] >> 3) & 0x01);
	clcw_params->farm_b_counter = ((packet[2] >> 1) & 0x03);
	clcw_params->rsvd_spare2 = (packet[2] & 0x01);

	clcw_params->report_value = packet[3];
}


