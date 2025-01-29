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

package satelliteattacksim.simulations;
import satelliteattacksim.Satellite;

network SatelliteNetwork
{
    @display("bgb=185,211");
    submodules:
        satellite1: Satellite {
            @display("p=31,30");
        }
        satellite2: Satellite {
            @display("p=149,30");
        }
    connections:
        satellite1.pppg++ <--> PointToPointLink <--> satellite2.pppg++;


}


channel PointToPointLink extends ned.DatarateChannel
{
    delay = 300ms; // Simulate satellite link delay
    datarate = 10Mbps; // Bandwidth
    ber = 1e-6; // Bit error rate
}