
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
	    //ProfilerFlush();
	    //ProfilerStop();
	}
    } else {
	cout << help;
    }
    return res;
}
