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

#include "Transport.h"

extern "C" {
#include "./osdlp/osdlp.h"
}


Define_Module(Transport);

void Transport::initialize()
{
    sendRate = par("sendRate").doubleValue();
    cMessage start = new cMessage("Start");
    scheduleAt(sendRate, start);
}

void Transport::handleMessage(cMessage *msg)
{
    if(msg->isSelfMessage()){
        initializeTC_Packet();
        simtime_t t = sendRate + simTime();
        scheduleAt(t, "send");
    }
    else {
        TC_Packet *tcPacket = check_and_cast<TC_Packet*>(msg);
        DecapsulateTC(tcPacket);
    }


}

/**
 * @brief Initializes and encapsulates a TC packet, then sends it.
 *
 * This function creates a TC_Packet object, initializes a TC transfer frame,
 * encapsulates the packet, and sends it through the "out" gate.
 *
 * @note The function dynamically allocates memory for TC_Packet and deletes
 *       the initial TC object after processing.
 *
 *
 * @see initializeTransferFrame_TC()
 * @see EncapsulateTC()
 */
void Transport::initializeTC_Packet() {
    struct tc_transfer_frame tc_tf;
    TC_Packet *TC = new TC_Packet();

    initializeTransferFrame_TC(&tc_tf, TC);


    uint8_t *TCPK = (uint8_t*) TC->getUtil_buffer();
    uint8_t *data = EncapsulateTC(&tc_tf, TCPK, TC->getUtil_bufferArraySize());
    TC_Packet *packet = new TC_Packet("TC_Packet");

    int length = TC->getUtil_bufferArraySize();
//    char* test = reinterpret_cast<char*>(data);
    packet->setUtil_bufferArraySize(length);
    for(int i = 0; i < length; i++) {
            packet->setUtil_buffer(i, data[i+6]);
    }
    send(packet, "out");

    delete TC;
}

/**
 *
 */
void Transport::initializeTransferFrame_TC(struct tc_transfer_frame *tc_tf, TC_Packet *TC){
    struct cop_config cop_cfg = {};
    initTC(TC, true);

    osdlp_tc_init(tc_tf, TC->getScid(), TC->getMax_fdu_len(),
                    TC->getMax_frame_len(), TC->getRx_fifo_len(),
                    TC->getVcid(), 0, (tc_crc_flag_t) TC->getCrc_flag(),
                    (tc_seg_hdr_t) TC->getSeg_hdr_flag(), (tc_bypass_t) TC->getBypass(),
                    (tc_ctrl_t) TC->getCtrl_cmd(), TC->getUtil_buffer(), cop_cfg
    );

    EV << "TC-Packet for satellite " << TC->getScid() << " prepared for transmission.\n";
}


void Transport::initializeTM_Packet(){

}



uint8_t* Transport::EncapsulateTC(struct tc_transfer_frame *tc_tf, uint8_t *data, uint16_t length){
    uint8_t *pack = new uint8_t[length];
    osdlp_tc_pack(tc_tf, pack, data, length);
    return pack;
}


void Transport::DecapsulateTC(TC_Packet *TC){
    struct tc_transfer_frame tc_tf;
        uint8_t* util_buffer = TC->getUtil_buffer();
        struct cop_config cop_cfg = {};
        initTC(TC, false);
        // Initialize the TC transfer frame
        osdlp_tc_init(&tc_tf, TC->getScid(), TC->getMax_fdu_len(),
                TC->getMax_frame_len(), TC->getRx_fifo_len(),
                TC->getVcid(), 0, (tc_crc_flag_t) TC->getCrc_flag(),
                (tc_seg_hdr_t) TC->getSeg_hdr_flag(), (tc_bypass_t) TC->getBypass(),
                (tc_ctrl_t) TC->getCtrl_cmd(), util_buffer, cop_cfg
        );
        EV_INFO << "before unpacking data length: " << tc_tf.frame_data.data_len << endl;

        // Unpack the received packet
        osdlp_tc_unpack(&tc_tf, util_buffer);

        EV_INFO << "Unpacked data length: " << tc_tf.frame_data.data_len << endl;
}

void Transport::initTC(TC_Packet *TC, bool encapsulate){
    TC->setScid(par("scid"));
    TC->setMax_fdu_len(par("max_fdu_len"));
    TC->setMax_frame_len(par("max_frame_len"));
    TC->setRx_fifo_len(par("rx_fifo_len"));
    TC->setVcid(par("vcid"));
    TC->setCrc_flag(par("crc_flag").boolValue());
    TC->setSeg_hdr_flag(par("seg_hdr_flag").boolValue());
    TC->setBypass(par("bypass").boolValue());
    TC->setCtrl_cmd(par("ctrl_cmd").boolValue());

    if(encapsulate) {
        std::string str = "TC_PACKAGE";
        std::vector<uint8_t> uint8_array(str.begin(), str.end());
        TC->setUtil_bufferArraySize(uint8_array.size());
        for(int i = 0; i < uint8_array.size(); i++) {
            TC->setUtil_buffer(i, uint8_array[i]);
        }
    }
}
