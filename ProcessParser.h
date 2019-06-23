#ifndef PROCESSPARSER_H
#define PROCESSPARSER_H

#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>

#include "constants.h"
#include "util.h"

using namespace std;

class ProcessParser{
public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static string getVmSize(string pid);
    static string getCpuPercent(string pid);
    static long int getSysUpTime();
    static string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfCores();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static string PrintCpuStats(vector<string> values1, vector<string> values2);
};


string ProcessParser::getVmSize(string pid) {
    string line;
    //Declaring search attribute for file
    string name = "VmData";
    float result;
    // Opening stream for specific file
    ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
    // Get each line from the stream
    while(getline(stream, line)){
        if (line.compare(0, name.size(), name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg, end);
            //conversion kB -> GB
            result = (stof(values[1]) / float(1024 * 1024));
            break;
        }
    }
    return to_string(result);
}

string ProcessParser::getCpuPercent(string pid) {
    string line;
    float result;
    ifstream stream = Util::getStream((Path::basePath() + pid + "/" + Path::statPath()));
    getline(stream, line);
    istringstream buf(line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);

    float utime = stof(ProcessParser::getProcUpTime(pid));
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float starttime = stof(values[21]);
    float uptime = ProcessParser::getSysUpTime();
    float freq = sysconf(_SC_CLK_TCK);
    float total_time = utime + stime + cutime + cstime;
    float seconds = uptime - (starttime/freq);
    result = 100.0*((total_time/freq)/seconds);
    return to_string(result);
}

string ProcessParser::getProcUpTime(string pid) {
  string line;
  float result;
  ifstream stream = Util::getStream((Path::basePath() + pid + "/" + Path::statPath()));
  getline(stream, line);
  istringstream buf(line);
  istream_iterator<string> beg(buf), end;
  vector<string> values(beg, end);
  // Using sysconf to get clock ticks of the host machine
  result = float(stof(values[13])/sysconf(_SC_CLK_TCK));

  return to_string(result);
}

long int ProcessParser::getSysUpTime() {
    string line;
    long int result;
    ifstream stream = Util::getStream((Path::basePath() + Path::upTimePath()));
    getline(stream, line);
    istringstream buf(line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg, end);
    // stoi converts a string to an int.
    result = stoi(values[0]);
    return result;
}

string ProcessParser::getProcUser(string pid) {
  std::string line;
  std::string name = "Uid:";
  std::string result = "";

  ifstream stream =  Util::getStream(Path::basePath() + pid + Path::statusPath());
  while(getline(stream, line)) {
    if(line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      result = values[1];
      break;
    }
  }
  stream = Util::getStream("/etc/passwd");
  name =("x:" + result);
  while(getline(stream, line)) {
      if(line.find(name) != string::npos) {
        result = line.substr(0, line.find(":"));
        return result;
      }
  }
  return "";
}

vector<string> ProcessParser::getPidList() {
    DIR* dir;
    // Scan /proc dir for all directories with numbers as their names
    // If we get valid check we store dir names in vector as list of machine pids
    vector<string> container;
    if(!(dir = opendir("/proc")))
        throw runtime_error(strerror(errno));
    while (dirent* dirp = readdir(dir)) {
    // is this a directory?
        if(dirp->d_type != DT_DIR)
            continue;
      
        if (all_of(dirp->d_name, dirp->d_name + strlen(dirp->d_name), [](char c){ return isdigit(c); })) {
            container.push_back(dirp->d_name);
        }
    }
    //Validating process of directory closing
    if(closedir(dir))
        throw std::runtime_error(strerror(errno));
    return container;
}

// Retrieve the command that executed the process.
string ProcessParser::getCmd(string pid) {
    string line;
    ifstream stream = Util::getStream((Path::basePath() + pid + Path::cmdPath()));
    getline(stream, line);
    return line;
}

// Retrieve the number of CPU cores on the host.
int ProcessParser::getNumberOfCores() {
    // Get the number of host cpu cores
    string line;
    string name = "cpu cores";
    ifstream stream = Util::getStream((Path::basePath() + "cpuinfo"));
    while(getline(stream, line)) {
      if(line.compare(0, name.size(), name) == 0) {
          istringstream buf(line);
          istream_iterator<string> beg(buf), end;
          vector<string> values(beg, end);
          return stoi(values[3]);
      }
    }
    return 0;
}

vector<string> ProcessParser::getSysCpuPercent(string coreNumber) {
  std::string line;
  std::string name = "cpu" + coreNumber;
  std::string value;
  int result;
  ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
  while (getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      return values;
    }
  }
  return vector<string>();
}

float getSysActiveCpuTime(vector<string> values) {
    return (stof(values[S_USER]) +
            stof(values[S_NICE]) +
            stof(values[S_SYSTEM]) +
            stof(values[S_IRQ]) +
            stof(values[S_SOFTIRQ]) +
            stof(values[S_STEAL]) +
            stof(values[S_GUEST]) +
            stof(values[S_GUEST_NICE]));
}

float getSysIdleCpuTime(vector<string> values) {
    return (stof(values[S_IDLE]) + stof(values[S_IOWAIT]));
}

string ProcessParser::PrintCpuStats(vector<string> values1, vector<string> values2) {
  float active_time = getSysActiveCpuTime(values2) - getSysActiveCpuTime(values1);
  float idle_time = getSysIdleCpuTime(values2) - getSysIdleCpuTime(values1);
  float total_time = active_time + idle_time;
  float result = 100.0 * (active_time / total_time);
  return to_string(result);
}

float ProcessParser::getSysRamPercent() {
  string line;
  string name1 = "MemAvailable:";
  string name2 = "MemFree:";
  string name3 = "Buffers:";

  ifstream stream = Util::getStream((Path::basePath() + Path::memInfoPath()));
  float total_mem = 0;
  float free_mem = 0;
  float buffers = 0;
  while(getline(stream, line)) {
    if(total_mem != 0 && free_mem != 0)
      break;

    if(line.compare(0, name1.size(), name1) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      total_mem = stof(values[1]);
    }

    if(line.compare(0, name2.size(), name2) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      free_mem = stof(values[1]);
    }

    if(line.compare(0, name3.size(), name3) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      buffers = stof(values[1]);
    }
  }
  //calculating usage:
  return float(100.0 * (1 - (free_mem / (total_mem - buffers))));
}

string ProcessParser::getSysKernelVersion() {
  string line;
  string name = "Linux version ";
  ifstream stream = Util::getStream((Path::basePath() + Path::versionPath()));
  while(getline(stream, line)) {
    if(line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string>  beg(buf), end;
      vector<string> values(beg, end);
      return values[2];
    }
  }
  return "";
}

string ProcessParser::getOSName() {
  string line;
  string name = "PRETTY_NAME";

  ifstream stream = Util::getStream(("/etc/os-release"));

  while(getline(stream, line)) {
    if(line.compare(0, name.size(), name) == 0) {
      size_t found = line.find("=");
      found++;
      string result = line.substr(found);
      result.erase(std::remove(result.begin(), result.end(), '"'), result.end());
      return result;
    }
  }
  return "";
}

// Calculate the total thread count
int ProcessParser::getTotalThreads() {
  string line;
  int result = 0;
  string name = "Threads:";
  vector<std::string>_list = ProcessParser::getPidList();
  for (int i = 0 ; i < _list.size(); i++) {
    string pid = _list[i];
    //getting every process and reading their number of their threads
    ifstream stream = Util::getStream((Path::basePath() + pid + Path::statusPath()));
    while(getline(stream, line)) {
      if(line.compare(0, name.size(), name) == 0) {
        istringstream buf(line);
        istream_iterator<std::string> beg(buf), end;
        vector<std::string> values(beg, end);
        result += std::stoi(values[1]);
        break;
      }
    }
  }
  return result;
}

int ProcessParser::getTotalNumberOfProcesses() {
  string line;
  int result = 0;
  string name = "processes";
  ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
  while(getline(stream, line)) {
    if(line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<string> values(beg, end);
      result += std::stoi(values[1]);
      break;
    }
  }
  return result;
}

int ProcessParser::getNumberOfRunningProcesses() {
  string line;
  int result = 0;
  string name = "procs_running";
  ifstream stream = Util::getStream((Path::basePath() + Path::statPath()));
  while(getline(stream, line)) {
    if(line.compare(0, name.size(), name) == 0) {
      istringstream buf(line);
      istream_iterator<string> beg(buf), end;
      vector<std::string> values(beg, end);
      result += std::stoi(values[1]);
      break;
    }
  }
  return result;
}

#endif