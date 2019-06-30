#include <cstdint>
#include <vector>
#include <iostream>
#include <algorithm>

struct Process
{
    uint8_t processId;
    uint8_t burstTime;
    uint8_t arrivalTime;
    uint8_t priority;
};

enum class SchedulingAlgo
{
    RR,
    PP,
    SRTF,
    OWN
};

struct ProcessIdDurationPair
{
    uint8_t processId;
    uint8_t duration;
};
//initialize Process
Process initProcess (uint8_t ID, uint8_t AT, uint8_t BT, uint8_t PR){
    Process p;
    p.processId=ID;
    p.burstTime=BT;
    p.arrivalTime=AT;
    p.priority=PR;
    return p;
}
//returns total burstTime
uint8_t totalBT (std::vector<Process> processes){
    uint8_t x=0;
    for (elem : processes){
        x=x+elem.burstTime;
    }
    return x;
}
//get index of lowest priority
uint8_t indexPriority (std::vector<Process> processes, uint8_t AT){
    uint8_t x=0;
    uint8_t lowest=processes[processes.size()-1].priority+1;
    for (int i=processes.size()-1; i>=0; i--){
        if (processes[i].priority<lowest&&processes[i].arrivalTime<=AT&&processes[i].burstTime>0){
            x=i;
            lowest=processes[i].priority;
        }
    }
    return x;
}
//get index of lowest BT
uint8_t indexBT (std::vector<Process> processes, uint8_t AT, uint8_t maxi){
    uint8_t x=0;
    uint8_t lowest=maxi;
    for (int i=processes.size()-1; i>=0; i--){
        if (processes[i].burstTime<lowest && processes[i].arrivalTime<=AT && processes[i].burstTime>0){
            x=i;
            lowest=processes[i].burstTime;
        }
    }
    return x;
}
//sort by ID
bool byId (const Process &L, const Process &R){
    return L.processId<R.processId;
}
//sort by priority
bool byPriority (const Process &L, const Process &R){
    return L.priority<R.priority;
}
//sort by BT
bool byBT (const Process &L, const Process &R){
    return L.burstTime<R.burstTime;
}
//sort by AT
bool byAT (const Process &L, const Process &R){
    return L.arrivalTime<R.arrivalTime;
}
//put in ready queue
std::vector<uint8_t> putQueue (std::vector<uint8_t> rQueue, std:: vector<Process> processes, int first, uint8_t last){
    std::sort(processes.begin(),processes.end(), byAT);
    uint8_t x=0;
    for (elem : processes){
        if (elem.arrivalTime>first&&elem.arrivalTime<=last){
            rQueue.push_back(x);
        }
        x++;
    }
    return rQueue;
}
std::vector<ProcessIdDurationPair> schedule(SchedulingAlgo algorithm, std::vector<Process> processes)
{
    std::vector<ProcessIdDurationPair> sorted;
    // implementation for Round Robin
    if ((int)algorithm==0){
        std::cout<<"Round Robin"<<std::endl;
        std::sort(processes.begin(), processes.end(), byAT);    //sort the processes by ID
        //std::cout<<(int)totalBT(processes)<<std::endl;
        uint8_t i=0;    //index
        uint8_t AT=0;   //current AT
        uint8_t timeQ=5;
        uint8_t maxBt=totalBT(processes);   //save max BT
        std::vector<uint8_t> rQueue;
        rQueue=putQueue(rQueue,processes,-1,0);
        while (i<rQueue.size()){
            ProcessIdDurationPair temp;
            temp.processId=processes[rQueue[i]].processId;
            if (processes[rQueue[i]].burstTime<timeQ){
                temp.duration=processes[rQueue[i]].burstTime;
                processes[rQueue[i]].burstTime=0;
                AT=AT+temp.duration;
                rQueue=putQueue(rQueue,processes,AT-temp.duration,AT);
            }
            else {
                temp.duration=timeQ;
                processes[rQueue[i]].burstTime=processes[rQueue[i]].burstTime-timeQ;
                AT=AT+timeQ;
                rQueue=putQueue(rQueue,processes,AT-timeQ,AT);
                rQueue.push_back(rQueue[i]);
            }
            if (temp.duration>0){
                sorted.push_back(temp);
            }
            i++;
        }

    }
    // implementation for Preemptive Priority
    else if ((int)algorithm==1){
        std::cout<<"Preemptive Priority"<<std::endl;
        std::sort(processes.begin(), processes.end(), byPriority);    //sort the processes by Priority
        uint8_t i=indexPriority(processes, 0);    //lowest priority index
        uint8_t AT=0;   //current AT
        uint8_t counter=0;  //count
        uint8_t maxBt=totalBT(processes);   //save max BT
        while (AT<maxBt){
            //std::cout<<(int)AT<<": "<<(int)i<<"- "<<(int)counter<<std::endl;
            if (i!=indexPriority(processes, AT)){
                ProcessIdDurationPair temp;
                temp.processId=processes[i].processId;
                temp.duration=counter;
                counter=0;
                i=indexPriority(processes, AT);
                sorted.push_back(temp);
            }
            counter=counter+1;
            processes[i].burstTime=processes[i].burstTime-1;
            AT=AT+1;
        }
        //add the last part
        ProcessIdDurationPair temp;
        temp.processId=processes[i].processId;
        temp.duration=counter;
        counter=0;
        i=indexPriority(processes, AT);
        sorted.push_back(temp);
    }
    // implementation for SRTF
    else if ((int)algorithm==2){
        std::cout<<"SRTF"<<std::endl;
        std::sort(processes.begin(), processes.end(), byBT);
        uint8_t maxBt=totalBT(processes);   //save max BT
        uint8_t i=indexBT(processes, 0, maxBt);    //lowest BT index
        uint8_t AT=0;   //current Time
        uint8_t counter=0;  //count
        //std::cout<<(int)indexBT(processes, 0)<<std::endl;
        while (AT<maxBt){
            //std::cout<<(int)AT<<": "<<(int)i<<"- "<<(int)counter<<std::endl;
            if (i!=indexBT(processes, AT, maxBt)){ //check if the previous index changed
                ProcessIdDurationPair temp;
                temp.processId=processes[i].processId;
                temp.duration=counter;
                counter=0;
                i=indexBT(processes, AT, maxBt);
                sorted.push_back(temp);
            }
            counter=counter+1;
            processes[i].burstTime=processes[i].burstTime-1;
            AT=AT+1;
        }
        //add the last part
        ProcessIdDurationPair temp;
        temp.processId=processes[i].processId;
        temp.duration=counter;
        counter=0;
        sorted.push_back(temp);
    }
    else if ((int)algorithm==3){
        std::cout<<"Own Algo"<<std::endl;
        std::sort(processes.begin(), processes.end(), byAT);    //sort the processes by ID
        //std::cout<<(int)totalBT(processes)<<std::endl;
        uint8_t i=0;    //index
        uint8_t AT=0;   //current AT
        uint8_t timeQ=5;
        uint8_t maxBt=totalBT(processes);   //save max BT
        std::vector<uint8_t> rQueue;
        rQueue=putQueue(rQueue,processes,-1,0);
        while (i<rQueue.size()){
            ProcessIdDurationPair temp;
            temp.processId=processes[rQueue[i]].processId;
            if (processes[rQueue[i]].burstTime<timeQ){
                temp.duration=processes[rQueue[i]].burstTime;
                processes[rQueue[i]].burstTime=0;
                AT=AT+temp.duration;
                rQueue=putQueue(rQueue,processes,AT-temp.duration,AT);
            }
            else {
                temp.duration=timeQ;
                processes[rQueue[i]].burstTime=processes[rQueue[i]].burstTime-timeQ;
                AT=AT+timeQ;
                rQueue=putQueue(rQueue,processes,AT-timeQ,AT);
                rQueue.push_back(rQueue[i]);
            }
            if (temp.duration>0){
                sorted.push_back(temp);
                timeQ++;
            }
            i++;
        }

    }
    return sorted;
}

int main() {
                          //ID,AT,BT,PR
    Process p1 = initProcess(1,0,3,2);
    Process p2 = initProcess(2,1,5,1);
    Process p3 = initProcess(3,2,7,4);
    Process p4 = initProcess(4,2,10,3);
    Process p5 = initProcess(5,4,2,4);
    Process p6 = initProcess(6,5,1,1);

    //round robin
    std::vector<ProcessIdDurationPair> RR = schedule(SchedulingAlgo::RR,{p2,p1,p3,p4});
    for (auto elem : RR){
        std::cout<<"<"<<(int)elem.processId<<", "<<(int)elem.duration<<">, ";
    }
    std::cout<<std::endl;
    //preemptive priority
    std::vector<ProcessIdDurationPair> PP = schedule(SchedulingAlgo::PP,{p2,p1,p3,p4});
    for (auto elem : PP){
        std::cout<<"<"<<(int)elem.processId<<", "<<(int)elem.duration<<">, ";
    }
    std::cout<<std::endl;
    //SRTF
    std::vector<ProcessIdDurationPair> SRTF = schedule(SchedulingAlgo::SRTF,{p2,p1,p3,p4,p5});
    for (auto elem : SRTF){
        std::cout<<"<"<<(int)elem.processId<<", "<<(int)elem.duration<<">, ";
    }
    std::cout<<std::endl;
    //Own Algo
    std::vector<ProcessIdDurationPair> OWN = schedule(SchedulingAlgo::OWN,{p2,p1,p3,p4,p5});
    for (auto elem : OWN){
        std::cout<<"<"<<(int)elem.processId<<", "<<(int)elem.duration<<">, ";
    }
    std::cout<<std::endl;
}
