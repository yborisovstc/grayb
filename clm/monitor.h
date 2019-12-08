#ifndef __FAPM_MONITOR__
#define __FAPM_MONITOR__

#include <string>
#include <map>

#include <env.h>

using namespace std;

class InputHandler;

/**
 * \brief System movement monitor
 */
class Monitor
{
    public:
	/** Input handler factory function */
	using TIhFact = InputHandler* (Monitor& aHost);
	/** Input handlers registry type */
	using TIhReg = map<string, TIhFact*>;
    public:
	Monitor();
	~Monitor();
	void setSpecName(const string& aFileName);
	/** @brief Sets converted spec file name */
	void setOutSpecName(const string& aFileName);
	bool setLogFile(const string& aPath);
	bool setProfPath(const string& aPath);
	void initEnv();
	void runModel();
	bool saveModel(const string& aPath);
	bool saveProfilerData();
	/** @brief Runs user interaction loop */
	bool run();
	/** @brief Converts chromo spec
	 * @return Sign of sucess
	 * */
	bool convertSpec();
    protected:
	/** Crates input handler for given command */
	InputHandler* createHandler(const string& aCmd);
    protected:
	string mSpecName;  //!< Input spec file name
	string mCSpecName; //!< Convrted spec file name
	string mLogName;
	/** Profiler file path */
	string mProfName;
	Env* mEnv;
	string mPrompt;
	static const string KDefPrompt;
	/** Input handlers factory function registry */
	static const TIhReg mIhReg;
};

/** Input handler base */
class InputHandler
{
    public:
	/** Each handler has to include two static method 
	 * 1. 'command' returning specific command's string
	 * 2. "create" returning newly created instanse of handler
	 * static const string command();
	 * static InputHandler* create(Monitor& aHost);
	 */
	InputHandler(Monitor& aHost): mHost(aHost) {}
	virtual ~InputHandler() {}
	/** Handles command with given arguments aArgs */
	virtual bool handle(const string& aArgs) = 0;
    protected:
	Monitor& mHost;
};

#endif // __FAPM_MONITOR__
