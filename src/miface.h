#ifndef __GRAYB_MIFACE_H
#define __GRAYB_MIFACE_H

#include <string>

using namespace std;

class MIface;

/**
 * @brief Interface provider
 */
class MIfaceProv
{
    public:
	virtual MIface* DoGetIface(const string& aName) = 0;
};

/**
 * @brief Interface of interface. Support unified invocation of methods
 */
class MIface
{
    public:
//	static const char* Type() { return "MIface";};
	// Invocation of iface method: aRes - data of result, returns context or NULL otherwise
	virtual MIface* Call(const string& aSpec, string& aRes) = 0;
	// Getting UID - id unique in the scope of env
	virtual string Uid() const { return string();};
	virtual string Mid() const = 0;
	// TODO to introduce GUID - ID unique in the scope of the whole model
};

class MIfaceResolver
{
    public:
	static const char* Type() { return "MIfaceResolver";};
	virtual MIface* GetIfaceByUid(const string& aUid) = 0;
};

#endif
