#include "Process.h"
#include <vector>
#include <string>

using namespace std;

class ProcessContainer{

private:
    vector<Process> _list;
public:
    ProcessContainer(){
        this->refreshList();
    }
    void refreshList();
    string printList();
    vector<vector<string>> getList();
};

void ProcessContainer::refreshList(){
    vector<string> pidList = ProcessParser::getPidList();
    this->_list.clear();
    for(int i = 0 ; i < pidList.size(); i++){
        Process proc(pidList[i]);
        this->_list.push_back(proc);
    }
}

string ProcessContainer::printList(){
    string result="";
    for(int i = 0; i < this->_list.size(); i++){
        result += this->_list[i].getProcess();
    }
    return result;
}

vector<vector<string>> ProcessContainer::getList(){
    vector<vector<string>> values;
    vector<string> stringifiedList;
    for(int i = 0; i < ProcessContainer::_list.size(); i++){
        stringifiedList.push_back(ProcessContainer::_list[i].getProcess());
    }
    int lastIndex = 0;
    for (int i = 0; i < stringifiedList.size(); i++){
        if(i % 10 == 0 && i > 0){
          vector<string>  sub(&stringifiedList[i-10], &stringifiedList[i]);
          values.push_back(sub);
          lastIndex = i;
        }
        if(i == (ProcessContainer::_list.size() - 1) && (i - lastIndex) <10){
            vector<string> sub(&stringifiedList[lastIndex], &stringifiedList[i+1]);
            values.push_back(sub);
        }
   }
    return values;
}
