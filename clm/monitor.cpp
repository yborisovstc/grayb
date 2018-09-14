#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "monitor.h"

using namespace std;

Monitor::Monitor(): mEnv(nullptr)
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
