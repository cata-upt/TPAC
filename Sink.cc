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

#include "Sink.h"

Define_Module(Sink);

void Sink::initialize()
{
    delayStats[0].setName("user1Histogram");
    delayStats[1].setName("user2Histogram");
    delayStats[2].setName("user3Histogram");
    delayVector[0].setName("user1Vector");
    delayVector[1].setName("user2Vector");
    delayVector[2].setName("user3Vector");
}

void Sink::handleMessage(cMessage *msg)
{
      simtime_t lifetime = simTime() - msg->getCreationTime();

      if (msg->arrivedOn("rxPackets")) {
        int index=msg->getArrivalGate()->getIndex();
        EV<<"Message from user "<<index<<endl;
        delayStats[index].collect(lifetime);
        delayVector[index].record(lifetime);
      }

      EV << "Received " << msg->getName() << ", lifetime: " << lifetime << "s" << endl;
      delete msg;
}

