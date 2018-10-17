#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "monitor.h"

using namespace std;

const string Monitor::KDefPrompt = ">";

Monitor::Monitor(): mEnv(nullptr), mPrompt(KDefPrompt)
{

}

Monitor::~Monitor()
{
}

void Monitor::setSpecName(const string& aFileName)
{
    assert(mSpecName.empty());
    mSpecName = aFileName;
}

void Monitor::initEnv()
{
    if (mEnv != nullptr) {
	delete mEnv;
	mEnv = nullptr;
    }
    if (mLogName.empty()) {
	// Assuming log name is default
    }
    mEnv = new Env(mSpecName, mLogName);
}


void Monitor::runModel()
{
    assert(mEnv != nullptr);
    mEnv->ConstructSystem();
}

bool Monitor::setProfPath(const string& aPath)
{
    bool res = true;
    /*
    FILE* fp = fopen(aPath.c_str(), "w+");
    if (fp) {
	mProfName = aPath;
    } else {
	res = false;
    }
    */
    mProfName = aPath;
    return res;
}

bool Monitor::setLogFile(const string& aPath)
{
    bool res = true;
    mLogName = aPath;
    return res;
}

bool Monitor::saveProfilerData()
{
    // Save profiler data to file
    bool res = mEnv->Profiler()->SaveToFile(mProfName);
    return res;
}

bool Monitor::run()
{
    bool res = true;
    bool end = false;
    do {
	cout << mPrompt;
	string input;
	cin >> input;
	if (input == "exit") {
	    end = true;
	}
    } while (!end);
    return res;
}
