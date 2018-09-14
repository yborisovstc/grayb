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
    protected:
	string mSpecName;
	string mLogName;
	/** Profiler file path */
	string mProfName;
	Env* mEnv;
};

#endif // __FAPM_MONITOR__
