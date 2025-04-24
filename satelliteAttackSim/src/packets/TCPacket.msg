//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
packet TC_Packet {
    int scid;          // Spacecraft ID
    int max_fdu_len;   // Maximum Frame Data Unit Length
    int max_frame_len; // Maximum Frame Length
    int rx_fifo_len;   // Receive FIFO Length
    int vcid;          // Virtual Channel ID
    bool crc_flag;     // CRC Enabled
    bool seg_hdr_flag; // Segmentation Header Flag
    bool bypass;       // Bypass Flag
    bool ctrl_cmd;     // Control Command Flag
    uint8_t util_buffer[]; // Buffer Data
}