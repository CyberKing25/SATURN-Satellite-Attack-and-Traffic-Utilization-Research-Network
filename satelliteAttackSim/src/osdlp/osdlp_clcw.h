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

#ifndef INCLUDE_OSDLP_CLCW_H_
#define INCLUDE_OSDLP_CLCW_H_

#include <stdint.h>

typedef enum {
	CLCW_PL_AVAILABLE       = 0,
	CLCW_PL_NOT_AVAILABLE   = 1
} rf_available_flag;

typedef enum {
	CLCW_BIT_LOCK_ACHIEVED      = 0,
	CLCW_BIT_LOCK_NOT_ACHIEVED  = 1
} bit_lock_flag;

typedef enum {
	CLCW_NO_LOCKOUT = 0,
	CLCW_LOCKOUT    = 1
} lockout_flag;

typedef enum {
	CLCW_DO_NOT_WAIT    = 0,
	CLCW_WAIT           = 1
} wait_flag;

typedef enum {
	CLCW_NO_RETRANSMIT  = 0,
	CLCW_RETRANSMIT     = 1
} retransmit_flag;

struct clcw_frame {
	uint8_t             ctrl_word_type      : 1;
	uint8_t             clcw_version_num    : 2;
	uint8_t             status_field        : 3;
	uint8_t             cop_in_effect       : 2;
	uint8_t             vcid                : 6;
	uint8_t             rsvd_spare1         : 2;
	uint8_t             rf_avail            : 1;
	uint8_t             bit_lock            : 1;
	uint8_t             lockout             : 1;
	uint8_t             wait                : 1;
	uint8_t             rt                  : 1;
	uint8_t             farm_b_counter      : 2;
	uint8_t             rsvd_spare2         : 1;
	uint8_t             report_value;
};

void
osdlp_clcw_pack(struct clcw_frame *clcw_params, uint8_t *buffer);

void
osdlp_clcw_unpack(struct clcw_frame *clcw_params, uint8_t *buffer);

#endif /* INCLUDE_OSDLP_CLCW_H_ */
