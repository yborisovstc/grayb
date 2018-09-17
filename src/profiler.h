#ifndef __GRAYB_PROFILER_H
#define __GRAYB_PROFILER_H

#include <vector>
#include <map>

#include "mprofiler.h"

class MEnv;

enum {
    EPiid_Clock = 0, /** Clock */
    EPiid_Dur = 1,   /** Duration */
} TPindId;


/** Performance indicator. Base */
class Pind: public MPind
{
    public:
	/** Type of events register */
	using TEventsMap = std::map<PEvent::TId, PEvent*>;
	using TEventsMapElem = std::pair<PEvent::TId, PEvent*>;
	using TEvents = std::vector<PEvent>;
	/** Initialization data */
	struct Idata {
	    Idata(const string& aDescription, int aBufLim, const TEvents* aEvents): mDescription(aDescription), mBufLim(aBufLim), mEvents(aEvents) {}
	    string mDescription; /** Description */
	    int mBufLim; /** Buffer size limit */
	    const TEvents* mEvents; /** Events map */
	};
    public:
	Pind(const string& aDescription, int aBufLenLIm, const TEvents& aEvents);
	Pind(const Idata& aIdata): Pind(aIdata.mDescription, aIdata.mBufLim, *(aIdata.mEvents)) {}
	virtual ~Pind() { delete mBuf;}
    public:
	// From MPind
	virtual string getDescription() override { return mDescription;}
	virtual int getBufLen() const override { return -1;}
	virtual int getBufLenLimit() const override { return mBufLenLim;}
	virtual const PEvent& getEvent(PEvent::TId aId) const override;
	/** Obtains clock resolution in nanoseconds */
	virtual TClock getClockResolution() const override;
	virtual bool saveBufToFile(const string& aFilePath) const { return false;}
	virtual PRec* NewRec() override;
    protected:
	/** Gets clock absolute value */
	TClock GetClock() const;
    protected:
	/** Events buffer */
	PRec* mBuf;
	/** Limit of buf len - predefined size of buffer */
	int mBufLenLim;
	/** Description */
	string mDescription;
	/** Insertion position in events buffer */
	int mPos = -1;
	/** Events registered */
	TEventsMap mEvents;
};

/** Record for performance indicator 'Clock' */
class PRecClock: public PRec
{
    public:
	void setNode(MElem* aNode) { mNode = aNode;}
	void setClock(Pind::TClock aClock) { mClock = aClock;}
    public:
	/** Node for that record is created */
	MElem* mNode = nullptr;
	// Clock value, in ns
	Pind::TClock mClock = 0;
};


/** Performance indicator: clock value */
class PindClock: public Pind
{
    public:
	struct Idata: public Pind::Idata {
	    Idata(const string& aDescription, int aBufLim, const TEvents* aEvents): Pind::Idata(aDescription, aBufLim, aEvents) {}
	};
    public:
	PindClock(const Idata& aIdata): Pind(aIdata) {}
	static const int KId = EPiid_Clock;
	void Rec(PEvent::TId aEventId, MElem* aNode);
	virtual string recToString(const PRec& aRec) const override;
    public:
	virtual PRec* NewRec();
};

/** Performance indicator: event to event duration */
class PindDur: public PindClock
{
    public:
	static const int KId = EPiid_Dur;
    protected:
	/** Finds nearest lower record for base event */
	PRec* FindBaseRec(int aPos) const;
};



/** Default profiler */
class GProfiler: public MProfiler
{
    public:
	/** Type of performance indicators register
	 *  The key is id of indicator
	 */
	using TPinds = std::map<MPind::TTid, Pind*>;
	using TPindsElem = std::pair<MPind::TTid, Pind*>;
    public:
	/** Initialzation data */
	using TIdata = map<MPind::TTid, const Pind::Idata*>;
    public:
	GProfiler(MEnv* aEnv, const TIdata& aInitData);
	virtual ~GProfiler();
	// From MProfiler
	virtual void Enable() override;
	virtual bool SaveToFile(const std::string& aPath) override;
	/** Gets pointer to Pid by Id */
	virtual MPind* getPind(int aId) override;
    protected:
	/** System environment */
	MEnv* mEnv;
	/** Performance indicators */
	TPinds mPinds;
	/** Clock indicator */
	PindClock* mPindClock;
};


#endif
