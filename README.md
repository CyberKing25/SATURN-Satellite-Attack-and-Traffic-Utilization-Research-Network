# A simulation module for simulating cyberattacks towards LEO satellite constellations

[NAME_OF_TOOL] is a low earth orbit (LEO) satellite network cyberattack simulation framework. It works as a module for OMNeT++, and provides two simulation scenarios based on the cyberattacks, Distributed-Denial-of-Service (DDoS) and Malware injection. The framework was developed for OMNeT++ 6.1 and INET version 4.5.

## Installation

1. Install [OMNeT++ 6.1](https://omnetpp.org/download/)
2. Launch omnetpp either from the terminal with `omnetpp` or from the executable, and choose a workspace for the project. Default is `/omnetpp-6.1/samples`.
3. Go to **Help >> Intall Simulation Models...** menu and install *INETv4.5.4* in the workspace.
4. Clone `https://github.com/CyberKing25/satellite-attack-sim` into the workspace.
5. In omnetpp, go to **File >> Open Projects from File System** and add [NAME_OF_TOOL] to the workspace.
6. Right-click [NAME_OF_TOOL] project and go to *Properties*, under *Project References* select only inet4.5.
7. Right-click [NAME_OF_TOOL] and *Build Project*.
8. Finally, right-click [NAME_OF_TOOL] and *Run As >> OMNeT++ Simulation*.

## Execution
