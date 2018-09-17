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
	using TEvents = std::map<PEvent::TId, PEvent::Idata>;
	/** Initialization data */
	struct Idata {
	    int mTid; /** Id of indicator type */
	    TEvents mEvents; /** Events map */
	};
    public:
	Pind(int aBufLenLIm, const TEvents& aEvents): mBuf(nullptr), mBufLenLim(aBufLenLIm), mEvents(aEvents) {}
	virtual ~Pind() { delete mBuf;}
    public:
	// From MPind
	virtual int getBufLen() const override { return -1;}
	virtual int getBufLenLimit() const override { return mBufLenLim;}
	/** Obtains clock resolution in nanoseconds */
	virtual TClock getClockResolution() const override;
	virtual bool saveBufToFile(const string& aFilePath) const { return false;}
	virtual PRec& NewRec() override;
    protected:
	/** Gets clock absolute value */
	TClock GetClock() const;
    protected:
	/** Events buffer */
	PRec* mBuf;
	/** Limit of buf len - predefined size of buffer */
	int mBufLenLim;
	/** Insertion position in events buffer */
	int mPos = -1;
	/** Events registered */
	const TEvents& mEvents;
};

/** Record for performance indicator 'Clock' */
class PRecClock: public PRec
{
    public:
	void setNode(MElem* aNode) { mNode = aNode;}
	void setClock(Pind::TClock aClock) { mClock = aClock;}
	void Rec(PEvent::TId aEventId, MElem* aNode);
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
	struct Idata: public Pind::Idata {};
    public:
	static const int KId = EPiid_Clock;
    public:
	PRecClock& NewRec();
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
	using TPinds = std::map<int, Pind*>;
	using TPindsElem = std::pair<int, Pind*>;
    public:
	/** Initialzation data */
	using TIdata = vector<const Pind::Idata*>;
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
