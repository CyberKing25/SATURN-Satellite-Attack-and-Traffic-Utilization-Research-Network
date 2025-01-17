package satelliteAttackSim.simulations;

import inet.node.inet.StandartHost;
import inet.node.inet.Router;
import inet.mobility.single.ConstSpeedMobility;
import inet.physicallayer.common.packetlevel.RadioMedium;


network SatelliteNetwork {

	parameters:
		int numSatellites = default(2);
		int numGroundStations = default(1);
		
	submodules:
		radioMedium: RadioMedium {
		
			@display("p=100,100");
			
		}
	
		groundStation[numGroundStations]: StandardHost {

			@display(“p=200,300+100*i”);

		}

		satellite[numSatellites]: StandardHost {

			@display(“p=400,100+100*i”);

			mobility.typename = “ConstSpeedMobility”;

		}

	connections allowunconnected:

		for i=0..numGroundStations-1 {
			groundStation[i].wlan[0] <--> radioMedium <--> satellite[0].wlan[0];
		}
		
		for i=0..numSatellites-1 {
			satellite[i].wlan[0] <--> radioMedium <--> satellite[(i+1) % numSatellites].wlan[0];
		}

}



@license(LGPL);
