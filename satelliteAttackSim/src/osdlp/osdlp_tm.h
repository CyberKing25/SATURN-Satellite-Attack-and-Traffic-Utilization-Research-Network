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

#include <stdint.h>
#include <stdbool.h>

#include "../osdlp/osdlp_crc.h"

#ifndef INCLUDE_TM_H_
#define INCLUDE_TM_H_

/**
 * The maximum allowed frame size
 */

#define TM_VERSION_NUMBER                   0
#define TM_SEC_HDR_VERSION_NUMBER           0
#define	TM_PRIMARY_HDR_LEN                  6
#define	TM_OCF_LENGTH                       4
#define TM_CRC_LENGTH                       2
#define	TM_FIRST_HDR_PTR_NO_PKT_START       0x07FF
#define TM_FIRST_HDR_PTR_OID                0x07FE
#define TM_IDLE_PACKET                      0x07 << 5

typedef enum {
	TM_OCF_NOTPRESENT 	= 0,
	TM_OCF_PRESENT 		= 1
} tm_ocf_flag_t;


typedef enum {
	TM_OCF_TYPE_1         = 0,
	TM_OCF_TYPE_2         = 1
} tm_ocf_type_t;


typedef enum {
	TM_CRC_NOTPRESENT 	= 0,
	TM_CRC_PRESENT 		= 1
} tm_crc_flag_t;

typedef enum {
	TM_SEC_HDR_NOTPRESENT   = 0,
	TM_SEC_HDR_PRESENT      = 1
} tm_sec_hdr_flag_t;

typedef enum {
	TYPE_OS_ID      = 0,
	TYPE_VCA_SDU    = 1
} tm_sync_flag_t;

/*State of reception of segmented FDUs*/
typedef enum {
	TM_LOOP_OPEN		= 0,
	TM_LOOP_CLOSED		= 1
} tm_loop_state_t;

typedef enum {
	TM_STUFFING_ON      = 0,
	TM_STUFFING_OFF     = 1
} tm_stuff_state_t;

typedef enum {
	TM_RX_OK            = 0,
	TM_RX_PENDING       = 1,
	TM_RX_ERROR         = 2,
	TM_RX_OID           = 3,
	TM_RX_DENIED        = 4,
	TM_RX_WRONG_CRC     = 5
} tm_rx_result_t;

struct tm_master_channel_id {
	uint8_t		version_num     : 2;
	uint16_t	spacecraft_id   : 10;
};

/**
 * Data field status struct
 */
struct tm_df_status {
	uint8_t					sec_hdr         : 1;
	uint8_t					sync            : 1;
	uint8_t					pkt_order       : 1;
	uint8_t					seg_len_id      : 2;
	uint16_t				first_hdr_ptr   : 11;
};

/**
 * Data field status of primary header
 */
struct tm_primary_hdr {
	struct tm_master_channel_id     mcid;
	uint8_t                         vcid            : 3;
	uint8_t                         ocf             : 1;
	uint8_t
	*mc_frame_cnt;			// This field needs to be shared among all vcs
	uint8_t                         vc_frame_cnt;
	struct tm_df_status             status;
};

/**
 * Secondary header ID struct
 */
struct tm_sec_hdr_id {
	uint8_t		version_num : 2;
	uint8_t		length      : 6;
};

/**
 * Secondary header struct
 */
struct tm_sec_hdr {
	struct tm_sec_hdr_id       sec_hdr_id;
	uint8_t                    *sec_hdr_data_field;
};

/**
 * Mission parameters struct
 */

struct tm_util_buf {
	uint8_t            *buffer;
	uint16_t            buffered_length;
	uint8_t             loop_state;
	uint16_t            expected_pkt_len;
};

struct tm_mission_params {
	struct tm_util_buf          util;
	uint16_t                    frame_len;			/* Fixed frame length*/
	uint16_t
	header_len;			/* Header length - Including secondary if present*/
	uint16_t
	max_data_len;		/* Maximum allowed data size per FDU*/
	uint16_t
	max_sdu_len;       /* Maximum size of higher layer frame*/
	uint8_t                     crc_present;		/* CRC present flag*/
	uint16_t                    max_vcs;			/* Max number oc VCs allowed*/
	uint16_t                    tx_fifo_max_size;	/* Max TX FIFO capacity*/
	uint8_t                     vcid;
	uint8_t                     stuff_state;
	uint8_t                     ocf_type;
};

struct tm_transfer_frame {
	struct tm_primary_hdr       primary_hdr;        /* The primary header struct*/
	struct tm_sec_hdr           secondary_hdr;      /* The secondary header struct*/
	uint8_t                     ocf[4];             /* The OCF field */
	uint16_t                    crc;                /* CRC value*/
	uint8_t                     *data;              /* Pointer to FDU*/
	struct tm_mission_params    mission;            /* Mission specific parameters*/
};

/**
 * Initializes the TM config structure
 * @param spacecraft_id the spacecraft ID
 * @param mc_count the master channel count. Must be common
 * for all TM structures of different VCs
 * @param vcid the virtual channel ID
 * @param ocf_flag flag for the presence or not of OCF
 * @param sec_hdr_fleg flag for the presence or not of
 * the secondary header
 * @param sync_flag sync flag
 * @param sec_hdr_len length of secondary header if present
 * @param sec_hdr pointer to the secondary header structure
 * if present
 * @param OCF value
 * @param crc_flag flag for the presence or not of CRC
 * @param frame_len Fixed frame length
 * @param max_vcs Max number oc VCs allowed
 * @param stuffing Indicates if packet stuffing is on
 * @param util_buffer a utility buffer with size MAX_SDU_SIZE
 */
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
              uint8_t *util_buffer);

/**
 * Packs a TM structure into a buffer to be transmitted
 * @param frame_params the TM config struct
 * @param pkt_out the buffer where the packet will be placed
 * @param data_in the input data buffer
 * @param length the length of the data_in buffer
 */
void
osdlp_tm_pack(struct tm_transfer_frame *frame_params, uint8_t *pkt_out,
              uint8_t *data_in, uint16_t length);

/**
 * Unpacks a received buffer and populates the corresponding fields of
 * a TM config structure
 * @param frame_params the TM config struct
 * @param pkt_in the received packet buffer
 */
void
osdlp_tm_unpack(struct tm_transfer_frame *frame_params, uint8_t *pkt_in);

int
osdlp_tm_transmit(struct tm_transfer_frame *tm_tf,
                  uint8_t *data_in, uint16_t length);

int
osdlp_tm_receive(uint8_t *data_in);

/**
 * Transmits an FDU with idle packets only
 */
int
osdlp_tm_transmit_idle_fdu(struct tm_transfer_frame *tm_tf, uint8_t vcid);

/**
 * Aborts an ongoing segmentation procedure
 */
void
osdlp_abort_segmentation(struct tm_transfer_frame *tm_tf);

/**
 * Disables the stuffing of packets into FDUs
 */
void
osdlp_disable_packet_stuffing(struct tm_transfer_frame *tm_tf);

/**
 * Enables the stuffing of packets into FDUs
 */
void
osdlp_enable_packet_stuffing(struct tm_transfer_frame *tm_tf);

/**
 * Checks if tx queue is empty
 * @param the vcid
 */
__attribute__((weak))
bool
osdlp_tm_tx_queue_empty(uint8_t);

/**
 * Returns a pointer to the last element of the TX queue
 * @param reference to the pointer where packet will be stored
 * @param the vcid
 * @return error code. Negative for error, zero or positive for success
 */
__attribute__((weak))
int
osdlp_tm_tx_queue_back(uint8_t **, uint8_t);

/**
 * Notifies the caller that operations on the last element
 * of the TX queue are over
 * @param the vcid
 */
__attribute__((weak))
void
osdlp_tm_tx_commit_back(uint8_t);

/**
 * Puts an item at the back of the TX queue
 * @param the vcid
 */
__attribute__((weak))
int
osdlp_tm_tx_queue_enqueue(uint8_t *, uint8_t);

/**
 * Puts an item at the back of the RX queue
 * @param pointer to the memory space of the packet
 * @param the vcid
 */
__attribute__((weak))
int
osdlp_tm_rx_queue_enqueue(uint8_t *, uint8_t);

/**
 * Returns the length of the packet pointed to by
 * the pointer
 * @param pointer to the variable where the length
 * will be stored
 * @param the pointer to the packet
 * @param the length of the memory space pointed
 * to by the pointer where the user is allowed to
 * search for the packet length field
 *
 * @return error code. Negative for error, zero or positive for success
 */
__attribute__((weak))
int
osdlp_tm_get_packet_len(uint16_t *, uint8_t *, uint16_t);

/**
 * gets the TM config corresponding to the vcid
 * @param the vcid
 */
__attribute__((weak))
int
osdlp_tm_get_rx_config(struct tm_transfer_frame **, uint8_t);

#endif /* INCLUDE_TM_H_ */
