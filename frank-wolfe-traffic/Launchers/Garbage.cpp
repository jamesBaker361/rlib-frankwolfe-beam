#include <iostream>

class Garbage{
    public:

        Garbage() : garbageInt(0) {}

        Garbage(int garbageInt) : garbageInt(garbageInt) {
            std::cout << "Garbage created with address "<< this<< std::endl;
        }
        ~Garbage() {
            std::cout << "deleted garbage "<< garbageInt << "address = "<< this<< std::endl;
        }

        Garbage(const Garbage & old){
            std::cout << "Garbage copied with new address "<< this  <<std::endl;
            garbageInt = old.garbageInt;
        }

        int getGarbageInt(){
            return garbageInt;
        }

        int garbageInt;
};

class ChildGarbage : public Garbage{
    public:
        using Garbage::Garbage;
        ChildGarbage() {
        }
        int getGarbageIntSquared(){
            return garbageInt*garbageInt;
        }
};