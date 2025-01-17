package satelliteAttackSim.simulations;

import inet.node.inet.StandardHost;
import inet.node.inet.Router;
import inet.mobility.single.LinearMobility;
import inet.physicallayer.contract.packetlevel.IRadioMedium


module SatelliteNode {

	gates:
	    input receiver;
	    output transmitter;
}

module GroundStation {

	gates:
	    input receiver;
	    output transmitter;
}

network SatelliteCommunicationNetwork {
    
    types:
        channel RadioLink  RadioMedium {
        
        } 

	submodules:

		satellite1: SatelliteNode {
			@display("i=device/satellite");
		}

		satellite2: SatelliteNode {
			@display("i=device/satellite");
		}

		groundStation1: GroundStation;

		groundStation2: GroundStation;

	connections:

		satellite1.transmitter --> RadioLink --> groundStation1.receiver;
		
		satellite1.transmitter --> RadioLink --> groundStation2.receiver;
		
		satellite1.transmitter --> RadioLink --> satellite2.receiver;
		
		satellite1.receiver <-- RadioLink <-- satellite2.transmitter;
		
		satellite1.receiver <-- RadioLink <-- groundStation1.transmitter;
		
		satellite1.receiver <-- RadioLink <-- groundStation2.transmitter;

		satellite2.transmitter --> RadioLink --> groundStation1.receiver;
		
		satellite2.transmitter --> RadioLink --> groundStation2.receiver;
		
		satellite2.transmitter --> RadioLink --> satellite2.receiver;
		
		satellite2.receiver <-- RadioLink <-- satellite2.transmitter;
		
		satellite2.receiver <-- RadioLink <-- groundStation1.transmitter;
		
		satellite2.receiver <-- RadioLink <-- groundStation2.transmitter;

}



@license(LGPL);
