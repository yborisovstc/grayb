#ifndef __GRAYB_MPROFILER_H
#define __GRAYB_MPROFILER_H

#include <string>

class MElem;

using namespace std;

/** Profilers event */
class PEvent
{
    public:
	/** Event identificator type */
	using TId = int;
    public:
	struct Idata {
	    string mDescription;
	};
    public:
	/** Undefined event id */
	static const TId KUndefEid = -1;
	PEvent(TId aId = KUndefEid, const string& aDescription = string()): mId(aId), mDescription(aDescription) {}
	/** Event Id */
	TId mId = KUndefEid;
	/** Event description */
	string mDescription;
};

/** Profilers record, base */
class PRec
{
    public:
	operator string() const;
	void setEventId(PEvent::TId aId) { mEventId = aId;}
	virtual string toString() const { return to_string(mEventId) + KFieldSep;}
    public:
	PEvent::TId mEventId; /** Event Id */
    public:
	static const char KFieldSep = '\t';

};

/** Performance indicator. Interface.
 * Inherited class should contain static int constant KId with unique value
 */
class MPind
{
    public:
	/** Type of clock value */
	using TClock = long int;
    public:
	virtual int getBufLen() const = 0;
	virtual int getBufLenLimit() const = 0;
	virtual const PEvent& GetEvent(PEvent::TId aId) const = 0;
	/** Obtains clock resolution in nanoseconds */
	virtual TClock getClockResolution() const =  0;
	virtual bool saveBufToFile(const string& aFilePath) const = 0;
	virtual PRec& NewRec() = 0;
};

/** Profiler interface */
class MProfiler
{
    public:
	virtual void Enable() = 0;
	virtual bool SaveToFile(const std::string& aPath) = 0;
	/** Gets pointer to Pid by Id */
	virtual MPind* getPind(int aId) = 0;
	/** Get Pid */
	template <typename T> T& Ind(const T&a) { return *dynamic_cast<T*>(getPind(T::Kid));}
};


#endif 
