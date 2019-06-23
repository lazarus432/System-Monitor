#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <fstream>

using namespace std;

// Classic helper function
class Util {

public:
    static string convertToTime (long int input_seconds);
    static string getProgressBar(string percent);
    static ifstream getStream(string path);
};

    string Util::convertToTime (long int input_seconds){
    long minutes = input_seconds / 60;
    long hours = minutes / 60;
    long seconds = int(input_seconds%60);
    minutes = int(minutes%60);
    string result = to_string(hours) + ":" + to_string(minutes) + ":" + to_string(seconds);
    return result;
}

// constructing string for given percentage
// 50 bars is uniformly streched 0 - 100 %
// meaning: every 2% is one bar(|)
string Util::getProgressBar(string percent) {
    string result = "0% ";
    int _size = 50;
    int  boundaries = (stof(percent)/100) * _size;

    for(int i = 0; i < _size; i++){
        if(i <= boundaries) {
            result += "|";
        }
        else {
            result += " ";
        }
    }

    result += " " + percent.substr(0,5) + " /100%";
    return result;
}

// wrapper for creating streams
ifstream Util::getStream(string path){
    ifstream stream(path);
    if (!stream){
        throw std::runtime_error("Non - existing PID");
    }
    return stream;
}

#endif