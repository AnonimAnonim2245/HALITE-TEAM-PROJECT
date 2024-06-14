#include <stdlib.h>
#include <time.h>
#include <cstdlib>
#include <ctime>
#include <time.h>
#include <set>
#include <fstream>
#include <bits/stdc++.h> 
#include "hlt.hpp"
#include "networking.hpp"
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <queue>
#include<unordered_map>
using namespace hlt;
std::ofstream fileOutput("output.txt"); // Open output.txt for writing

namespace std {
    template <>
    struct hash<hlt::Location> {
        std::size_t operator()(const hlt::Location& loc) const {
            return ((hash<int>()(loc.x) ^ (hash<int>()(loc.y) << 1)) >> 1);
        }
    };

    template<>
    struct equal_to<hlt::Location> {
        bool operator()(const hlt::Location& lhs, const hlt::Location& rhs) const {
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }
    };
}
std::unordered_map<hlt::Location, int> isValid;


struct CompareStrength3 {
    
    hlt::GameMap &presentMap;

    CompareStrength3(hlt::GameMap& map) : presentMap(map) {}

    bool operator()(hlt::Move const& p1, hlt::Move const& p2)
    {
        return (6*(double)presentMap.getSite(p1.loc).production-(double)presentMap.getSite(p1.loc).strength) < (6*(double)presentMap.getSite(p2.loc).production-(double)presentMap.getSite(p2.loc).strength);
    }
};
unsigned char myID;
struct CompareStrength4 {
    
    hlt::GameMap &presentMap;
    int &number2;

    CompareStrength4(hlt::GameMap& map, int number) : presentMap(map),number2(number)  {}

    bool operator()(hlt::Move const& p1, hlt::Move const& p2)
    {
        return (double)presentMap.getSite(p1.loc).strength+(double)number2*presentMap.getSite(p1.loc).production 
        < (double)presentMap.getSite(p2.loc).strength+number2*(double)presentMap.getSite(p2.loc).production;
    }
};


hlt::Move find_n_th_element(std::priority_queue<hlt::Move, std::vector<hlt::Move>, CompareStrength4> pq, int n) {
    hlt::Move move;
    std::priority_queue<hlt::Move, std::vector<hlt::Move>, CompareStrength4> copyPq(pq);

    for(int i=0;i<n;i++){
        move = copyPq.top();
        copyPq.pop();
    }
    return move;
}
void BestWaytoCounterEnemy(hlt::GameMap &presentMap, hlt::Move move, bool strategiseAhead, std::set<hlt::Move> &moves) {
    int number = (strategiseAhead) ? 1 : 0;

    hlt::Site site = presentMap.getSite(move.loc, move.dir);
    hlt::Location location = presentMap.getLocation(move.loc, move.dir);
    fileOutput << location.x << " " << location.y << std::endl;



    CompareStrength4 comp(presentMap,number);
    std::priority_queue<hlt::Move, std::vector<hlt::Move>, CompareStrength4> pq(comp);
    for(unsigned char i=1;i<5;i++){
        hlt::Location loc = { location.x, location.y };
        hlt::Site neighborSite = presentMap.getSite(loc, i);
        loc = presentMap.getLocation(loc, i);
        unsigned char j = i;
        if(i+2>4){
            i-=2;
        }
        else{
            i+=2;
        }
        if(neighborSite.owner == myID && isValid.count(loc)<=0) {
            
            pq.push({ loc, i});
        }
        i=j;
    }

    if(pq.empty()) {
        return;
    }
    fileOutput << "REACHED0.5"<<std::endl;
    unsigned char total_strength;
    hlt::Site siteFirstElement, siteSecondElement, siteThirdElement;
    hlt::Move moveFirst = pq.top();

    if(!pq.empty()){
        hlt::Site siteFirstElement = presentMap.getSite(pq.top().loc);
        //fileOutput<<pq.top().loc.x<<" "<<pq.top().loc.y<<std::endl;
        unsigned char total_strength = siteFirstElement.strength + number*siteFirstElement.production;
        if(total_strength > site.strength){
            unsigned char DIR = (!strategiseAhead) ? pq.top().dir : STILL;
            fileOutput << "REACHED1";
            Location site3 = presentMap.getLocation(move.loc, move.dir);
            Location site2 = presentMap.getLocation(pq.top().loc, pq.top().dir);
            fileOutput << site2.x << " " << site2.y << std::endl;
            fileOutput << site3.x << " " << site3.y << std::endl;
            //fileOutput << strategiseAhead<<std::endl;
            if(strategiseAhead){
                moves.insert({ pq.top().loc, STILL });
            }
            else{
                moves.insert({pq.top().loc, pq.top().dir});
            }
            for(hlt::Move move2 : moves){
                isValid.insert({ move2.loc, 1 });
            }
            return;
        }

        pq.pop();

        siteSecondElement = presentMap.getSite(pq.top().loc);
        int size = pq.size();
        if(size > 0){
            for(int i=pq.size()-1;i>0;i--){
                hlt::Move secondMove = find_n_th_element(pq, i);
                hlt::Site siteMove2 = presentMap.getSite(secondMove.loc);
                unsigned char tempTotal = total_strength + siteMove2.strength + number*siteMove2.production;

                if(tempTotal > site.strength){
                    //fileOutput << "REACHED SECOND MOVE"<<std::endl;
                    unsigned char DIR = (!strategiseAhead) ? moveFirst.dir : STILL;
                    moves.insert({ moveFirst.loc, DIR });
                    moves.insert({ secondMove.loc, secondMove.dir });
                    for(hlt::Move move2 : moves){
                        isValid.insert({ move2.loc, 1 });
                    }

                    return;
                }
            }
        }

        hlt::Move secondMove = pq.top();
        pq.pop();
        siteThirdElement = presentMap.getSite(pq.top().loc);
        size = pq.size();
        total_strength+=siteSecondElement.strength+number*siteSecondElement.production;
        if(size>0){
            for(int i=pq.size()-1;i>0;i--){
                hlt::Move thirdMove = find_n_th_element(pq, i);
                hlt::Site siteMove3 = presentMap.getSite(thirdMove.loc);
                unsigned char tempTotal = total_strength + siteMove3.strength + number*siteMove3.production;
                if(tempTotal > site.strength){
                    unsigned char DIR = (!strategiseAhead) ? moveFirst.dir : STILL;
                    moves.insert({ moveFirst.loc, DIR });
                    moves.insert({ thirdMove.loc, thirdMove.dir });
                    moves.insert({ secondMove.loc, secondMove.dir });
                    for(hlt::Move move2 : moves){
                        isValid.insert({ move2.loc, 1 });
                    }
                    return;
                }
            }
        }

    }


}

unsigned char AStarDistance(hlt::GameMap &presentMap, hlt::Location location, std::set<hlt::Move> &moves) {
    int TotalDistance[5]={0};
    std::unordered_map <int, hlt::Location> BestMove;
                fileOutput << "REACHED TOTAL"<<std::endl;
    
    for(unsigned char i=1;i<5;i++){
        hlt::Location loc = { location.x, location.y };
        hlt::Site neighborSite = presentMap.getSite(loc);
        int length = (i%2) ? presentMap.width/2 : presentMap.height/2;
       
        while(neighborSite.owner == myID
        && TotalDistance[i] < length) {
            loc = presentMap.getLocation(loc,i);
            BestMove[i] = loc;
            neighborSite = presentMap.getSite(loc);
            TotalDistance[i]++;
        }
        

    }

    int MAX_TRIES = 9;
    int MIN=-1;
    unsigned char INDEX;
    for(unsigned char i=1;i<5;i++){
        hlt::Location loc = BestMove[i];
        hlt::Site neighborSite = presentMap.getSite(BestMove[i]);
        int length = (i%2) ? presentMap.width/2 : presentMap.height/2;
        int CurrentDistance = 0;
        if(TotalDistance[i] >= length){
            TotalDistance[i]+=6;
            if(MIN==-1 || MIN>TotalDistance[i]){
                MIN=TotalDistance[i];
                INDEX=i;

            }
            continue;
        }
        
        while((neighborSite.owner == 0)
        && CurrentDistance < 6) {
            loc = presentMap.getLocation(loc, i);
            BestMove[i] = loc;
            neighborSite = presentMap.getSite(loc);
            CurrentDistance++;
        }
        TotalDistance[i]+=CurrentDistance;
        if(MIN==-1 || MIN>TotalDistance[i]){
            MIN = TotalDistance[i];
            INDEX=i;
        }
        
    }

    fileOutput << "REACHED END"<< INDEX<<std::endl;

    return INDEX;
}
int main() {
    srand(time(NULL));

    std::cout.sync_with_stdio(0);

    hlt::GameMap presentMap;
    getInit(myID, presentMap);
    sendInit("MyC++Bot");
    int elemen=0;
    int h=0;
    

    while(true) {
        std::set<hlt::Move> moves;

        moves.clear();
        isValid.clear();
        getFrame(presentMap);
        h=0;
        

        CompareStrength3 comp(presentMap);
        std::priority_queue<hlt::Move, std::vector<hlt::Move>, CompareStrength3> pq(comp);
        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                hlt::Site site = presentMap.getSite({ b, a });
                hlt::Location location = { b, a };    
                int newValue=0;
                if(site.owner != myID){
                    for(unsigned char d = 1; d < 5; d++) {
                        hlt::Location loc = { b, a };
                        hlt::Site neighborSite = presentMap.getSite(loc, d);
                        if(neighborSite.owner == myID) {
                            newValue=1;
                            break;
                        }
                    }
                    if(newValue==1){
                        pq.push({location});
                    }
                }   
                
            }

        }
        Location site = presentMap.getLocation(pq.top().loc, pq.top().dir);

        int okay = 0;

        while(!pq.empty()){
            hlt::Move move = pq.top();
            pq.pop();
            okay=0;
            for(int i=0;i<=1;i++){
                std::set<hlt::Move> movesCopy;
                BestWaytoCounterEnemy(presentMap, move, i, movesCopy);
                fileOutput << "Size2: " << movesCopy.size() << std::endl;
                if(movesCopy.size() > 0){

                    for(auto move2 : movesCopy){
                        moves.insert(move2);
                    }
                    okay = 1;
                    break;
                }
            }
            if(okay==0){
                CompareStrength4 comp1(presentMap,1);
                CompareStrength4 comp2(presentMap,0);
                std::priority_queue<hlt::Move, std::vector<hlt::Move>, CompareStrength4> moves2(comp1);
                std::priority_queue<hlt::Move, std::vector<hlt::Move>, CompareStrength4> moves3(comp2);

                Location loc = presentMap.getLocation(move.loc, STILL);
                hlt::Site site = presentMap.getSite(move.loc);
                for(unsigned char i=1;i<5;i++){
                    hlt::Site neighborSite = presentMap.getSite(loc, i);
                    loc = presentMap.getLocation(loc, i);
                    unsigned char j_copy = i;
                    if(i+2>4){
                        i-=2;
                    }
                    else{
                        i+=2;
                    }
                    if(neighborSite.owner == myID && isValid.count(loc)<=0) {
                        moves2.push({ loc, i});
                    }
                    i=j_copy;
                }
                if(moves2.empty()){
                    continue;
                }
                hlt::Move move2 = moves2.top();
                for(unsigned char i=1;i<5;i++){
                    hlt::Site neighborSite = presentMap.getSite(move2.loc, i);
                    loc = presentMap.getLocation(move2.loc, i);
                    unsigned char j_copy = i;
                    if(i+2>4){
                        i-=2;
                    }
                    else{
                        i+=2;
                    }
                    if(neighborSite.owner == myID && isValid.count(loc)<=0) {
                        moves3.push({ loc, i});
                    }
                    i=j_copy;
                }
                if(moves3.empty()){
                    continue;
                }
                hlt::Move move3 = moves3.top();
                hlt::Site site4 = presentMap.getSite(move2.loc);
                unsigned char total = site4.strength + site4.production;
                total+=presentMap.getSite(move3.loc).strength;

                if(total > presentMap.getSite(move.loc).strength){
                    moves.insert({ move2.loc, STILL });
                    isValid.insert({ move2.loc, 1 });
                    isValid.insert({ move3.loc, 1 });
                    moves.insert({ move3.loc, move3.dir });
                }

            }
        }
        //fileOutput << "HERE"<<std::endl;
        for(unsigned short a = 0; a < presentMap.height; a++) {
            for(unsigned short b = 0; b < presentMap.width; b++) {
                hlt::Site site = presentMap.getSite({ b, a });
                hlt::Location location = { b, a };
                if (site.owner == myID) {
                    int ok=1;
                    for(unsigned char d = 1; d < 5; d++) {
                        hlt::Location loc = { b, a };
                        hlt::Site neighborSite = presentMap.getSite(loc, d);
                        if(neighborSite.owner != myID) {
                            ok=0;
                            break;
                        }
                        
                    }
                    if((ok==0 && isValid.count(location)<=0) || (site.strength <= 6 * site.production) ){
                        moves.insert({ { b, a }, STILL });
                    }
                    else{
                        
                        unsigned char dir = AStarDistance(presentMap, location, moves);
                        moves.insert({{b,a},dir});
                        

                    }
                    isValid.insert({location, 1});

                }
                
            }

        }
        

        sendFrame(moves);
    }

    return 0;
}