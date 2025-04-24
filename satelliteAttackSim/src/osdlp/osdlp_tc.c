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

#include "../osdlp/osdlp_tc.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../osdlp/osdlp_clcw.h"
#include "../osdlp/osdlp_cop.h"
#include "../osdlp/osdlp_crc.h"

#define	TC_TRANSFER_FRAME_PRIMARY_HEADER		5

int
osdlp_tc_init(struct tc_transfer_frame *tc_tf,
              uint16_t scid,
              uint16_t max_sdu_len,
              uint16_t max_frame_len,
              uint16_t rx_fifo_size,
              uint8_t vcid,
              uint8_t mapid,
              tc_crc_flag_t crc_flag,
              tc_seg_hdr_t seg_hdr_flag,
              tc_bypass_t bypass,
              tc_ctrl_t ctrl_cmd,
              uint8_t *util_buffer,
              struct cop_config cop)
{
	if (max_frame_len <= TC_TRANSFER_FRAME_PRIMARY_HEADER) {
		return -1;
	}
	struct tc_mission_params m;
	tc_tf->primary_hdr.version_num          = TC_VERSION_NUMBER;
	tc_tf->primary_hdr.spacecraft_id        = scid & 0x03ff;
	tc_tf->primary_hdr.vcid                 = vcid;
	tc_tf->primary_hdr.bypass               = bypass;
	tc_tf->primary_hdr.ctrl_cmd             = ctrl_cmd;
	tc_tf->primary_hdr.frame_len            = 0;
	tc_tf->primary_hdr.frame_seq_num        = 0;
	tc_tf->primary_hdr.rsvd_spare           = 0;

	m.version_num                           = TC_VERSION_NUMBER;
	m.spacecraft_id                         = scid;
	m.vcid                                  = vcid;
	m.crc_flag                              = crc_flag;
	m.seg_hdr_flag                          = seg_hdr_flag;
	m.max_frame_len                         = max_frame_len;
	m.max_sdu_len                           = max_sdu_len;
	m.max_data_len                          = max_frame_len;
	m.rx_fifo_max_size                      = rx_fifo_size;
	m.util.buffer                           = util_buffer;
	m.fixed_overhead_len                    = TC_TRANSFER_FRAME_PRIMARY_HEADER;
	m.unlock_cmd                            = 0;
	m.set_vr_cmd[0]                         = SETVR_BYTE1;
	m.set_vr_cmd[1]                         = SETVR_BYTE2;

	if (m.seg_hdr_flag == TC_SEG_HDR_PRESENT) {
		m.fixed_overhead_len += 1;
	}
	if (m.crc_flag == TC_CRC_PRESENT) {
		m.fixed_overhead_len += 2;
	}
	m.max_data_len                          = m.max_frame_len -
	                m.fixed_overhead_len;
	m.util.loop_state                       = TC_LOOP_CLOSED;
	tc_tf->mission                          = m;
	tc_tf->frame_data.seg_hdr.map_id        = mapid & 0x3f;
	tc_tf->cop_cfg                          = cop;
	tc_tf->seg_status.flag                  = SEG_ENDED;
	tc_tf->seg_status.octets_txed           = 0;
	return 0;
}

void
osdlp_tc_unpack(struct tc_transfer_frame *tc_tf,  uint8_t *pkt_in)
{
	struct tc_primary_hdr tc_p_hdr;
	tc_p_hdr.version_num    = ((pkt_in[0] >> 6) & 0x03);
	tc_p_hdr.bypass         = ((pkt_in[0] >> 5) & 0x01);
	tc_p_hdr.ctrl_cmd       = ((pkt_in[0] >> 4) & 0x01);
	tc_p_hdr.rsvd_spare     = ((pkt_in[0] >> 2) & 0x03);
	tc_p_hdr.spacecraft_id  = (((pkt_in[0] & 0x03) << 8) | pkt_in[1]);

	tc_p_hdr.vcid           = ((pkt_in[2] >> 2) & 0x3f);
	tc_p_hdr.frame_len      = (((pkt_in[2] & 0x03) << 8) | pkt_in[3]);
	tc_p_hdr.frame_seq_num  = pkt_in[4];

	tc_tf->primary_hdr      = tc_p_hdr;

	tc_tf->frame_data.data_len = tc_p_hdr.frame_len + 1 -
	                             tc_tf->mission.fixed_overhead_len;

	if (tc_tf->mission.seg_hdr_flag) {
		tc_tf->frame_data.seg_hdr.seq_flag  = (pkt_in[5] >> 6) & 0x03;
		tc_tf->frame_data.seg_hdr.map_id    = pkt_in[5] & 0x3f;
		tc_tf->frame_data.data = &pkt_in[6];

	} else {
		tc_tf->frame_data.data = &pkt_in[5];
	}
	if (tc_tf->mission.crc_flag == TC_CRC_PRESENT) {
		tc_tf->crc = pkt_in[tc_p_hdr.frame_len - 1] << 8;
		tc_tf->crc |= pkt_in[tc_p_hdr.frame_len];
	}
}

void
osdlp_tc_pack(struct tc_transfer_frame *tc_tf, uint8_t *pkt_out,
              uint8_t *data_in, uint16_t length)
{
	uint16_t crc = 0;
	uint16_t packet_len = tc_tf->mission.fixed_overhead_len + length - 1;
	pkt_out[0] = ((tc_tf->primary_hdr.version_num & 0x03) << 6);
	pkt_out[0] |= ((tc_tf->primary_hdr.bypass & 0x01) << 5);
	pkt_out[0] |= ((tc_tf->primary_hdr.ctrl_cmd & 0x01) << 4);
	pkt_out[0] |= ((tc_tf->primary_hdr.rsvd_spare & 0x03) << 2);
	pkt_out[0] |= ((tc_tf->primary_hdr.spacecraft_id >> 8) & 0x03);

	pkt_out[1] = tc_tf->primary_hdr.spacecraft_id & 0xff;

	pkt_out[2] = ((tc_tf->primary_hdr.vcid & 0x3f) << 2);

	pkt_out[2] |= ((packet_len >> 8) & 0x03);
	pkt_out[3] = packet_len & 0xff;

	if (tc_tf->primary_hdr.bypass == TYPE_A) {
		pkt_out[4] = tc_tf->cop_cfg.fop.vs & 0xff;
	} else {
		pkt_out[4] = 0;
	}
	if (tc_tf->mission.seg_hdr_flag) {
		pkt_out[5] = ((tc_tf->frame_data.seg_hdr.seq_flag & 0x03) << 6);
		pkt_out[5] |= (tc_tf->frame_data.seg_hdr.map_id & 0x3f);
		memcpy(&pkt_out[6], data_in, length * sizeof(uint8_t));
	} else {
		memcpy(&pkt_out[5], data_in, length * sizeof(uint8_t));
	}
	if (tc_tf->mission.crc_flag == TC_CRC_PRESENT) {
		crc = osdlp_calc_crc(pkt_out, packet_len - 1);
		pkt_out[packet_len - 1] = (crc >> 8) & 0xff;
		pkt_out[packet_len] = crc & 0xff;
		tc_tf->crc = crc;
	}
}

int
osdlp_frame_validation_check(struct tc_transfer_frame *tc_tf,
                             uint8_t *rx_buffer)
{
	if (tc_tf->mission.version_num != tc_tf->primary_hdr.version_num) {
		return -1;
	}
	if (tc_tf->mission.spacecraft_id != tc_tf->primary_hdr.spacecraft_id) {
		return -1;
	}
	if (tc_tf->mission.crc_flag) {
		uint16_t crc = osdlp_calc_crc(rx_buffer, tc_tf->primary_hdr.frame_len - 1);
		uint16_t rx_crc;
		rx_crc = (rx_buffer[tc_tf->primary_hdr.frame_len - 1] & 0xff) << 8;
		rx_crc |= (rx_buffer[tc_tf->primary_hdr.frame_len] & 0xff);
		if (crc != rx_crc) {
			return -1;
		}
	}
	return 0;
}

int
osdlp_tc_receive(uint8_t *rx_buffer, uint32_t length)
{
	int ret;
	farm_result_t farm_ret;
	/* Delimiting */
	uint16_t frame_len = (rx_buffer[2] & 0x03) << 8;
	frame_len |= (rx_buffer[3] & 0xff);
	if (((frame_len + 1) > length) || (length < TC_TRANSFER_FRAME_PRIMARY_HEADER)) {
		return -TC_RX_FRAME_LEN_ERR;
	}

	/* Frame Validation Checks */
	uint8_t vcid = ((rx_buffer[2] >> 2) & 0x3f);
	struct tc_transfer_frame *tc_tf;
	ret = osdlp_tc_get_rx_config(&tc_tf, vcid);
	if (ret < 0) {
		return -TC_RX_CONFIG_ERR;
	}

	osdlp_tc_unpack(tc_tf, rx_buffer);

	ret = osdlp_frame_validation_check(tc_tf, rx_buffer);
	if (ret < 0) {
		return -TC_RX_FRAME_VAL_ERR;
	}

	/* Perform FARM-1 */
	farm_ret = osdlp_farm_1(tc_tf);
	if (farm_ret == COP_ENQ || farm_ret == COP_PRIORITY_ENQ) {
		/* Handle segmentation */
		if (tc_tf->mission.seg_hdr_flag) {
			switch (tc_tf->frame_data.seg_hdr.seq_flag) {
				case TC_UNSEG:
					if (tc_tf->frame_data.data_len > tc_tf->mission.max_data_len) {
						return -TC_RX_COP_ERR;
					}
					memcpy(tc_tf->mission.util.buffer,
					       tc_tf->frame_data.data,
					       tc_tf->frame_data.data_len * sizeof(uint8_t));
					if (farm_ret == COP_ENQ) {
						ret = osdlp_tc_rx_queue_enqueue(tc_tf->mission.util.buffer,
						                                tc_tf->frame_data.data_len,
						                                vcid);
					} else {
						ret = osdlp_tc_rx_queue_enqueue_now(tc_tf->mission.util.buffer,
						                                    tc_tf->frame_data.data_len,
						                                    vcid);
					}
					tc_tf->mission.util.buffered_length = 0;
					tc_tf->mission.util.loop_state = TC_LOOP_CLOSED;
					if (ret < 0) {
						return -TC_RX_QUEUE_ERR;
					} else {
						return TC_RX_OK;
					}
				case TC_FIRST_SEG:
					if (tc_tf->mission.util.loop_state != TC_LOOP_CLOSED) {
						tc_tf->mission.util.buffered_length = 0;
						tc_tf->mission.util.loop_state = TC_LOOP_CLOSED;
						return -TC_RX_COP_ERR;
					}
					tc_tf->mission.util.buffered_length = tc_tf->frame_data.data_len;
					if (tc_tf->frame_data.data_len > tc_tf->mission.max_data_len) {
						return -TC_RX_COP_ERR;
					}
					memcpy(tc_tf->mission.util.buffer,
					       tc_tf->frame_data.data,
					       tc_tf->frame_data.data_len * sizeof(uint8_t));
					tc_tf->mission.util.loop_state = TC_LOOP_OPEN;
					return TC_RX_OK;
				case TC_LAST_SEG:
					/* An intermediate packet was lost. Loop was never opened*/
					if (tc_tf->mission.util.loop_state != TC_LOOP_OPEN) {
						tc_tf->mission.util.buffered_length = 0;
						return -TC_RX_COP_ERR;
					}
					if (tc_tf->frame_data.data_len + tc_tf->mission.util.buffered_length >
					    tc_tf->mission.max_sdu_len) {
						tc_tf->mission.util.buffered_length = 0;
						tc_tf->mission.util.loop_state = TC_LOOP_CLOSED;
						return -TC_RX_COP_ERR;
					}
					memcpy(&tc_tf->mission.util.buffer[tc_tf->mission.util.buffered_length],
					       tc_tf->frame_data.data,
					       tc_tf->frame_data.data_len * sizeof(uint8_t));
					if (farm_ret == COP_ENQ) {
						ret = osdlp_tc_rx_queue_enqueue(tc_tf->mission.util.buffer,
						                                tc_tf->mission.util.buffered_length + tc_tf->frame_data.data_len,
						                                vcid);
					} else {
						ret = osdlp_tc_rx_queue_enqueue_now(tc_tf->mission.util.buffer,
						                                    tc_tf->mission.util.buffered_length + tc_tf->frame_data.data_len,
						                                    vcid);
					}
					tc_tf->mission.util.loop_state = TC_LOOP_CLOSED;
					if (ret < 0) {
						return -TC_RX_QUEUE_ERR;
					} else {
						return TC_RX_OK;
					}
				case TC_CONT_SEG:
					/* An intermediate packet was lost. Loop was never opened*/
					if (tc_tf->mission.util.loop_state != TC_LOOP_OPEN) {
						tc_tf->mission.util.buffered_length = 0;
						return -TC_RX_COP_ERR;
					}
					if (tc_tf->frame_data.data_len + tc_tf->mission.util.buffered_length >
					    tc_tf->mission.max_sdu_len) {
						tc_tf->mission.util.buffered_length = 0;
						tc_tf->mission.util.loop_state = TC_LOOP_CLOSED;
						return -TC_RX_COP_ERR;
					}

					memcpy(&tc_tf->mission.util.buffer[tc_tf->mission.util.buffered_length],
					       tc_tf->frame_data.data,
					       tc_tf->frame_data.data_len * sizeof(uint8_t));
					tc_tf->mission.util.buffered_length += tc_tf->frame_data.data_len;
					return TC_RX_OK;
			}
		} else {
			if (tc_tf->frame_data.data_len > tc_tf->mission.max_sdu_len) {
				tc_tf->mission.util.buffered_length = 0;
				tc_tf->mission.util.loop_state = TC_LOOP_CLOSED;
				return -TC_RX_COP_ERR;
			}
			memcpy(tc_tf->mission.util.buffer,
			       tc_tf->frame_data.data,
			       tc_tf->frame_data.data_len * sizeof(uint8_t));
			if (farm_ret == COP_ENQ) {
				ret = osdlp_tc_rx_queue_enqueue(tc_tf->mission.util.buffer,
				                                tc_tf->frame_data.data_len,
				                                vcid);
			} else {
				ret = osdlp_tc_rx_queue_enqueue_now(tc_tf->mission.util.buffer,
				                                    tc_tf->frame_data.data_len,
				                                    vcid);
			}
			tc_tf->mission.util.buffered_length = 0;
			tc_tf->mission.util.loop_state = TC_LOOP_CLOSED;
			if (ret < 0) {
				return -TC_RX_QUEUE_ERR;
			} else {
				return TC_RX_OK;
			}
		}
	} else if (farm_ret == COP_ERROR) {
		tc_tf->mission.util.buffered_length = 0;
		tc_tf->mission.util.loop_state = TC_LOOP_CLOSED;
		return -TC_RX_COP_ERR;
	}
	if (farm_ret == COP_OK) {
		return TC_RX_OK;
	} else {
		return -TC_RX_COP_ERR;
	}
}

int
osdlp_tc_transmit(struct tc_transfer_frame *tc_tf, uint8_t *buffer,
                  uint32_t length)
{
	uint16_t remaining = length;
	uint16_t bytes_avail = 0;
	notification_t notif;
	if (tc_tf->seg_status.flag) {
		remaining = length - tc_tf->seg_status.octets_txed;
	}
	while (remaining > 0) {
		if (remaining / tc_tf->mission.max_data_len > 0) {
			bytes_avail = tc_tf->mission.max_data_len;
		} else {
			bytes_avail = remaining;
		}
		/*Prepare the config struct*/
		/* Check if a segmentation process has already begun*/
		if (tc_tf->seg_status.flag) {
			if (remaining > tc_tf->mission.max_data_len) {
				tc_tf->frame_data.seg_hdr.seq_flag = TC_CONT_SEG;
			} else {
				tc_tf->frame_data.seg_hdr.seq_flag = TC_LAST_SEG;
			}
		} else {
			if (remaining <= bytes_avail) {
				tc_tf->frame_data.seg_hdr.seq_flag = TC_UNSEG;
			} else {
				tc_tf->frame_data.seg_hdr.seq_flag = TC_FIRST_SEG;
			}
		}

		tc_tf->primary_hdr.frame_len = tc_tf->mission.fixed_overhead_len + bytes_avail -
		                               1;
		tc_tf->frame_data.data_len = bytes_avail;
		tc_tf->frame_data.data = buffer + (length - remaining);

		if (!osdlp_tc_tx_queue_full(tc_tf->primary_hdr.vcid)) {
			notif = osdlp_req_transfer_fdu(tc_tf);
		} else {
			tc_tf->cop_cfg.fop.signal = REJECT_TX;
			return -TC_TX_COP_ERR;
		}
		remaining -= bytes_avail;
		/* Handle response */
		switch (notif) {
			case REJECT_TX:
				tc_tf->seg_status.flag = SEG_ENDED;
				tc_tf->seg_status.octets_txed = 0;
				tc_tf->cop_cfg.fop.signal = REJECT_TX;
				return -TC_TX_COP_ERR;
			case DELAY_RESP:
				if (remaining > 0) {
					tc_tf->seg_status.flag = SEG_IN_PROGRESS;
					tc_tf->seg_status.octets_txed = length - remaining;
				} else {
					tc_tf->seg_status.flag = SEG_ENDED;
					tc_tf->seg_status.octets_txed = 0;
				}
				return -TC_TX_DELAY;
			case ACCEPT_TX:
			case IGNORE:
				if (remaining == 0) {
					tc_tf->seg_status.flag = SEG_ENDED;
					tc_tf->seg_status.octets_txed = 0;
				} else {
					tc_tf->seg_status.flag = SEG_IN_PROGRESS;
					tc_tf->seg_status.octets_txed = length - remaining;
				}
				continue;
			case UNDEF_ERROR:
				tc_tf->seg_status.flag = SEG_ENDED;
				tc_tf->seg_status.octets_txed = 0;
				tc_tf->cop_cfg.fop.signal = UNDEF_ERROR;
				return -TC_TX_COP_ERR;
			default:
				tc_tf->seg_status.flag = SEG_ENDED;
				tc_tf->seg_status.octets_txed = 0;
				tc_tf->cop_cfg.fop.signal = UNDEF_ERROR;
				return -TC_TX_COP_ERR;
		}
	}
	tc_tf->cop_cfg.fop.signal = ACCEPT_TX;
	return TC_TX_OK;
}

void
osdlp_prepare_typea_data_frame(struct tc_transfer_frame *tc_tf, uint8_t *buffer,
                               uint16_t len, uint8_t mapid)
{
	tc_tf->primary_hdr.bypass = TYPE_A;
	tc_tf->primary_hdr.ctrl_cmd = TC_DATA;
	tc_tf->frame_data.data = buffer;
	tc_tf->frame_data.data_len = len;
	tc_tf->frame_data.seg_hdr.map_id = mapid;
}

void
osdlp_prepare_typeb_data_frame(struct tc_transfer_frame *tc_tf, uint8_t *buffer,
                               uint16_t len, uint8_t mapid)
{
	tc_tf->primary_hdr.bypass = TYPE_B;
	tc_tf->primary_hdr.ctrl_cmd = TC_DATA;
	tc_tf->frame_data.data = buffer;
	tc_tf->frame_data.data_len = len;
	tc_tf->frame_data.seg_hdr.map_id = mapid;
}

void
osdlp_prepare_typeb_setvr(struct tc_transfer_frame *tc_tf, uint8_t vr)
{
	tc_tf->primary_hdr.bypass = TYPE_B;
	tc_tf->primary_hdr.ctrl_cmd = TC_COMMAND;
	tc_tf->frame_data.seg_hdr.seq_flag = TC_UNSEG;
	tc_tf->mission.set_vr_cmd[2] = vr;
	tc_tf->frame_data.data = tc_tf->mission.set_vr_cmd;
	tc_tf->frame_data.data_len = 3;
}

void
osdlp_prepare_typeb_unlock(struct tc_transfer_frame *tc_tf)
{
	tc_tf->primary_hdr.bypass = TYPE_B;
	tc_tf->primary_hdr.ctrl_cmd = TC_COMMAND;
	tc_tf->frame_data.seg_hdr.seq_flag = TC_UNSEG;
	tc_tf->frame_data.data = &tc_tf->mission.unlock_cmd;
	tc_tf->frame_data.data_len = 1;
}

void
osdlp_prepare_clcw(struct tc_transfer_frame *tc_tf, uint8_t *ocf)
{
	tc_tf->mission.clcw.ctrl_word_type = 0; // TM_OCF_TYPE_1
	tc_tf->mission.clcw.cop_in_effect = 1;
	tc_tf->mission.clcw.farm_b_counter = tc_tf->cop_cfg.farm.farmb_cnt;
	tc_tf->mission.clcw.lockout = tc_tf->cop_cfg.farm.lockout;
	tc_tf->mission.clcw.rt = tc_tf->cop_cfg.farm.retransmit;
	tc_tf->mission.clcw.wait = tc_tf->cop_cfg.farm.wait;
	tc_tf->mission.clcw.report_value = tc_tf->cop_cfg.farm.vr;
	tc_tf->mission.clcw.vcid = tc_tf->mission.vcid;
	tc_tf->mission.clcw.clcw_version_num = 0;
	osdlp_clcw_pack(&tc_tf->mission.clcw, ocf);
}
