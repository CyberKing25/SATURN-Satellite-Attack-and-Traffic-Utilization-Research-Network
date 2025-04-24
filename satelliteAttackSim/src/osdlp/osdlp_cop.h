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

#ifndef INCLUDE_OSDLP_COP_H_
#define INCLUDE_OSDLP_COP_H_

#include <stdint.h>
#include <stdbool.h>

#include "../osdlp/osdlp_tc.h"

#define UNLOCK_CMD      0
#define SETVR_BYTE1     0x82
#define SETVR_BYTE2     0

typedef enum {
	FARM = 0,
	FOP  = 1
} cop_type_t;

typedef enum {
	FARM_STATE_OPEN         = 0,
	FARM_STATE_WAIT         = 1,
	FARM_STATE_LOCKOUT      = 2
} farm_state_t;

typedef enum {
	RT_FLAG_OFF = 0,
	RT_FLAG_ON  = 1
} rt_flag_t;

typedef enum {
	FOP_STATE_ACTIVE        = 0,
	FOP_STATE_RT_NO_WAIT    = 1,
	FOP_STATE_RT_WAIT       = 2,
	FOP_STATE_INIT_NO_BC    = 3,
	FOP_STATE_INIT_BC       = 4,
	FOP_STATE_INIT          = 5
} fop_state_t;

/**
 * The struct used to store the sent packets.
 * structs of this type are stored in the sent
 * queue.
 */
struct queue_item {
	uint8_t     *fdu;       /* Pointer to the packet*/
	uint8_t     rt_flag;    /* Retransmit flag*/
	uint8_t     seq_num;    /* Sequence number of the packet*/
	tc_bypass_t type;       /* Type_A or Type_B*/
};

struct farm_vars {
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

/**
 * Prepares the FOP config struct
 * @param fop the fop handle
 * @param slide_wnd the slide window
 * @param state the initial state of the machine
 * @param t1_init the timer's initial value
 * @param timeout_type the timeout type
 * @param tx_lim the frame retransmission limit
 */
void
osdlp_prepare_fop(struct fop_config *fop, uint16_t slide_wnd,
                  fop_state_t state, uint16_t t1_init, uint8_t timeout_type,
                  uint8_t tx_lim);

/**
 * Prepares the FARM config struct
 * @param farm the farm handle
 * @param state the initial state of the machine
 * @param window_width the window width
 */
void
osdlp_prepare_farm(struct farm_config *farm, farm_state_t state,
                   uint8_t window_width);

int
osdlp_initialize_cop(struct tc_transfer_frame *tc_tf);

int
osdlp_alert(struct tc_transfer_frame *tc_tf);

int
osdlp_resume(struct tc_transfer_frame *tc_tf);

int
osdlp_reset_accept(struct tc_transfer_frame *tc_tf);

farm_result_t
osdlp_farm_1(struct tc_transfer_frame *tc_tf);

void
osdlp_set_lockout(struct farm_config *farm_cfg);

void
osdlp_reset_lockout(struct farm_config *farm_cfg);

void
osdlp_set_wait(struct farm_config *farm_cfg);

void
osdlp_reset_wait(struct tc_transfer_frame *tc_tf);

void
osdlp_buffer_release(struct tc_transfer_frame *tc_tf);

void
osdlp_set_retransmit(struct farm_config *farm_cfg);

void
osdlp_reset_retransmit(struct farm_config *farm_cfg);

int
osdlp_fop_1(struct tc_transfer_frame *tc_tf);

int
osdlp_purge_sent_queue(struct tc_transfer_frame *tc_tf);

int
osdlp_purge_wait_queue(struct tc_transfer_frame *tc_tf);

notification_t
osdlp_look_for_fdu(struct tc_transfer_frame *tc_tf);

int
osdlp_transmit_type_ad(struct tc_transfer_frame *tc_tf);

int
osdlp_transmit_type_bc(struct tc_transfer_frame *tc_tf);

int
osdlp_transmit_type_bd(struct tc_transfer_frame *tc_tf);

int
osdlp_initiate_ad_retransmission(struct tc_transfer_frame *tc_tf);

int
osdlp_initiate_bc_retransmission(struct tc_transfer_frame *tc_tf);

int
osdlp_remove_acked_frames(struct tc_transfer_frame *tc_tf, uint8_t nr);

notification_t
osdlp_look_for_directive(struct tc_transfer_frame *tc_tf);

notification_t
osdlp_handle_clcw(struct tc_transfer_frame *tc_tf, uint8_t *ocf_buffer);

notification_t
osdlp_handle_timer_expired(struct tc_transfer_frame *tc_tf);

notification_t
osdlp_req_transfer_fdu(struct tc_transfer_frame *tc_tf);

int
osdlp_release_copy_of_bc(struct tc_transfer_frame *tc_tf);


/* Directives from management */

notification_t
osdlp_initiate_no_clcw(struct tc_transfer_frame *tc_tf);

notification_t
osdlp_initiate_with_clcw(struct tc_transfer_frame *tc_tf);

notification_t
osdlp_initiate_with_unlock(struct tc_transfer_frame *tc_tf);

notification_t
osdlp_initiate_with_setvr(struct tc_transfer_frame *tc_tf, uint8_t new_vr);

notification_t
osdlp_terminate_ad(struct tc_transfer_frame *tc_tf);

notification_t
osdlp_resume_ad(struct tc_transfer_frame *tc_tf);

notification_t
osdlp_set_new_vs(struct tc_transfer_frame *tc_tf, uint8_t new_vs);

notification_t
osdlp_set_sliding_window(struct tc_transfer_frame *tc_tf, uint8_t new_wnd);

notification_t
osdlp_set_timer(struct tc_transfer_frame *tc_tf, uint16_t new_t1);

notification_t
osdlp_set_tx_limit(struct tc_transfer_frame *tc_tf, uint8_t new_lim);

notification_t
osdlp_set_tt(struct tc_transfer_frame *tc_tf, uint8_t new_tt);

/* Responses from lower layers */

notification_t
osdlp_ad_accept(struct tc_transfer_frame *tc_tf);

notification_t
osdlp_ad_reject(struct tc_transfer_frame *tc_tf);

notification_t
osdlp_bc_accept(struct tc_transfer_frame *tc_tf);

notification_t
osdlp_bc_reject(struct tc_transfer_frame *tc_tf);

notification_t
osdlp_bd_accept(struct tc_transfer_frame *tc_tf);

notification_t
osdlp_bd_reject(struct tc_transfer_frame *tc_tf);


/* Queue API */

/**
 * Enqueues an item on the wait queue
 * @param the buffer to enqueue
 * @param the vcid
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_tc_wait_queue_enqueue(void *, uint16_t);

/**
 * Dequeues an item from the wait queue
 * @param the buffer to hold the item
 * @param the vcid
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_tc_wait_queue_dequeue(void *, uint16_t);

/**
 * Returns if the wait queue is empty
 * @param the vcid
 *
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
bool
osdlp_tc_wait_queue_empty(uint16_t);

/**
 * Clears the wait queue
 * @param the vcid
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_tc_wait_queue_clear(uint16_t);

/**
 * Enqueues an item on the sent queue
 * @param the queue_item to enqueue
 * @param the vcid
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_tc_sent_queue_enqueue(struct queue_item *, uint16_t);


/**
 * Dequeues an item from the sent queue
 * @param the queue_item to hold the item
 * @param the vcid
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_tc_sent_queue_dequeue(struct queue_item *, uint16_t);

/**
 * Returns if the sent queue is empty
 * @param the vcid
 */
__attribute__((weak))
bool
osdlp_tc_sent_queue_empty(uint16_t);

/**
 * Clears the sent queue
 * @param the vcid
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_tc_sent_queue_clear(uint16_t);

/**
 * Returns a pointer to the head of the sent
 * queue.
 * @param the pointer to the item of the queue
 * @param the vcid
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_tc_sent_queue_head(struct queue_item *, uint16_t);


/**
 * Checks if rx queue is full
 * @param the vcid
 */
__attribute__((weak))
bool
osdlp_tc_rx_queue_full(uint16_t);

/**
 * Checks if tx queue is full
 * @param the vcid
 */
__attribute__((weak))
bool
osdlp_tc_tx_queue_full();

/**
 * Enqueues an item on the tx queue.
 * This can be used as a means for
 * the higher layers to stop the flow
 * of packets if some error occurs.
 * @param the buffer to be enqueued
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_tc_tx_queue_enqueue(uint8_t *, uint16_t);

/**
 * Starts the timer
 * @param the vcid
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_timer_start(uint16_t);

/**
 * Stops the timer
 * @param the vcid
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_timer_cancel(uint16_t);

/**
 * Cancels any ongoing operations on the
 * lower levels, eg purge the tx queue.
 * This is used to facilitate retransmissions
 *
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_cancel_lower_ops();

/**
 * Marks all type_A queue_item structs on the
 * sent queue as 'to be retransmitted'
 * @param the vcid
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_mark_ad_as_rt(uint16_t);

/**
 * Marks the type_B queue_item struct on the
 * sent queue as 'to be retransmitted'
 * @param the vcid
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_mark_bc_as_rt(uint16_t);

/**
 * Resets the flag of the passed queue_item on
 * the sent queue
 * @param the queue_item to be reset
 * @param the vcid
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_reset_rt_frame(struct queue_item *, uint16_t);

/**
 * Fetches the first frame on the sent queue
 * with the retransmit flag on.
 * @param the queue_item that will be fetched
 * @param the vcid
 * @return 0 or positive value for success, negative value otherwise
 */
__attribute__((weak))
int
osdlp_get_first_ad_rt_frame(struct queue_item *, uint16_t);

#endif /* INCLUDE_OSDLP_COP_H_ */
