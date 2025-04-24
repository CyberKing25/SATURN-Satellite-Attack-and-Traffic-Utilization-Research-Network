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

#include "../osdlp/osdlp_tm.h"

#include <stdlib.h>
#include <string.h>

int
osdlp_tm_init(struct tm_transfer_frame *tm_tf,
              uint16_t spacecraft_id,
              uint8_t *mc_count,
              uint8_t vcid,
              tm_ocf_flag_t ocf_flag,
              tm_ocf_type_t ocf_type,
              tm_sec_hdr_flag_t sec_hdr_fleg,
              tm_sync_flag_t sync_flag,
              uint8_t sec_hdr_len,
              uint8_t *sec_hdr,
              tm_crc_flag_t crc_flag,
              uint16_t frame_size,
              uint16_t max_sdu_len,
              uint16_t max_vcs,
              uint16_t max_fifo_size,
              tm_stuff_state_t stuffing,
              uint8_t *util_buffer)
{
	if (frame_size <= TM_PRIMARY_HDR_LEN) {
		return -1;
	}
	struct tm_mission_params m;
	tm_tf->primary_hdr.mcid.version_num 	= TM_VERSION_NUMBER;
	tm_tf->primary_hdr.mcid.spacecraft_id 	= spacecraft_id & 0x03ff;
	tm_tf->primary_hdr.vcid 				= vcid;
	tm_tf->primary_hdr.ocf 					= ocf_flag & 0x01;
	tm_tf->primary_hdr.mc_frame_cnt			= mc_count;
	tm_tf->primary_hdr.vc_frame_cnt 		= 0;
	tm_tf->primary_hdr.status.sec_hdr 		= sec_hdr_fleg & 0x01;
	tm_tf->primary_hdr.status.sync 			= sync_flag & 0x01;
	tm_tf->primary_hdr.status.pkt_order		= 0;
	tm_tf->primary_hdr.status.seg_len_id 	= 0x03;

	m.crc_present               = crc_flag;
	m.frame_len	                = frame_size;
	m.max_vcs                   = max_vcs;
	m.util.buffer               = util_buffer;
	m.vcid                      = vcid;
	m.util.buffered_length      = 0;
	m.util.loop_state           = TM_LOOP_CLOSED;
	m.util.expected_pkt_len     = 0;
	m.stuff_state               = stuffing;
	m.tx_fifo_max_size          = max_fifo_size;
	m.max_sdu_len               = max_sdu_len;
	m.ocf_type                  = ocf_type;


	uint16_t occupied = TM_PRIMARY_HDR_LEN;
	uint16_t occupied_header = TM_PRIMARY_HDR_LEN;
	if (tm_tf->primary_hdr.status.sec_hdr == TM_SEC_HDR_PRESENT) {
		tm_tf->secondary_hdr.sec_hdr_id.version_num = 0;
		tm_tf->secondary_hdr.sec_hdr_id.length = sec_hdr_len;
		memcpy(tm_tf->secondary_hdr.sec_hdr_data_field,
		       sec_hdr,
		       sec_hdr_len * sizeof(uint8_t));
		occupied += tm_tf->secondary_hdr.sec_hdr_id.length + 1;
		occupied_header = occupied;
	}
	if (tm_tf->primary_hdr.ocf == TM_OCF_PRESENT) {
		occupied += TM_OCF_LENGTH;
	}
	if (m.crc_present == TM_CRC_PRESENT) {
		occupied += TM_CRC_LENGTH;
	}

	m.max_data_len = m.frame_len - occupied;
	m.header_len = occupied_header;
	tm_tf->mission = m;
	return 0;
}

void
osdlp_tm_pack(struct tm_transfer_frame *tm_tf, uint8_t *pkt_out,
              uint8_t *data_in, uint16_t length)
{
	pkt_out[0] = ((tm_tf->primary_hdr.mcid.version_num & 0x03) << 6);
	pkt_out[0] |= ((tm_tf->primary_hdr.mcid.spacecraft_id >> 4) & 0x3f);
	pkt_out[1] = ((tm_tf->primary_hdr.mcid.spacecraft_id & 0x0f) << 4);
	pkt_out[1] |= ((tm_tf->primary_hdr.vcid & 0x07) << 1);
	pkt_out[1] |= (tm_tf->primary_hdr.ocf & 0x01);

	pkt_out[2] = (*tm_tf->primary_hdr.mc_frame_cnt & 0xff);
	pkt_out[3] = (tm_tf->primary_hdr.vc_frame_cnt & 0xff);

	pkt_out[4] = ((tm_tf->primary_hdr.status.sec_hdr & 0x01) << 7);
	pkt_out[4] |= ((tm_tf->primary_hdr.status.sync & 0x01) << 6);
	pkt_out[4] |= ((tm_tf->primary_hdr.status.pkt_order & 0x01) << 5);
	pkt_out[4] |= ((tm_tf->primary_hdr.status.seg_len_id & 0x03) << 3);
	pkt_out[4] |= ((tm_tf->primary_hdr.status.first_hdr_ptr >> 8) & 0x07);
	pkt_out[5] = tm_tf->primary_hdr.status.first_hdr_ptr & 0xff;

	if (tm_tf->primary_hdr.status.sec_hdr == TM_SEC_HDR_PRESENT) {
		pkt_out[6] = ((tm_tf->secondary_hdr.sec_hdr_id.version_num &
		               0x03) << 6);
		pkt_out[6] |= (tm_tf->secondary_hdr.sec_hdr_id.length & 0x3f);
		memcpy(&pkt_out[7], tm_tf->secondary_hdr.sec_hdr_data_field,
		       sizeof(uint8_t)*tm_tf->secondary_hdr.sec_hdr_id.length);
	}
	if (length != 0) {
		memcpy(&pkt_out[tm_tf->mission.header_len],
		       data_in, length * sizeof(uint8_t));
	}
	/* Add OCF */
	if (tm_tf->primary_hdr.ocf == TM_OCF_PRESENT) {
		memcpy(&pkt_out[tm_tf->mission.header_len + tm_tf->mission.max_data_len],
		       tm_tf->ocf, 4 * sizeof(uint8_t));
	}

	/* Add idle packets */
	if (length < tm_tf->mission.max_data_len) {
		memset(&pkt_out[tm_tf->mission.header_len + length],
		       TM_IDLE_PACKET,
		       (tm_tf->mission.max_data_len - length) * sizeof(uint8_t));
	}

	/* Add CRC */
	if (tm_tf->mission.crc_present == TM_CRC_PRESENT) {
		uint16_t crc = 0;
		pkt_out[tm_tf->mission.frame_len - 2] = 0;
		pkt_out[tm_tf->mission.frame_len - 1] = 0;
		crc = osdlp_calc_crc(pkt_out, tm_tf->mission.frame_len - 2);

		pkt_out[tm_tf->mission.frame_len - 2] = (crc >> 8) & 0xff;
		pkt_out[tm_tf->mission.frame_len - 1] = crc & 0xff;
		tm_tf->crc = crc;
	}


}

void
osdlp_tm_unpack(struct tm_transfer_frame *tm_tf, uint8_t *pkt_in)
{
	tm_tf->primary_hdr.mcid.version_num 	= ((pkt_in[0] >> 6) && 0x03);
	tm_tf->primary_hdr.mcid.spacecraft_id 	= ((pkt_in[0] & 0x3f) << 4) | ((
	                        pkt_in[1] >> 4) & 0x0f);
	tm_tf->primary_hdr.vcid 				= (pkt_in[1] >> 1) & 0x07;
	tm_tf->primary_hdr.ocf	 				= pkt_in[1] & 0x01;
	*tm_tf->primary_hdr.mc_frame_cnt 		= pkt_in[2];
	tm_tf->primary_hdr.vc_frame_cnt 		= pkt_in[3];
	tm_tf->primary_hdr.status.sec_hdr 		= (pkt_in[4] >> 7) & 0x01;
	tm_tf->primary_hdr.status.sync 			= (pkt_in[4] >> 6) & 0x01;
	tm_tf->primary_hdr.status.pkt_order 	= (pkt_in[4] >> 5) & 0x01;
	tm_tf->primary_hdr.status.seg_len_id 	= (pkt_in[4] >> 3) & 0x03;
	tm_tf->primary_hdr.status.first_hdr_ptr	= ((pkt_in[4] & 0x07) << 8) | pkt_in[5];

	if (tm_tf->primary_hdr.status.sec_hdr == TM_SEC_HDR_PRESENT) {
		tm_tf->secondary_hdr.sec_hdr_id.version_num 	= (pkt_in[6] >> 6) & 0x03;
		tm_tf->secondary_hdr.sec_hdr_id.length			= pkt_in[6] & 0x3f;
	}
	tm_tf->data = &pkt_in[tm_tf->mission.header_len];

	if (tm_tf->primary_hdr.ocf == TM_OCF_PRESENT) {
		memcpy(tm_tf->ocf, &pkt_in[tm_tf->mission.header_len +
		                                                     tm_tf->mission.max_data_len],
		       4 * sizeof(uint8_t));
	}
	if (tm_tf->mission.crc_present == TM_CRC_PRESENT) {
		//tm_tf->crc = calc_crc(pkt_)
		tm_tf->crc = pkt_in[tm_tf->mission.frame_len - 2] << 8;
		tm_tf->crc |= pkt_in[tm_tf->mission.frame_len - 1];
	}
}

static void
pack_crc(struct tm_transfer_frame *tm_tf, uint8_t *pkt_out)
{
	if (tm_tf->mission.crc_present == TM_CRC_PRESENT) {
		uint16_t crc = 0;
		pkt_out[tm_tf->mission.frame_len - 2] = 0;
		pkt_out[tm_tf->mission.frame_len - 1] = 0;
		crc = osdlp_calc_crc(pkt_out, tm_tf->mission.frame_len - 2);

		pkt_out[tm_tf->mission.frame_len - 2] = (crc >> 8) & 0xff;
		pkt_out[tm_tf->mission.frame_len - 1] = crc & 0xff;
	}
}

void
osdlp_abort_segmentation(struct tm_transfer_frame *tm_tf)
{
	tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
	tm_tf->mission.util.buffered_length = 0;
}

void
osdlp_disable_packet_stuffing(struct tm_transfer_frame *tm_tf)
{
	tm_tf->mission.stuff_state = TM_STUFFING_OFF;
}

void
osdlp_enable_packet_stuffing(struct tm_transfer_frame *tm_tf)
{
	tm_tf->mission.stuff_state = TM_STUFFING_ON;
}

/**
 * Returns the number of useful bytes that already are stored in
 * a buffer
 */
static uint16_t
eval_residue_len(struct tm_transfer_frame *tm_tf,
                 uint8_t *last_pkt, uint8_t vcid)
{
	int ret;
	uint16_t residue_len = 0;
	uint16_t pkt_len = 0;
	/*There is at least one packet in fifo*/
	uint16_t first_hdr_ptr = ((last_pkt[4] & 0x07) << 8) | last_pkt[5];
	if (first_hdr_ptr != TM_FIRST_HDR_PTR_NO_PKT_START &&
	    first_hdr_ptr != TM_FIRST_HDR_PTR_OID) {
		/*The packet contains the start of a packet*/

		if (last_pkt[tm_tf->mission.header_len +
		                                       first_hdr_ptr] == TM_IDLE_PACKET) {
			residue_len = first_hdr_ptr;
			return residue_len;
		} else {
			residue_len += first_hdr_ptr;
			while (residue_len <= tm_tf->mission.max_data_len) {
				ret = osdlp_tm_get_packet_len(&pkt_len,
				                              &last_pkt[tm_tf->mission.header_len + residue_len],
				                              tm_tf->mission.max_data_len);
				if (ret < 0) {
					return 0;
				}
				if (pkt_len + tm_tf->mission.header_len + residue_len
				    >= tm_tf->mission.max_data_len) {
					/* The contained packed spills into the next packet*/
					residue_len += pkt_len;
					break;
				}
				residue_len += pkt_len;
				if (last_pkt[tm_tf->mission.header_len +
				                                       residue_len] == TM_IDLE_PACKET) {
					break;
				}
			}
		}
	} else if (first_hdr_ptr == TM_FIRST_HDR_PTR_OID) {
		return 0;
	} else if (first_hdr_ptr == TM_FIRST_HDR_PTR_NO_PKT_START) {
		return tm_tf->mission.max_data_len;
	}
	return residue_len;
}

static void
handle_pkt_stuffing(struct tm_transfer_frame *tm_tf,
                    uint16_t num_packets, uint8_t *last_pkt,
                    uint8_t *data_in, uint16_t length,
                    uint16_t *remaining_len, uint16_t residue_len)
{
	uint16_t chunk_size = 0;
	/*There is room in the last packet so let's use it*/
	if (num_packets == 1) {
		memcpy(&last_pkt[tm_tf->mission.header_len +
		                                           residue_len],
		       data_in, length * sizeof(uint8_t));
		*remaining_len = 0;
		uint16_t leftover_space = tm_tf->mission.max_data_len - length
		                          - residue_len;

		/*Add idle data*/
		if (leftover_space > 0) {
			memset(&last_pkt[tm_tf->mission.header_len +
			                                           residue_len + length],
			       TM_IDLE_PACKET, leftover_space * sizeof(uint8_t));
		}
	} else {
		chunk_size = tm_tf->mission.max_data_len - residue_len;
		*remaining_len = length - chunk_size;
		memcpy(&last_pkt[tm_tf->mission.header_len +
		                                           residue_len],
		       data_in, chunk_size * sizeof(uint8_t));
	}
	/*Reset and recalculate CRC*/
	pack_crc(tm_tf, last_pkt);
}

int
osdlp_tm_transmit(struct tm_transfer_frame *tm_tf,
                  uint8_t *data_in, uint16_t length)
{
	int ret;
	uint8_t vcid = tm_tf->mission.vcid;
	uint16_t residue_len = 0;
	uint16_t bytes_avail = 0;
	uint16_t num_packets = 0;
	uint16_t remaining_len = 0;
	uint8_t *last_pkt = NULL;

	if (tm_tf->mission.util.loop_state == TM_LOOP_OPEN) {
		remaining_len = length - tm_tf->mission.util.buffered_length;
	} else {
		remaining_len = length;
	}

	/*Check if last packet in fifo has leftover space*/
	if (!osdlp_tm_tx_queue_empty(vcid)
	    && tm_tf->mission.stuff_state == TM_STUFFING_ON
	    && tm_tf->mission.util.loop_state == TM_LOOP_CLOSED) {
		ret = osdlp_tm_tx_queue_back(&last_pkt,
		                             vcid);		// Get a pointer to the last packet in queue
		if (ret < 0) {
			residue_len = tm_tf->mission.max_data_len;
		} else {
			residue_len = eval_residue_len(tm_tf, last_pkt, vcid);
		}
		/* The FDU has no free space */
		if (residue_len >= tm_tf->mission.max_data_len) {
			residue_len = 0;
			num_packets = (remaining_len) /
			              tm_tf->mission.max_data_len;
			if ((num_packets == 0)
			    || (remaining_len % tm_tf->mission.max_data_len != 0)) {
				num_packets++;
			}
		} else {
			num_packets = (remaining_len + residue_len) /
			              tm_tf->mission.max_data_len;
			if ((num_packets == 0)
			    || ((remaining_len + residue_len) % tm_tf->mission.max_data_len != 0)) {
				num_packets++;
			}
			handle_pkt_stuffing(tm_tf, num_packets, last_pkt,
			                    data_in, length, &remaining_len, residue_len);
			num_packets--;

		}
		osdlp_tm_tx_commit_back(vcid);
	} else {
		num_packets = (remaining_len) /
		              tm_tf->mission.max_data_len;
		if ((num_packets == 0)
		    || (remaining_len % tm_tf->mission.max_data_len != 0)) {
			num_packets++;
		}
	}


	/*Now fill the rest of the packets*/
	while (num_packets > 0) {
		bytes_avail = 0;

		/*Add the proper first header pointer*/
		if (length == remaining_len) {
			tm_tf->primary_hdr.status.first_hdr_ptr = 0;
		} else {
			if (remaining_len >= tm_tf->mission.max_data_len) {
				tm_tf->primary_hdr.status.first_hdr_ptr = TM_FIRST_HDR_PTR_NO_PKT_START;
			} else {
				tm_tf->primary_hdr.status.first_hdr_ptr = remaining_len;
			}
		}
		if (tm_tf->mission.max_data_len >= remaining_len) {
			bytes_avail = remaining_len;
		} else {
			bytes_avail = tm_tf->mission.max_data_len;
		}
		/*Check for overflow on counters*/
		if (*tm_tf->primary_hdr.mc_frame_cnt < 255) {
			*tm_tf->primary_hdr.mc_frame_cnt =
			        *tm_tf->primary_hdr.mc_frame_cnt + 1;
		} else {
			*tm_tf->primary_hdr.mc_frame_cnt = 0;
		}
		if (tm_tf->primary_hdr.vc_frame_cnt < 255) {
			tm_tf->primary_hdr.vc_frame_cnt++;
		} else {
			tm_tf->primary_hdr.vc_frame_cnt = 0;
		}

		osdlp_tm_pack(tm_tf, tm_tf->mission.util.buffer,
		              &data_in[length - remaining_len], bytes_avail);

		num_packets--;
		ret = osdlp_tm_tx_queue_enqueue(tm_tf->mission.util.buffer, vcid);
		if (ret < 0) {
			tm_tf->mission.util.loop_state = TM_LOOP_OPEN;
			tm_tf->mission.util.buffered_length = length - remaining_len;
			return ret;
		}
		remaining_len -= bytes_avail;
	}
	tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
	tm_tf->mission.util.buffered_length = 0;
	return 0;
}

static tm_rx_result_t
handle_ns_ptr_zero(struct tm_transfer_frame *tm_tf)
{
	int ret;
	uint16_t length;
	ret = osdlp_tm_get_packet_len(&length, tm_tf->data,
	                              tm_tf->mission.max_data_len);
	if (ret < 0) {
		return TM_RX_ERROR;
	}
	tm_tf->mission.util.expected_pkt_len = length;
	if (length > tm_tf->mission.max_data_len) {
		memcpy(tm_tf->mission.util.buffer, tm_tf->data, tm_tf->mission.max_data_len *
		       sizeof(uint8_t));
		tm_tf->mission.util.loop_state = TM_LOOP_OPEN;
		tm_tf->mission.util.buffered_length = tm_tf->mission.max_data_len;
		return TM_RX_PENDING;
	} else {
		memcpy(tm_tf->mission.util.buffer, tm_tf->data, length *
		       sizeof(uint8_t));
		tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
		tm_tf->mission.util.buffered_length = 0;
		ret = osdlp_tm_rx_queue_enqueue(tm_tf->mission.util.buffer,
		                                tm_tf->mission.vcid);
		if (ret < 0) {
			return TM_RX_DENIED;
		}
		return TM_RX_OK;
	}
}

static tm_rx_result_t
handle_ns_ptr_positive(struct tm_transfer_frame *tm_tf)
{
	int ret;
	if (tm_tf->mission.util.loop_state == TM_LOOP_CLOSED) { // A packet was lost
		return TM_RX_ERROR;
	}
	uint16_t usable_len = 0;
	if (tm_tf->primary_hdr.status.first_hdr_ptr
	    == TM_FIRST_HDR_PTR_NO_PKT_START) {
		usable_len = tm_tf->mission.max_data_len;
	} else {
		usable_len = tm_tf->primary_hdr.status.first_hdr_ptr;
	}
	if (tm_tf->mission.util.buffered_length
	    + usable_len > tm_tf->mission.max_sdu_len) {
		tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
		tm_tf->mission.util.buffered_length = 0;
		return TM_RX_ERROR;
	} else {
		memcpy(
		        &tm_tf->mission.util.buffer[tm_tf->mission.util.buffered_length],
		        tm_tf->data, usable_len * sizeof(uint8_t));
		if (tm_tf->mission.util.buffered_length + usable_len
		    == tm_tf->mission.util.expected_pkt_len) {
			tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
			tm_tf->mission.util.buffered_length = 0;
			ret = osdlp_tm_rx_queue_enqueue(tm_tf->mission.util.buffer,
			                                tm_tf->mission.vcid);
			if (ret < 0) {
				return TM_RX_DENIED;
			}
			return TM_RX_OK;
		} else {
			tm_tf->mission.util.buffered_length += usable_len;
			return TM_RX_PENDING;
		}
	}
}

static tm_rx_result_t
handle_rx_no_stuffing(struct tm_transfer_frame *tm_tf)
{
	tm_rx_result_t notif;
	if (tm_tf->primary_hdr.status.first_hdr_ptr == 0) {
		notif = handle_ns_ptr_zero(tm_tf);
		return notif;
	} else if ((tm_tf->primary_hdr.status.first_hdr_ptr
	            == TM_FIRST_HDR_PTR_NO_PKT_START)
	           || (tm_tf->primary_hdr.status.first_hdr_ptr > 0)) {
		notif = handle_ns_ptr_positive(tm_tf);
		return notif;
	} else {
		return TM_RX_ERROR;
	}
}

static tm_rx_result_t
handle_s_ptr_zero(struct tm_transfer_frame *tm_tf)
{
	uint16_t length;
	int ret;
	bool more_pkts = true;
	uint16_t bytes_explored = 0;
	while (more_pkts) {
		if (tm_tf->data[bytes_explored] == TM_IDLE_PACKET) {
			tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
			tm_tf->mission.util.buffered_length = 0;
			tm_tf->mission.util.expected_pkt_len = 0;
			return TM_RX_OK;
		}
		ret = osdlp_tm_get_packet_len(&length, &tm_tf->data[bytes_explored],
		                              (tm_tf->mission.max_data_len - bytes_explored));
		if (ret < 0) {
			memcpy(tm_tf->mission.util.buffer, &tm_tf->data[bytes_explored],
			       (tm_tf->mission.max_data_len - bytes_explored) * sizeof(uint8_t));
			tm_tf->mission.util.loop_state = TM_LOOP_OPEN;
			tm_tf->mission.util.buffered_length = (tm_tf->mission.max_data_len -
			                                       bytes_explored);
			tm_tf->mission.util.expected_pkt_len = 0;
			return TM_RX_PENDING;
		}
		tm_tf->mission.util.expected_pkt_len = length;
		if ((length + bytes_explored) > tm_tf->mission.max_data_len) {
			memcpy(tm_tf->mission.util.buffer, &tm_tf->data[bytes_explored],
			       (tm_tf->mission.max_data_len - bytes_explored) *
			       sizeof(uint8_t));
			tm_tf->mission.util.loop_state = TM_LOOP_OPEN;
			tm_tf->mission.util.buffered_length = (tm_tf->mission.max_data_len -
			                                       bytes_explored);
			return TM_RX_PENDING;
		} else {
			memcpy(tm_tf->mission.util.buffer, &tm_tf->data[bytes_explored], length *
			       sizeof(uint8_t));
			tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
			tm_tf->mission.util.buffered_length = 0;
			tm_tf->mission.util.expected_pkt_len = 0;
			ret = osdlp_tm_rx_queue_enqueue(tm_tf->mission.util.buffer,
			                                tm_tf->mission.vcid);
			if (ret < 0) {
				return TM_RX_DENIED;
			}
			bytes_explored += length;
			if (bytes_explored == tm_tf->mission.max_data_len) {
				return TM_RX_OK;
			}
		}
	}
	return TM_RX_ERROR;
}

static tm_rx_result_t
handle_s_ptr_nopkt(struct tm_transfer_frame *tm_tf)
{
	uint16_t length;
	int ret;
	if (tm_tf->mission.util.loop_state == TM_LOOP_CLOSED) { // A packet was lost
		return TM_RX_ERROR;
	}
	/* The length of the packet couldn't be determined from the
	 * previous FDU. Try to determine it now */
	if (tm_tf->mission.util.loop_state == TM_LOOP_OPEN &&
	    tm_tf->mission.util.expected_pkt_len == 0) {
		if (tm_tf->mission.util.buffered_length + tm_tf->mission.max_data_len <
		    tm_tf->mission.max_sdu_len) {
			memcpy(&tm_tf->mission.util.buffer[tm_tf->mission.util.buffered_length],
			       tm_tf->data, tm_tf->mission.max_data_len * sizeof(uint8_t));
			ret = osdlp_tm_get_packet_len(&length, tm_tf->mission.util.buffer,
			                              (tm_tf->mission.max_data_len
			                               + tm_tf->mission.util.buffered_length));
		} else {
			tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
			tm_tf->mission.util.buffered_length = 0;
			return TM_RX_ERROR;
		}
		if (ret >= 0) {
			memcpy(
			        &tm_tf->mission.util.buffer[tm_tf->mission.util.buffered_length],
			        tm_tf->data, tm_tf->mission.max_data_len * sizeof(uint8_t));
			if (tm_tf->mission.max_data_len
			    + tm_tf->mission.util.buffered_length == length) {
				tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
				tm_tf->mission.util.buffered_length = 0;

				ret = osdlp_tm_rx_queue_enqueue(tm_tf->mission.util.buffer,
				                                tm_tf->mission.vcid);
				if (ret < 0) {
					return TM_RX_DENIED;
				}
				return TM_RX_OK;
			} else {
				tm_tf->mission.util.buffered_length += tm_tf->mission.max_data_len;
				return TM_RX_PENDING;
			}
		}
	}

	if (tm_tf->mission.util.buffered_length
	    + tm_tf->mission.max_data_len > tm_tf->mission.max_sdu_len) {
		tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
		tm_tf->mission.util.buffered_length = 0;
		return TM_RX_ERROR;
	} else {
		memcpy(
		        &tm_tf->mission.util.buffer[tm_tf->mission.util.buffered_length],
		        tm_tf->data, tm_tf->mission.max_data_len * sizeof(uint8_t));
		if (tm_tf->mission.util.buffered_length + tm_tf->mission.max_data_len
		    == tm_tf->mission.util.expected_pkt_len) {
			tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
			tm_tf->mission.util.buffered_length = 0;
			ret = osdlp_tm_rx_queue_enqueue(tm_tf->mission.util.buffer,
			                                tm_tf->mission.vcid);
			if (ret < 0) {
				return TM_RX_DENIED;
			}
			return TM_RX_OK;
		} else {
			tm_tf->mission.util.buffered_length += tm_tf->mission.max_data_len;
			return TM_RX_PENDING;
		}
	}
}

static tm_rx_result_t
handle_s_ptr_positive(struct tm_transfer_frame *tm_tf)
{
	uint16_t length;
	int ret;
	bool more_pkts = true;
	uint16_t bytes_explored = 0;
	if (tm_tf->mission.util.loop_state == TM_LOOP_OPEN &&
	    tm_tf->mission.util.expected_pkt_len == 0) {
		if (tm_tf->mission.util.buffered_length +
		    tm_tf->primary_hdr.status.first_hdr_ptr < tm_tf->mission.max_sdu_len) {
			memcpy(&tm_tf->mission.util.buffer[tm_tf->mission.util.buffered_length],
			       tm_tf->data, tm_tf->primary_hdr.status.first_hdr_ptr * sizeof(uint8_t));
			ret = osdlp_tm_get_packet_len(&length, tm_tf->mission.util.buffer,
			                              (tm_tf->primary_hdr.status.first_hdr_ptr
			                               + tm_tf->mission.util.buffered_length));
		} else {
			ret = -1;
		}
		if (ret >= 0) {
			if (tm_tf->primary_hdr.status.first_hdr_ptr
			    + tm_tf->mission.util.buffered_length == length) {
				tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
				tm_tf->mission.util.buffered_length = 0;
				ret = osdlp_tm_rx_queue_enqueue(tm_tf->mission.util.buffer,
				                                tm_tf->mission.vcid);
				if (ret < 0) {
					return TM_RX_DENIED;
				}
			} else {
				tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
				tm_tf->mission.util.buffered_length = 0;
				tm_tf->mission.util.expected_pkt_len = 0;
			}
		}
	} else if (tm_tf->mission.util.loop_state == TM_LOOP_OPEN &&
	           tm_tf->mission.util.expected_pkt_len != 0) {
		if (tm_tf->primary_hdr.status.first_hdr_ptr
		    + tm_tf->mission.util.buffered_length ==
		    tm_tf->mission.util.expected_pkt_len) {
			memcpy(&tm_tf->mission.util.buffer[tm_tf->mission.util.buffered_length],
			       tm_tf->data, tm_tf->primary_hdr.status.first_hdr_ptr * sizeof(uint8_t));
			tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
			tm_tf->mission.util.buffered_length = 0;
			ret = osdlp_tm_rx_queue_enqueue(tm_tf->mission.util.buffer,
			                                tm_tf->mission.vcid);
			if (ret < 0) {
				return TM_RX_DENIED;
			}
		}
	}
	bytes_explored = tm_tf->primary_hdr.status.first_hdr_ptr;
	while (more_pkts) {
		if (tm_tf->data[bytes_explored] == TM_IDLE_PACKET) {
			tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
			tm_tf->mission.util.buffered_length = 0;
			tm_tf->mission.util.expected_pkt_len = 0;
			return TM_RX_OK;
		}
		ret = osdlp_tm_get_packet_len(&length, &tm_tf->data[bytes_explored],
		                              (tm_tf->mission.max_data_len - bytes_explored));
		if (ret < 0) {
			memcpy(tm_tf->mission.util.buffer, &tm_tf->data[bytes_explored],
			       (tm_tf->mission.max_data_len - bytes_explored) * sizeof(uint8_t));
			tm_tf->mission.util.loop_state = TM_LOOP_OPEN;
			tm_tf->mission.util.buffered_length = (tm_tf->mission.max_data_len -
			                                       bytes_explored);
			tm_tf->mission.util.expected_pkt_len = 0;
			return TM_RX_PENDING;
		}
		tm_tf->mission.util.expected_pkt_len = length;
		if ((length + bytes_explored) > tm_tf->mission.max_data_len) {
			memcpy(tm_tf->mission.util.buffer, &tm_tf->data[bytes_explored],
			       (tm_tf->mission.max_data_len - bytes_explored) *
			       sizeof(uint8_t));
			tm_tf->mission.util.loop_state = TM_LOOP_OPEN;
			tm_tf->mission.util.buffered_length = (tm_tf->mission.max_data_len -
			                                       bytes_explored);
			return TM_RX_PENDING;
		} else {
			memcpy(tm_tf->mission.util.buffer, &tm_tf->data[bytes_explored], length *
			       sizeof(uint8_t));
			tm_tf->mission.util.loop_state = TM_LOOP_CLOSED;
			tm_tf->mission.util.buffered_length = 0;
			tm_tf->mission.util.expected_pkt_len = 0;
			ret = osdlp_tm_rx_queue_enqueue(tm_tf->mission.util.buffer,
			                                tm_tf->mission.vcid);
			if (ret < 0) {
				return TM_RX_DENIED;
			}
			bytes_explored += length;
			if (bytes_explored == tm_tf->mission.max_data_len) {
				return TM_RX_OK;
			}
		}
	}
	return TM_RX_ERROR;
}

static tm_rx_result_t
handle_rx_stuffing(struct tm_transfer_frame *tm_tf)
{
	tm_rx_result_t notif;
	if (tm_tf->primary_hdr.status.first_hdr_ptr == 0) {
		notif = handle_s_ptr_zero(tm_tf);
		return notif;
	} else if (tm_tf->primary_hdr.status.first_hdr_ptr
	           == TM_FIRST_HDR_PTR_NO_PKT_START) {
		notif = handle_s_ptr_nopkt(tm_tf);
		return notif;
	} else { // First header pointer points inside the buffer
		/* The length of the packet couldn't be determined from the
		 * previous FDU. Try to determine it now */
		notif = handle_s_ptr_positive(tm_tf);
		return notif;
	}
}

int
osdlp_tm_receive(uint8_t *data_in)
{
	tm_rx_result_t notif;
	uint8_t vcid = (data_in[1] >> 1) & 0x07;
	struct tm_transfer_frame *tm_tf;
	int ret = osdlp_tm_get_rx_config(&tm_tf, vcid);
	if (ret < 0) {
		return ret;
	}
	osdlp_tm_unpack(tm_tf, data_in);
	uint16_t crc = osdlp_calc_crc(data_in, tm_tf->mission.frame_len - 2);
	if (crc != tm_tf->crc) {
		return -TM_RX_WRONG_CRC;
	}
	if (tm_tf->primary_hdr.status.first_hdr_ptr == TM_FIRST_HDR_PTR_OID) {
		return TM_RX_OID;
	}
	if (tm_tf->mission.stuff_state == TM_STUFFING_OFF) {
		notif = handle_rx_no_stuffing(tm_tf);
	} else { // Stuffing on
		notif = handle_rx_stuffing(tm_tf);
	}
	return -notif;
}

int
osdlp_tm_transmit_idle_fdu(struct tm_transfer_frame *tm_tf, uint8_t vcid)
{
	int ret;
	tm_tf->primary_hdr.status.first_hdr_ptr = TM_FIRST_HDR_PTR_OID;
	osdlp_tm_pack(tm_tf, tm_tf->mission.util.buffer,
	              NULL, 0);
	ret = osdlp_tm_tx_queue_enqueue(tm_tf->mission.util.buffer, vcid);
	if (ret < 0) {
		return ret;
	}
	return 0;
}


