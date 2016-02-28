#ifndef __GRAYB_MIFACE_H
#define __GRAYB_MIFACE_H

/*
 * Interface of interface. Support unified invocation of methods
 */

class MIface
{
    public:
	static const char* Type() { return "MIface";};
	// Invocation of iface method: aRes - data of result, returns context or NULL otherwise
	virtual MIface* Call(const string& aSpec, string& aRes) = 0;
	// Getting UID
	virtual string Uid() const { return string();};
};

#endif