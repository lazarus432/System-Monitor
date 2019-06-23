#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <ncurses.h>
#include <time.h>
#include <sstream>
#include <iomanip>

#include "util.h"
#include "SysInfo.h"
#include "ProcessContainer.h"

using namespace std;


char* getCString(std::string str){
    char * cstr = new char [str.length()+1];
    std::strcpy (cstr, str.c_str());
    return cstr;
}
void writeSysInfoToConsole(SysInfo sys, WINDOW* sys_win){
    sys.setAttributes();

    wattron(sys_win,COLOR_PAIR(3));
    mvwprintw(sys_win,2,2,getCString("OS: "));
    wattroff(sys_win,COLOR_PAIR(3));
    mvwprintw(sys_win,2,6,getCString(( sys.getOSName())));
    wattron(sys_win,COLOR_PAIR(3));
    mvwprintw(sys_win,3,2,getCString("Kernel Version: "));
    wattroff(sys_win,COLOR_PAIR(3));
    mvwprintw(sys_win,3,18,getCString(( sys.getKernelVersion())));
    mvwprintw(sys_win,7,2,getCString( "CPU: \n"));
    wattron(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,8,2,getCString(Util::getProgressBar(sys.getCpuPercent())));
    wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,9,2,getCString(( "Other cores:")));
    wattron(sys_win,COLOR_PAIR(1));
    std::vector<std::string> val = sys.getCoresStats();
    for(int i=0;i<val.size();i++){
    mvwprintw(sys_win,(10+i),2,getCString(val[i]));
    }
    wattroff(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,12,2,getCString(( "Memory: \n")));
    wattron(sys_win,COLOR_PAIR(1));
    mvwprintw(sys_win,13,2,getCString(Util::getProgressBar(sys.getMemPercent())));
    wattroff(sys_win,COLOR_PAIR(1));
    wattron(sys_win,COLOR_PAIR(3));
    mvwprintw(sys_win,4,2,getCString("Total Processes: "));
    wattroff(sys_win,COLOR_PAIR(3));
    mvwprintw(sys_win,4,20,getCString(( sys.getTotalProc())));
    wattron(sys_win,COLOR_PAIR(3));
    mvwprintw(sys_win,5,2,getCString("Running Processes: "));
    wattroff(sys_win,COLOR_PAIR(3));
    mvwprintw(sys_win,5,21,getCString(( sys.getRunningProc())));
    wattron(sys_win,COLOR_PAIR(3));
    mvwprintw(sys_win,6,2,getCString("Up Time: "));
    wattroff(sys_win,COLOR_PAIR(3));
    mvwprintw(sys_win,6,11,getCString((Util::convertToTime(sys.getUpTime()))));
    
    wrefresh(sys_win);
}

void getProcessListToConsole(std::vector<string> processes,WINDOW* win){

    wattron(win,COLOR_PAIR(2));
    mvwprintw(win,2,2,"PID:");
    mvwprintw(win,2,7,"User:");
    mvwprintw(win,2,13,"CPU[%%]:");
    mvwprintw(win,2,21,"RAM[MB]:");
    mvwprintw(win,2,30,"Uptime:");
    mvwprintw(win,2,39,"CMD:");
    wattroff(win, COLOR_PAIR(2));
    for(int i=0; i< processes.size();i++){
        mvwprintw(win,3+i,2,getCString(processes[i]));
   }
}
void printMain(SysInfo sys, ProcessContainer procs){
	initscr();			/* Start curses mode 		  */
    noecho(); // not printing input values
    cbreak(); // Terminating on classic ctrl + c
    start_color(); // Enabling color change of text
    int yMax,xMax;
    getmaxyx(stdscr,yMax,xMax); // getting size of window measured in lines and columns(column one char length)
	WINDOW *sys_win = newwin(15,xMax-1,0,0);
	WINDOW *proc_win = newwin(15,xMax-1,18,0);


    init_pair(1,COLOR_CYAN,COLOR_BLACK);
    init_pair(2,COLOR_CYAN,COLOR_BLACK);
    init_pair(3,COLOR_YELLOW,COLOR_BLACK);
    int counter = 0;
    while(1){
    //box(sys_win,35,47);
    wattron(sys_win,COLOR_PAIR(3));
    wborder(sys_win, 32, 32, 47, 32, 43, 43, 32, 32);
    wattroff(sys_win,COLOR_PAIR(3));
    wattron(proc_win,COLOR_PAIR(3));
    wborder(proc_win, 32, 32, 47, 47, 43, 43, 43, 43);
    wattroff(proc_win,COLOR_PAIR(3));
    //box(proc_win,0,0);
    procs.refreshList();
    vector<vector<string>> processes = procs.getList();
    writeSysInfoToConsole(sys,sys_win);
    getProcessListToConsole(processes[counter],proc_win);
    wrefresh(sys_win);
    wrefresh(proc_win);
    refresh();
    sleep(1);
    if(counter ==  (processes.size() -1)){
        counter = 0;
    }
    else {
        counter ++;
    }
    }
	endwin();
}
int main( int   argc, char *argv[] )
{
 //Object which contains list of current processes, Container for Process Class
    ProcessContainer procs;
// Object which containts relevant methods and attributes regarding system details
    SysInfo sys;
    //std::string s = writeToConsole(sys);
    printMain(sys,procs);
    return 0;
}
