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

#ifndef INCLUDE_OSDLP_SPP_H_
#define INCLUDE_OSDLP_SPP_H_

#include <stddef.h>
#include <stdint.h>

/** Default SPP packet version number */
#define SPP_PACKET_VERSION_NUMBER   0

/** APID for idle packets */
#define SPP_APID_IDLE   0x07FF

/** Sequence flag values */
#define SPP_SEQ_FLAG_CONTINUATION_SEGMENT   0 /**< continuation segment of user data */
#define SPP_SEQ_FLAG_FIRST_SEGMENT          1 /**< first segment of user data */
#define SPP_SEQ_FLAG_LAST_SEGMENT           2 /**< last segment of user data */
#define SPP_SEQ_FLAG_UNSEGMENTED            3 /**< unsegmented usser data */

/** SPP primary header */
struct spp_prim_hdr {
	uint16_t version : 3; /**< packet version number */
	uint16_t is_tc : 1; /**< packet type - 1 if TC, 0 if TM */
	uint16_t has_sec_hdr : 1; /**< secondary header flag */
	uint16_t apid : 11; /**< application process ID (APID) */
	uint16_t seq_flag : 2; /**< sequence flag */
	uint16_t seq_count : 14; /**< packet sequence count or packet name */
	uint16_t packet_data_len; /**< packet data length */
} __attribute__((packed));

/**
 * Packs SPP primary header (mandatory) and packet data (optional) into a buffer.
 * If `in_pack_data` is NULL - only SPP primary header is packed
 *
 * @param[in] prim_hdr SPP primary header
 * @param[in] in_pack_data SPP packet data, may be NULL
 * @param[out] out_buf buffer where SPP packet will be placed
 * @param[in] buf_size size of output buffer
 * @retval (< 0) error: invalid input, or buffer too small
 * @retval (>= 0) length of data in output buffer
 */
int32_t
osdlp_spp_pack(struct spp_prim_hdr *prim_hdr, uint8_t *in_pack_data,
               uint8_t *out_buf, size_t buf_size);

/**
 * Unpacks SPP primary header (mandatory) and packet data (optional) from a buffer.
 * If `out_pack_data` is NULL - only SPP primary header is unnpacked
 *
 * @param[out] prim_hdr unpacked SPP primary header
 * @param[in] in_buf buffer which contains SPP packet
 * @param[in] in_len length of data in input buffer
 * @param[out] out_pack_data pointer to SPP packet data, may be NULL
 * @retval (< 0) error: invalid input, or invalid data in input bbuffer
 * @retval (>= 0) length of parsed data
 */
int32_t
osdlp_spp_unpack(struct spp_prim_hdr *prim_hdr, uint8_t *in_buf, size_t in_len,
                 uint8_t **out_pack_data);

#endif /* INCLUDE_OSDLP_SPP_H_ */
