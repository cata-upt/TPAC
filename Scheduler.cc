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

#include "Scheduler.h"

Define_Module(Scheduler);



Scheduler::Scheduler()
{
    selfMsg = nullptr;
    userNumber=0;
    userBlockNumber=0;
}

Scheduler::~Scheduler()
{
    cancelAndDelete(selfMsg);
}


void Scheduler::initialize()
{
    NrUsers = par("gateSize").intValue();
    NrOfChannels = 10;//read from omnetpp.ini
    selfMsg = new cMessage("selfMsg");
    scheduleAt(simTime(), selfMsg);
}

void Scheduler::handleMessage(cMessage *msg)
{
    int availableChannels=NrOfChannels;
    int q[NrUsers];
    int userWeights[NrUsers];
    for(int j=0;j<NrUsers; j++){
        q[j]= getParentModule()->getSubmodule("user",j)->getSubmodule("myqq")->par("qlp");
        userWeights[j]= getParentModule()->getSubmodule("user",j)->par("weight");
        EV << "q["<<j<<"]= " << q[j] <<endl;
        EV << "weight["<<j<<"]= " << userWeights[j] <<endl;
    }
    int nrOfBlocks[NrUsers];
    if (msg == selfMsg){
        for(int i=0;i<NrUsers;i++){
           nrOfBlocks[i]=0;
        }
        //finds out the length of each queue !!
        while(availableChannels>0){
            for(int i=userNumber;i<NrUsers;i++){
                EV << "User number:"<<userNumber<<endl;
                EV << "User block number:"<<userBlockNumber<<endl;
                if(availableChannels>0 && q[i]>0){
                    int blocks=0;
                    if(userBlockNumber==0){
                        blocks = std::min(availableChannels,userWeights[i]);
                        nrOfBlocks[i] +=blocks;
                        availableChannels -= blocks;
                    }else{
                        blocks = std::min(availableChannels,userBlockNumber);
                        nrOfBlocks[i] +=blocks;
                        availableChannels -= blocks;
                    }
                    if(blocks<userWeights[i] && availableChannels==0){
                        userNumber=i;
                        userBlockNumber=userWeights[i]-blocks;
                    }else{
                        if(i+1<NrUsers) userNumber=i+1;
                        else userNumber=0;
                        userBlockNumber=userWeights[userNumber];
                    }
                    EV<<"Blocks allocated: "<< blocks<<endl;
                    EV<<"Available channels: "<<availableChannels<<endl;
                }

                //set parameter value, e.g., nr of blocks to be sent from the queue by user i

            }
        }
        for(int i =0;i<NrUsers;i++){
            cMessage *cmd = new cMessage("cmd");
            cmd->addPar("nrOfBlocks").setLongValue(nrOfBlocks[i]);
            EV << "NrOfBlocks["<<i<<"]= " << nrOfBlocks[i] <<endl;
            send(cmd,"txScheduling",i);
        }
        scheduleAt(simTime()+par("schedulingPeriod").doubleValue(), selfMsg);
    }

}
