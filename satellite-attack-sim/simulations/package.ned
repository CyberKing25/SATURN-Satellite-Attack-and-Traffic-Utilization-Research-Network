package satellite_attack_sim.simulations;


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

}


@license(LGPL);
