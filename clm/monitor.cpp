#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "monitor.h"

using namespace std;

const string Monitor::KDefPrompt = ">";

/** Generator of input handlers factory registry item */
template<typename T> pair<string, Monitor::TIhFact*> Item() {
    return pair<string, Monitor::TIhFact*>
	(T::command(), T::create);
}

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
	// TODO to redesign using streambuf
	const int bufsize = 256;
	char buf[bufsize];
	cin.getline(buf, bufsize);
	string input(buf); 

	if (!input.empty()) {
	    if (input == "exit") {
		end = true;
	    } else {
		size_t pos = input.find_first_of(' ');
		string command = input.substr(0, pos);
		string args = input.substr(pos + 1);
		// Create handler
		unique_ptr<InputHandler> handler(createHandler(command));
		if (handler.get() == nullptr) {
		    cout << "Error: unknown command [" << command << "]";
		} else {
		    handler->handle(args);
		}
	    }
	}
    } while (!end);
    return res;
}

InputHandler* Monitor::createHandler(const string& aCmd)
{
    InputHandler* res = nullptr;
    if (mIhReg.count(aCmd) > 0) {
	TIhFact* fact = mIhReg.at(aCmd);
	res = fact(*this);
    }
    return res;
}

bool Monitor::saveModel(const string& aPath)
{
    bool res = false;
    MElem* root = mEnv->Root();
    if (root != nullptr) {
	// Check getting chromo 
	root->Chromos().Save(aPath);
    }
    return res;
}


/** Handler of 'run' command
 * Creates environment  and runs model with given spec
 */
class IhRun: public InputHandler
{
    public:
	static const string command() { return "run";}
	static InputHandler* create(Monitor& aHost) { return new IhRun(aHost);}

	IhRun(Monitor& aHost): InputHandler(aHost) {}
	virtual bool handle(const string& aArgs) override {
	    // Init env and run
	    mHost.initEnv();
	    mHost.runModel();
	    bool sdres = mHost.saveProfilerData();
	    if (!sdres) {
		cout << "Error on saving profile data to file";
	    }
	}
};

/** Handler of 'save' command
 * Saves models spec (OSM)
 */
class IhSave: public InputHandler
{
    public:
	static const string command() { return "save";}
	static InputHandler* create(Monitor& aHost) { return new IhSave(aHost);}
	IhSave(Monitor& aHost): InputHandler(aHost) {}
	virtual bool handle(const string& aArgs) override {
	    bool res = mHost.saveModel(aArgs);
	    return res;
	}
};


const Monitor::TIhReg Monitor::mIhReg = {
    {IhRun::command(), IhRun::create},
    Item<IhSave>()
};


