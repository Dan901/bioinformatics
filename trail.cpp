#include "trail.h"

std::string Trail::getName(){
    std::string name = trail.front().first;
    for(auto element : trail){
        name +="->"+element.second;
    }
    return name;
}