#include "Launchers/Garbage.cpp"
#include <iostream>
#include <vector>

class GarbageList{
    public:
    GarbageList(std::vector< Garbage> gList): gList(gList) {

    }

    ~GarbageList(){
        std::cout <<"list deleted address = "<< this << std::endl;
    }

    void add(Garbage g){
        gList.push_back(g);
    }

    std::vector<Garbage> getGList(){
        return gList;
    }
    
    std::vector<Garbage>  gList;
};