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
import inet.node.inet.WirelessHost;
import inet.physicallayer.wireless.common.antenna.IsotropicAntenna;
//
// TODO auto-generated module
//
module GroundStation extends WirelessHost
{
    parameters:
        @display("i=device/receiverdish;");
        //wlan[0].typename = "WirelessInterface";
        //wlan[0].radio.typename = "Radio";
        //wlan[0].radio.transmitter.typename = "GenericTransmitter";
        //wlan[0].radio.receiver.typename = "GenericReceiver";
        //wlan[0].radio.antenna.typename = "DirectionalAntenna";  // Use Directional Antenna
		
		string nodeType = default("GroundStation");
        double frequency @unit("GHz") = default(2.4 GHz); // S-band for Earth communication
        double transmitPower @unit("mW") = default(50 mW); // More powerful than satellites
        bool isTrackingSatellite = default(true);
}

