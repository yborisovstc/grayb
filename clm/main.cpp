
#include <string>
#include <iostream>

//#include "gperftools/profiler.h"

#include "monitor.h"

/* \brief FAP2 command line monitor
 *
 */

using namespace std;


const string help = "\nFAP2 Command line monitor\n\n\
Usage:\n\
fapm [options] [file]\n\
Options:\n\
-h - show help\n\
-l <file> - log file\n\
-p <file> - profiler file\n\
\n";

int main(int argc, char* argv[])
{
    int res = 0;

    Monitor mnt;
    if (argc > 1) {
	for (int i = 1; i < argc; i++) {
	    string arg(argv[i]);
	    cout << arg << endl;
	    if (arg == "-h") {
		cout << help;
	    } else if (arg.compare(0, 2, "-l") == 0) {
		// Log file
		string path = arg.substr(2);
		bool spres = mnt.setLogFile(path);
		if (!spres) {
		    cout << "Cannot open log file";
		}

	    } else if (arg.compare(0, 2, "-p") == 0) {
		// Profile file
		string path = arg.substr(2);
		bool spres = mnt.setProfPath(path);
		if (!spres) {
		    cout << "Cannot open profiler file";
		}
	    } else {
		// File name ?
		mnt.setSpecName(arg);
	    }
	}
	if (res == 0) {
	    // Init env and run
	    // Tried to use google profiler, ref ds_prfopt_gpprof
	    //ProfilerStart("./google.profile");
	    mnt.initEnv();
	    mnt.runModel();
	    bool sdres = mnt.saveProfilerData();
	    if (!sdres) {
		cout << "Error on saving profile data to file";
	    }
	    //ProfilerFlush();
	    //ProfilerStop();
	}
    } else {
	cout << help;
    }
    return res;
}
