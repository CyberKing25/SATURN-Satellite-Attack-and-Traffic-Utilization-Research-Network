# A simulation module for simulating cyberattacks towards LEO satellite constellations

SATURN (Satellite Attack and Traffic Utilization Research Network) is a low earth orbit (LEO) satellite network cyberattack simulation framework. It works as a module for OMNeT++, and provides two simulation scenarios based on the cyberattacks, Distributed-Denial-of-Service (DDoS) and Malware injection. The framework was developed for OMNeT++ 6.1 and INET version 4.5.

## Installation

1. Install [OMNeT++ 6.1](https://omnetpp.org/download/)
2. Launch omnetpp either from the terminal with `omnetpp` or from the executable, and choose a workspace for the project. Default is `/omnetpp-6.1/samples`.
3. Go to **Help >> Install Simulation Models...** menu and install *INETv4.5.4* in the workspace.
4. Clone `https://github.com/CyberKing25/satellite-attack-sim` into the workspace.
5. In omnetpp, go to **File >> Open Projects from File System** and add *satelliteAttackSim* to the workspace.
6. Right-click *satelliteAttackSim* project and go to *Properties*, under *Project References* select only inet4.5.
7. Right-click *satelliteAttackSim* and click *Build Project*.
8. Finally, right-click *satelliteAttackSim* and click *Run As >> OMNeT++ Simulation*.

## Execution

This framework is made up of several different cyberattack simulation scenarios, where the two attack types, DoS and Malware Injection, are simulated towards a LEO satellite network. After performing step 8. under Installation, a new window will be opened, this is the Qtenv, which is the simulation environment in OMNeT++. When this environment is opened, the following image will be shown:![image](https://github.com/user-attachments/assets/3911c324-13e7-4a19-85c6-4769c576fb3e)

By clicking on the drop-down menu titled **Config name**, you can choose the wanted scenario. The options are:
- DDoS-base - A DDoS attack where the attack nodes send DoS traffic every 50 ms.
- DDoS-120ms - A DDoS attack where the attack nodes send DoS traffic every 120 ms.
- DDoS-20ms - A DDoS attack where the attack nodes send DoS traffic every 20 ms.
- DDoS-1ms - A DDoS attack where the attack nodes send DoS traffic every 1 ms.
  
- Malware-base - An attack where the attack node sends an abnormally large message (10KB) towards one of the satellites in the network, through the ground station.
- Malware-30KB - A malware injection attack where the attack node sends a malicious packet which has a size of 30KB.
- Malware-45KB - A malware injection attack where the attack node sends a malicious packet which has a size of 45KB.
- Malware-max - A malware injection attack where the attack node sends a malicious packet which has a size of 65527B, the largest available packet size.

When the DDoS scenarios are selected in the drop-down menu mentioned above, the environment will ask for the number of attacker and satellite nodes that is going to be used in the attack. The malware scenarios however, are always set to use 1 attacker node. 

![image](https://github.com/user-attachments/assets/737fbd28-b1a6-4282-be34-5db96d8b2403)

When the number of attacker nodes are filled in, the number of satellites will be asked for. The Malware Injection scenario will not ask for the number of attacker nodes, only the number of satellites. When this number is input, the simulation will be ready to be run. Click the green **play** button in the upper left to start the simulation.


## Instructional Video

An instructional video is available [here](https://youtu.be/NVk7GdAgObU). This video shows how the SATURN framework is used, and how different metrics like energy consumption can be fetched from the OMNeT++ analysis.

## Citing

If you feel our paper and code is helpful, please consider citing our paper by:

```
@inproceedings{SATURN,
  title={SATURN: A Tool for Advancing Satellite Constellation Security through Attack Simulation and Time Series Data Generation},
  author={Hyldmo, Martin and Jensen, Mats Dimitri},
  booktitle={ADD_JOURNAL},
  year={2025}
}
```




