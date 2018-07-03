#include <cassert>

#include "monitor.h"

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
    mEnv = new Env(mSpecName, "");
}


void Monitor::runModel()
{
    assert(mEnv != nullptr);
    mEnv->ConstructSystem();
}
