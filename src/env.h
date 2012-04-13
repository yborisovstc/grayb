#ifndef __GRAYB_ENV_H
#define __GRAYB_ENV_H

#include "menv.h"
#include "mgraph.h"

class GProvider;
class GFactory;
class Elem;
class GLogRec;

class Env: public Base, public MEnv
{
public:
	static const char* Type() { return "GEnv";};
	Env(const string& aName, const string& aSpecFile, const string& aLogFileName = string());
	virtual ~Env();
	inline Elem* Root();
	// Separated from constructor because constr can be followed by second phase - setting providers etc.
	void ConstructSystem();
	void AddProvider(GProvider* aProv);
public:
	// From Base
	virtual void *DoGetObj(const char *aName, TBool aIncUpHier = ETrue);
	// From MEnv
	virtual MProvider *Provider() const;
	virtual MLogRec *Logger();
private:
	GLogRec* iLogger; 
	Elem* iRoot;
	GFactory *iProvider;
	string iSystSpec;
};

inline Elem* Env::Root() { return iRoot; }


#endif
