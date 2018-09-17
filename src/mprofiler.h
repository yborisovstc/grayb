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
	virtual const PEvent& getEvent(PEvent::TId aId) const = 0;
	/** Obtains clock resolution in nanoseconds */
	virtual TClock getClockResolution() const =  0;
	virtual bool saveBufToFile(const string& aFilePath) const = 0;
	virtual PRec* NewRec() = 0;
	virtual string recToString(const PRec& aRec) const;
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
	template <typename T> T& Ind(const T&a) { return *dynamic_cast<T*>(getPind(T::KId));}
};


#endif 
