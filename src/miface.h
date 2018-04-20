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
	// Invocation of iface method: aRes - data of result, returns context or NULL otherwise
	virtual MIface* Call(const string& aSpec, string& aRes) = 0;
	// Getting UID - id unique in the scope of env
	virtual string Uid() const { return string();}
	virtual string Mid() const { return string();}
	// TODO to introduce GUID - ID unique in the scope of the whole model
};

/**
 * @brief Interface of interface. Stub
 */
class MIfaceStub: public MIface
{
    public:
	MIface* Call(const string& aSpec, string& aRes) override { return NULL;}
	string Mid() const override { return "?";}
};


class MIfaceResolver
{
    public:
	static const char* Type() { return "MIfaceResolver";};
	virtual MIface* GetIfaceByUid(const string& aUid) = 0;
};

#endif
