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
import satelliteattacksim.Attacker;
import satelliteattacksim.GroundStation;
import inet.physicallayer.wireless.common.medium.RadioMedium;
import inet.node.inet.WirelessHost;
import inet.node.wireless.AccessPoint;
import inet.mobility.static.StaticLinearMobility;
import inet.mobility.single.CircleMobility;
import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.physicallayer.wireless.common.medium.RadioMedium;
import inet.physicallayer.wireless.apsk.packetlevel.ApskScalarRadioMedium;
import inet.physicallayer.wireless.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.visualizer.canvas.integrated.IntegratedCanvasVisualizer;
import inet.linklayer.ieee80211.mgmt.Ieee80211AgentSta;

network SatelliteNetwork
{
    parameters:
        int numAttackers;
        int numSats;
        @figure[rcvdPkText](type=indicatorText; pos=380,20; anchor=w; font=,18; textFormat="packets received: %g"; initialValue=0);
        @statistic[packetReceived](source=groundStation.app[0].packetReceived; record=figure(count); targetFigure=rcvdPkText);
    submodules:
        visualizer: IntegratedCanvasVisualizer {
            @display("p=580,125");
        }
        configurator : Ipv4NetworkConfigurator;
        radioMedium : Ieee80211ScalarRadioMedium;
        groundStation : GroundStation;
        satellite[numSats] : Satellite {
        	parameters:
        	    startDegree = index * 10 deg;
        };
        	
        attacker[numAttackers] : Attacker;
        //accessPoint : AccessPoint;
}
