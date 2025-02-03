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
import inet.mobility.single.CircleMobility;

//
// TODO auto-generated module
//
module Satellite extends WirelessHost {
    parameters:
        @display("i=device/satellite;");
    	string nodeType = default("Satellite");
    	string orbitNormal = default(""); 
    	double altitude @unit("km") = default(2000km);
        double frequency @unit("GHz") = default(1 GHz); // RF Frequency
        double transmitPower @unit("mW") = default(10 mW);
        double startingPhase @unit("deg") = default(0deg);
        //double mobilitySpeed @unit("m/s") = default(7500 m/s); // Approximate LEO speed
        
}
