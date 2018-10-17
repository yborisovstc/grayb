#ifndef __FAPM_MONITOR__
#define __FAPM_MONITOR__

#include <string>

#include <env.h>

using namespace std;

/**
 * \brief System movement monitor
 */
class Monitor
{
    public:
	Monitor();
	~Monitor();
	void setSpecName(const string& aFileName);
	bool setLogFile(const string& aPath);
	bool setProfPath(const string& aPath);
	void initEnv();
	void runModel();
	bool saveProfilerData();
	/** Runs user interaction loop */
	bool run();
    protected:
	string mSpecName;
	string mLogName;
	/** Profiler file path */
	string mProfName;
	Env* mEnv;
	string mPrompt;
	static const string KDefPrompt;
};

/** Input handler base */
class InputHandler
{
    public:
	InputHandler(Monitor& aHost): mHost(aHost) {}
	virtual ~InputHandler() {}
	virtual bool handle() = 0;
    protected:
	Monitor& mHost;
};

#endif // __FAPM_MONITOR__
