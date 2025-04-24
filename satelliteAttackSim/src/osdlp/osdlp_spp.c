/*
 * Space Packet Protocol
 *
 * Copyright (C) 2021 TU Darmstadt Space Technology e.V.
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "../osdlp/osdlp_spp.h"

#include <string.h>


int32_t
osdlp_spp_pack(struct spp_prim_hdr *prim_hdr, uint8_t *in_pack_data,
               uint8_t *out_buf, size_t buf_size)
{
	int32_t out_len; /*output data length */

	/* but allow in_pack_data to e NULL */
	if (!prim_hdr || !out_buf)
		return -1;

	out_len = sizeof(struct spp_prim_hdr);
	if (buf_size < out_len)
		return -1;

	/* primary header */
	out_buf[0] = ((prim_hdr->version & 0x07) << 5) |
	             ((prim_hdr->is_tc & 0x01) << 4) |
	             ((prim_hdr->has_sec_hdr & 0x01) << 3) |
	             ((prim_hdr->apid >> 8) & 0x07);
	out_buf[1] = prim_hdr->apid & 0xFF;
	out_buf[2] = ((prim_hdr->seq_flag & 0x03) << 6) |
	             ((prim_hdr->seq_count >> 8) & 0x3F);
	out_buf[3] = prim_hdr->seq_count & 0xFF;
	out_buf[4] = (prim_hdr->packet_data_len >> 8) & 0xFF;
	out_buf[5] = prim_hdr->packet_data_len & 0xFF;

	/* if packet data (secondary header + user data) */
	if (in_pack_data) {
		out_len += prim_hdr->packet_data_len + 1;
		if (buf_size < out_len)
			return -1;

		memcpy(&out_buf[sizeof(struct spp_prim_hdr)], in_pack_data,
		       (prim_hdr->packet_data_len + 1));
	}

	return out_len;
}

int32_t
osdlp_spp_unpack(struct spp_prim_hdr *prim_hdr, uint8_t *in_buf, size_t in_len,
                 uint8_t **out_pack_data)
{
	int32_t parsed_len; /* length of parsed data */

	/* but allow out_pack_data to be NULL */
	if (!in_buf || !prim_hdr)
		return -1;

	parsed_len = sizeof(struct spp_prim_hdr);
	if (in_len < parsed_len)
		return -1;

	/* primary header */
	prim_hdr->version = ((in_buf[0] >> 5) & 0x07);
	prim_hdr->is_tc = ((in_buf[0] >> 4) & 0x01);
	prim_hdr->has_sec_hdr = ((in_buf[0] >> 3) & 0x01);
	prim_hdr->apid = (((uint16_t)in_buf[0] & 0x07) << 8) | in_buf[1];
	prim_hdr->seq_flag = ((in_buf[2] >> 6) & 0x03);
	prim_hdr->seq_count = (((uint16_t)in_buf[2] & 0x3F) << 8) | in_buf[3];
	prim_hdr->packet_data_len = ((uint16_t)in_buf[4] << 8) | in_buf[5];

	/* packet data (secondary header + user data) */
	if (out_pack_data) {
		parsed_len += prim_hdr->packet_data_len + 1;
		if (in_len < parsed_len)
			return -1;

		*out_pack_data = &in_buf[sizeof(struct spp_prim_hdr)];
	}

	return parsed_len;
}
