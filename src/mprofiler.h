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
	PRec(): mEventId(PEvent::KUndefEid) {}
	PRec(PEvent::TId aId): mEventId(aId) {}
	void setEventId(PEvent::TId aId) { mEventId = aId;}
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
	/** Id of indicator type */
	using TTid = int;
    public:
	virtual string getDescription() = 0;
	virtual int getBufLen() const = 0;
	virtual int getBufLenLimit() const = 0;
	/** Returns file suffix */
	virtual string getFileSuf() const = 0;
	/** Obtains clock resolution in nanoseconds */
	virtual TClock getClockResolution() const =  0;
	virtual string recToString(int aRecNum) const = 0;
	virtual bool saveToFile(const std::string& aPath) = 0;
	virtual void dump() const = 0;
};

/** Performace indicators recorder: Clock
 * Records time stamps of event with assosiation to node
*/
class MPClock
{
    public:
	virtual void operator()(PEvent::TId aEventId, MElem* aNode) = 0;
};

/** Performace indicators recorder: Duration
 * Records duration between start and end events with assosiation to node
*/
class MPDur
{
    public:
	virtual void operator()(PEvent::TId aEventId, MElem* aNode) = 0;
};

/** Performace indicators recorder: Duration statistic
 * Records stat of duration between start and end events
*/
class MPDurStat
{
    public:
	virtual void operator()(PEvent::TId aEventId, bool aStart = false) = 0;
};


/** Profiler interface */
class MProfiler
{
    public:
	virtual void Enable() = 0;
	virtual bool SaveToFile(const std::string& aPath) = 0;
	/** Gets pointer to indicator by Id */
	virtual MPind* getPind(int aId) = 0;
	/** Gets Clock ind recorder */
	virtual MPClock& Clock() = 0;
	/** Gets duration ind recorder */
	virtual MPDur& Dur() = 0;
	/** Gets duration stat ind recorder */
	virtual MPDurStat& DurStat() = 0;
	/** Get Pid */
	template <typename T> T& Ind(const T&a) { return *dynamic_cast<T*>(getPind(T::KId));}
};


#endif 
