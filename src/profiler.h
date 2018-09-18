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
	/** Initialization data */
	struct Idata {
	    Idata(const string& aDescription): mDescription(aDescription) {}
	    string mDescription; /** Description */
	};
    public:
	Pind(const string& aDescription, int aBufLenLIm): mDescription(aDescription), mBufLenLim(aBufLenLIm) {}
	virtual ~Pind() {}
    public:
	// From MPind
	virtual string getDescription() override { return mDescription;}
	virtual int getBufLen() const override { return -1;}
	virtual int getBufLenLimit() const override { return mBufLenLim;}
	/** Obtains clock resolution in nanoseconds */
	virtual TClock getClockResolution() const override;
	virtual bool saveBufToFile(const string& aFilePath) const { return false;}
	virtual bool saveToFile(const std::string& aPath) override;
	//virtual PRec* NewRec() override;
	virtual void dump() const override {}
    protected:
	/** Gets clock absolute value */
	TClock GetClock() const;
    protected:
	/** Description */
	string mDescription;
	/** Limit of buf len - predefined size of buffer */
	int mBufLenLim;
	/** Insertion position in events buffer */
	int mPos = -1;
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
	static const int KId = EPiid_Clock;
	/** Type of events register */
	using TEventsMap = std::map<PEvent::TId, const PEvent*>;
	using TEventsMapElem = std::pair<PEvent::TId, const PEvent*>;
	using TEvents = std::vector<PEvent>;
    public:
	/** Initialization data */
	struct Idata: public Pind::Idata {
	    Idata(const string& aDescription, int aBufLim, const TEvents* aEvents): Pind::Idata(aDescription), mBufLim(aBufLim), mEvents(aEvents) {}
	    string mDescription; /** Description */
	    int mBufLim; /** Buffer size limit */
	    const TEvents* mEvents; /** Events map */
	};
    public:
	PindClock(const Idata& aIdata);
	virtual ~PindClock();
	void Rec(PEvent::TId aEventId, MElem* aNode);
	// From MPind
	virtual const PEvent& getEvent(PEvent::TId aId) const override;
	virtual string recToString(int aRecNum) const override;
	virtual string getFileSuf() const override { return "clock";}
    public:
	virtual PRec* NewRec();
    protected:
	/** Events registered */
	TEventsMap mEvents;
	/** Events buffer */
	PRecClock* mBuf;
};

/** Event for duration indicator */
class PEventDur: public PEvent
{
    public:
	/** Constructor
	 * @param aStart - sign of measured interval start
	 */
	PEventDur(TId aId = KUndefEid, const string& aDescription = string()): PEvent(aId, aDescription) {} 
    public:
	bool mStart; /** Sign of measured interval start */
};

/** Performance indicator: event to event duration */
class PindDur: public Pind
{
    public:
	static const int KId = EPiid_Dur;
	using TEventsMap = std::map<PEvent::TId, const PEventDur*>;
	using TEventsMapElem = std::pair<PEvent::TId, const PEventDur*>;
	using TEvents = std::vector<PEventDur>;
	struct TBufData { Pind::TClock mStartClock = 0, mDur = 0; TBufData(Pind::TClock aStart, Pind::TClock aDur): mStartClock(aStart), mDur(aDur) {}};
	using TBuf = map<PEvent::TId, TBufData>;
	using TBufElem = pair<PEvent::TId, TBufData>;
    public:
	/** Initialization data */
	struct Idata: public Pind::Idata {
	    Idata(const string& aDescription, const TEvents& aEvents):
		Pind::Idata(aDescription), mEvents(aEvents) {}
	    string mDescription; /** Description */
	    TEvents mEvents; /** Events */
	};
    protected:
	PindDur(const Idata& aIdata);
	virtual ~PindDur();
	void Rec(PEvent::TId aEventId, bool aStart = false);
	/** Finds nearest lower record for base event */
	PRec* FindBaseRec(int aPos) const;
	// From MPind
	virtual const PEvent& getEvent(PEvent::TId aId) const override;
    protected:
	/** Events registered */
	TEventsMap mEvents;
	TBuf mBuf;

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
