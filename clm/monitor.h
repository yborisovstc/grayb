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
	void initEnv();
	void runModel();
	
    protected:
	string mSpecName;
	string mLogName;
	Env* mEnv;
};

#endif // __FAPM_MONITOR__
