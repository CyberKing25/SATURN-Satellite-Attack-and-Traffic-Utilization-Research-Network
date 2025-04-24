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

#ifndef INCLUDE_TC_H_
#define INCLUDE_TC_H_

#include <stdbool.h>
#include <stdint.h>

#include "../osdlp/osdlp_clcw.h"

#define TC_VERSION_NUMBER           0
#define UNLOCK_CMD                  0
#define SETVR_BYTE1                 0x82
#define SETVR_BYTE2                 0

typedef enum {
	TYPE_A      = 0,
	TYPE_B      = 1
} tc_bypass_t;

typedef enum {
	TC_DATA     = 0,
	TC_COMMAND  = 1
} tc_ctrl_t;

/*State of reception of segmented FDUs*/
typedef enum {
	TC_LOOP_OPEN        = 0,
	TC_LOOP_CLOSED      = 1
} tc_loop_state_t;

typedef enum {
	TC_CRC_NOTPRESENT   = 0,
	TC_CRC_PRESENT      = 1
} tc_crc_flag_t;

typedef enum {
	TC_SEG_HDR_NOTPRESENT   = 0,
	TC_SEG_HDR_PRESENT      = 1
} tc_seg_hdr_t;

typedef enum {
	SEG_ENDED           = 0,
	SEG_IN_PROGRESS     = 1
} seg_status_t;

typedef enum {
	COP_OK              = 0,
	COP_ENQ             = 1,
	COP_PRIORITY_ENQ    = 2,
	COP_DISCARD         = 3,
	COP_ERROR           = 4
} farm_result_t;

typedef enum {
	TC_RX_OK               = 0,
	TC_RX_FRAME_LEN_ERR    = 1,
	TC_RX_CONFIG_ERR       = 2,
	TC_RX_FRAME_VAL_ERR    = 3,
	TC_RX_QUEUE_ERR        = 4,
	TC_RX_COP_ERR          = 5
} tc_rx_result_t;

typedef enum {
	TC_TX_OK               = 0,
	TC_TX_QUEUE_FULL_ERR   = 1,
	TC_TX_DELAY            = 2,
	TC_TX_COP_ERR          = 3
} tc_tx_result_t;

struct tc_primary_hdr {
	uint8_t             version_num     : 2;    /* TC version number*/
	uint8_t             bypass          : 1;    /* Bypass flag*/
	uint8_t             ctrl_cmd        : 1;    /* Control command*/
	uint8_t             rsvd_spare      : 2;    /* Raserved - spare*/
	uint16_t            spacecraft_id   : 10;   /* Spacecraft ID*/
	uint8_t             vcid            : 6;    /* Cirtual Channel ID*/
	uint16_t            frame_len       : 10;   /* Frame length (Total octets - 1)*/
	uint8_t             frame_seq_num;          /* Frame sequence number*/
};

/**
 * Sequence flag type
 */
typedef enum {
	TC_CONT_SEG     = 0,
	TC_FIRST_SEG    = 1,
	TC_LAST_SEG     = 2,
	TC_UNSEG        = 3
} tc_seq_flag_t;

struct tc_seg_hdr {
	uint8_t         seq_flag    : 2;    /* Sequence flag*/
	uint8_t         map_id      : 6;    /* MAP ID*/
};

/**
 * Frame data field struct
 */
struct	tc_fdf {
	struct tc_seg_hdr   seg_hdr;
	uint8_t             *data;
	uint16_t            data_len;
};

/**
 * Utility buffer for use in segmentation operations
 */
struct tc_util_buf {
	uint8_t             *buffer;
	uint16_t            buffered_length;
	uint8_t             loop_state;
};

/**
 * Mission specific parameters
 */
struct tc_mission_params {
	struct tc_util_buf  util;
	uint8_t             crc_flag;
	uint8_t             seg_hdr_flag;
	uint16_t            max_sdu_len;       /* Maximum size of higher layer frame*/
	uint16_t
	max_data_len;      /* Maximum size of data portion of packet*/
	uint16_t            max_frame_len;     /* Maximum size of frame*/
	uint16_t            rx_fifo_max_size;
	uint16_t            fixed_overhead_len;
	uint16_t            spacecraft_id;      /* Spacecraft ID*/
	uint8_t             vcid;               /* Cirtual Channel ID*/
	uint8_t             version_num;
	uint8_t             unlock_cmd;
	uint8_t             set_vr_cmd[3];
	struct clcw_frame   clcw;
};

/**
 * Segmentation status struct
 */
struct segment_status {
	uint8_t     flag;
	uint16_t    octets_txed;
};

struct farm_config {
	uint8_t     state;
	uint8_t     lockout;
	uint8_t     wait;
	uint8_t     retransmit;
	uint8_t     vr;
	uint8_t     farmb_cnt;
	uint8_t     w;
	uint8_t     pw;
	uint8_t     nw;
};

typedef enum {
	ACCEPT_DIR      = 0,
	REJECT_DIR      = 1,
	POSITIVE_DIR    = 2,
	NEGATIVE_DIR    = 3,
	SUSPEND         = 4,
	ALERT_LIMIT     = 5,
	ALERT_T1        = 6,
	ALERT_LOCKOUT   = 7,
	ALERT_SYNCH     = 8,
	ALERT_NNR       = 9,
	ALERT_CLCW      = 10,
	ALERT_LLIF      = 11,
	ALERT_TERM      = 12,
	ACCEPT_TX       = 13,
	REJECT_TX       = 14,
	POSITIVE_TX     = 15,
	NEGATIVE_TX     = 16,
	DELAY_RESP      = 17,
	UNDEF_ERROR     = 18,
	IGNORE          = 19,
	NA              = 20
} notification_t;

struct fop_config {
	uint8_t         state;
	uint8_t         vs;
	uint8_t         out_flag;
	uint8_t         nnr;
	uint16_t        t1_init;
	uint8_t         tx_lim;
	uint8_t         tx_cnt;
	uint8_t         slide_wnd;
	uint8_t         tt;
	uint8_t         ss;
	notification_t  signal;   /* This field is used as a way to signal
                              higher layers from the COP-1*/
};

struct cop_config {
	union {
		struct fop_config   fop;
		struct farm_config  farm;
	};
};

struct tc_transfer_frame {
	struct tc_primary_hdr       primary_hdr;    /* Primary header struct*/
	struct tc_mission_params    mission;        /* Mission params*/
	struct cop_config           cop_cfg;        /* COP configuration struct*/
	struct tc_fdf               frame_data;     /* Frame data structure*/
	struct segment_status       seg_status;     /* Segment status struct*/
	uint16_t                    crc;            /* CRC*/
};

int
osdlp_tc_init(struct tc_transfer_frame *tc_tf,
              uint16_t scid,
              uint16_t max_fdu_len,
              uint16_t max_frame_len,
              uint16_t rx_fifo_len,
              uint8_t vcid,
              uint8_t mapid,
              tc_crc_flag_t crc_flag,
              tc_seg_hdr_t seg_hdr_flag,
              tc_bypass_t bypass,
              tc_ctrl_t ctrl_cmd,
              uint8_t *util_buffer,
              struct cop_config cop);
/**
 * Unpacks a received buffer and populates the corresponding fields of
 * a TC config structure
 * @param frame_params the TC config struct
 * @param pkt_in the received packet buffer
 */
void
osdlp_tc_unpack(struct tc_transfer_frame *tc_tf,  uint8_t *pkt_in);

/**
 * Packs a TC structure into a buffer to be transmitted
 * @param frame_params the TC config struct
 * @param pkt_out the buffer where the packet will be placed
 * @param data_in the input data buffer
 * @param length the length of the data_in buffer
 */
void
osdlp_tc_pack(struct tc_transfer_frame *tc_tf, uint8_t *pkt_out,
              uint8_t *data_in, uint16_t length);

void
osdlp_prepare_typea_data_frame(struct tc_transfer_frame *tc_tf, uint8_t *buffer,
                               uint16_t len, uint8_t mapid);

void
osdlp_prepare_typeb_data_frame(struct tc_transfer_frame *tc_tf, uint8_t *buffer,
                               uint16_t len, uint8_t mapid);

void
osdlp_prepare_typeb_unlock(struct tc_transfer_frame *tc_tf);

void
osdlp_prepare_typeb_setvr(struct tc_transfer_frame *tc_tf, uint8_t vr);

void
osdlp_prepare_clcw(struct tc_transfer_frame *tc_tf, uint8_t *ocf);

int
osdlp_frame_validation_check(struct tc_transfer_frame *tc_tf,
                             uint8_t *rx_buffer);

/* Performs TC receive with COP
 *
 * @param the rx_buffer
 * @param the length of the frame
 *
 * @return the negative value of tc_rx_result_t for error, 0 for success
 *
 */
int
osdlp_tc_receive(uint8_t *rx_buffer, uint32_t length);


/* Performs TC transmit with COP
 *
 * @param the transfer frame config struct
 * @param the buffer containing the packet
 * @param the length of the frame
 *
 * @return the negative value of tc_tx_result_t for error, 0 for success
 *
 */
int
osdlp_tc_transmit(struct tc_transfer_frame *tc_tf, uint8_t *buffer,
                  uint32_t length);

/**
 * Returns the configuration struct for the specific vcid
 *
 * @param reference to the pointer to the transfer frame struct
 * @param the vcid
 *
 * @return error code. Negative for error, zero or positive for success
 */
__attribute__((weak))
int
osdlp_tc_get_rx_config(struct tc_transfer_frame **, uint16_t);

/**
 * Enqueues an item on the rx queue
 * @param the buffer to be enqueued
 * @param the length buffer to be enqueued
 * @param the vcid
 *
 * @return error code. Negative for error, zero or positive for success
 */
__attribute__((weak))
int
osdlp_tc_rx_queue_enqueue(uint8_t *, uint32_t, uint16_t);

/**
 * Enqueues an item on the rx queue even
 * if there is no space,
 * by deleting the tail item
 * @param the buffer to place the item
 * @param the length buffer to be enqueued
 * @param the vcid
 *
 * @return error code. Negative for error, zero or positive for success
 */
__attribute__((weak))
int
osdlp_tc_rx_queue_enqueue_now(uint8_t *, uint32_t, uint8_t);

#endif /* INCLUDE_TC_H_ */
