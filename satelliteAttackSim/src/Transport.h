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

#ifndef __SATELLITEATTACKSIM_TRANSPORT_H_
#define __SATELLITEATTACKSIM_TRANSPORT_H_

#include <omnetpp.h>
#include "./packets/TCPacket_m.h"
#include "packets/TMPacket_m.h"

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class Transport : public cSimpleModule
{
protected:
  virtual void initialize() override;
  virtual void handleMessage(cMessage *msg) override;

  void initializeTC_Packet();
  void initTC(TC_Packet *TC, bool encapsulate);
  void initializeTransferFrame_TC(struct tc_transfer_frame *tc_tf, TC_Packet *TC);
  uint8_t* EncapsulateTC(struct tc_transfer_frame *tc_tf, uint8_t *data, uint16_t length);
  void DecapsulateTC(TC_Packet *TC);
};

#endif
