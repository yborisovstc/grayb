
#include <string>
#include <iostream>

//#include "gperftools/profiler.h"

#include "monitor.h"

/* \brief FAP2 command line monitor
 *
 */

using namespace std;

static bool mAutoRun = false;

const string help = "\nFAP2 Command line monitor\n\n\
Usage:\n\
fapm [options] [file]\n\
Options:\n\
-h - show help\n\
-l <file> - log file\n\
-p <file> - profiler file\n\
-a        - auto-run\n\
-s <file> - spec file\n\
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
	    } else if (arg.compare(0, 2, "-s") == 0) {
		// Spec file
		string path = arg.substr(2);
		mnt.setSpecName(path);
	    } else if (arg.compare(0, 2, "-a") == 0) {
		// Autorun
		mAutoRun = true;
	    } else {
		cout << "Error: unknown argument [" << arg << "]" << endl;
		res = -1;
	    }
	}
	if (res == 0) {
	    if (mAutoRun) {
		// Auto-run mode: run model and exit
		// Init env and run
		mnt.initEnv();
		mnt.runModel();
		bool sdres = mnt.saveProfilerData();
		if (!sdres) {
		    cout << "Error on saving profile data to file";
		}
	    } else {
		// Run monitor user input handling loop
		mnt.run();
	    }
	}
    } else {
	cout << help;
    }
    return res;
}
