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

package satelliteattacksim;

//
// TODO auto-generated module
//
simple Transport {
    parameters:
        @class(Transport);
        volatile double sendRate @unit(s) = 0.7s; 
        int scid = parent.scid;
        int vcid; // Virtual Channel ID
        int max_fdu_len; // Maximum FDU length
        int max_frame_len; // Maximum frame length
        int rx_fifo_len; // RX FIFO length
        bool crc_flag; // CRC flag
        bool seg_hdr_flag; // Segmentation header flag
        bool bypass; // Bypass flag
        bool ctrl_cmd; // Control command flag
    gates:
        input in;
        output out;
}
