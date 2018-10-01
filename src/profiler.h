#ifndef __GRAYB_PROFILER_H
#define __GRAYB_PROFILER_H

#include <vector>
#include <map>

#include "mprofiler.h"

class MEnv;

enum {
    EPiid_Clock = 0, /** Clock */
    EPiid_Dur = 1,   /** Duration */
    EPiid_DurStat = 2,   /** Duration statistic */
} TPindId;

/** Performance indicator. Base */
class PindBase: public MPind
{
    public:
	/** Initialization data */
	struct Idata {
	    Idata(const string& aDescription): mDescription(aDescription) {}
	    string mDescription; /** Description */
	};
    public:
	PindBase(const string& aDescription): mDescription(aDescription) {}
	virtual ~PindBase() {}
	// From MPind
	virtual string getDescription() { return mDescription;}
	/** Obtains clock resolution in nanoseconds */
	virtual TClock getClockResolution() const override;
	virtual void dump() const override {}
    protected:
	/** Gets clock absolute value */
	TClock GetClock() const;
	static string field(const string& aField) { return aField + PRec::KFieldSep;}
    protected:
	/** Description */
	string mDescription;
};


/** Performance indicator. Linear buffer based  */
class Pind: public PindBase
{
    public:
	Pind(const string& aDescription, int aBufLenLIm): PindBase(aDescription), mBufLenLim(aBufLenLIm) {}
	virtual ~Pind() {}
    public:
	// From MPind
	virtual string getDescription() override { return mDescription;}
	virtual int getBufLen() const override { return -1;}
	virtual int getBufLenLimit() const override { return mBufLenLim;}
	virtual bool saveToFile(const std::string& aPath) override;
	virtual void dump() const override {}
    protected:
	/** Limit of buf len - predefined size of buffer */
	int mBufLenLim;
	/** Insertion position in events buffer */
	int mPos = -1;
};

/** Record for performance indicator 'Clock' */
class PRecClock: public PRec
{
    public:
	PRecClock(): mClock(0), mNode(nullptr) {}
	PRecClock(PEvent::TId aEid, MPind::TClock aClock, MElem* aNode): PRec(aEid), mClock(aClock), mNode(aNode) {}
	void setNode(MElem* aNode) { mNode = aNode;}
	void setClock(Pind::TClock aClock) { mClock = aClock;}
    public:
	// Clock value, in ns
	Pind::TClock mClock = 0;
	/** Node for that record is created */
	MElem* mNode = nullptr;
};


/** Performance indicator: clock value */
class PindClock: public Pind, public MPClock
{
    public:
	static const int KId = EPiid_Clock;
	/** Type of events register */
	using TPEvent = PEvent;
	using TEventsMap = std::map<PEvent::TId, const PEvent*>;
	using TEventsMapElem = std::pair<PEvent::TId, const PEvent*>;
	using TEvents = std::vector<PEvent>;
    public:
	/** Initialization data */
	struct Idata: public PindBase::Idata {
	    Idata(const string& aDescription, int aBufLim, const TEvents* aEvents):
		PindBase::Idata(aDescription), mBufLim(aBufLim), mEvents(aEvents) {}
	    string mDescription; /** Description */
	    int mBufLim; /** Buffer size limit */
	    const TEvents* mEvents; /** Events map */
	};
    public:
	PindClock(const Idata& aIdata);
	virtual ~PindClock();
	void Rec(PEvent::TId aEventId, MElem* aNode);
	// From MPind
	virtual const PEvent& getEvent(PEvent::TId aId) const;
	virtual string recToString(int aRecNum) const override;
	virtual string getFileSuf() const override { return "clock";}
	// From MPClock
	virtual void operator()(PEvent::TId aEventId, MElem* aNode) { *NewRec() = {aEventId, GetClock(), aNode};}
    protected:
	virtual PRecClock* NewRec();
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
	PEventDur(TId aId = KUndefEid, const string& aDescription = string(), PEvent::TId aStartEvent = KUndefEid):
	    PEvent(aId, aDescription), mStartEvent(aStartEvent) {} 
    public:
	PEvent::TId mStartEvent; /** Events to start duration measure */
};

/** Performance indicator: event to event duration */
class PindDur: public Pind, public MPDur
{
    public:
	static const int KId = EPiid_Dur;
	/** Type of events register */
	using TPEvent = PEventDur;
	using TPRec = PRecClock;
	using TEventsMap = std::map<PEvent::TId, const TPEvent*>;
	using TEventsMapElem = std::pair<PEvent::TId, const TPEvent*>;
	using TEvents = std::vector<TPEvent>;
    public:
	/** Initialization data */
	struct Idata: public PindBase::Idata {
	    Idata(const string& aDescription, int aBufLim, const TEvents& aEvents):
		PindBase::Idata(aDescription), mBufLim(aBufLim), mEvents(aEvents) {}
	    int mBufLim; /** Buffer size limit */
	    const TEvents mEvents; /** Events map */
	};
    public:
	PindDur(const Idata& aIdata);
	virtual ~PindDur();
	void Rec(PEvent::TId aEventId, MElem* aNode);
	// From MPind
	virtual const TPEvent& getEvent(PEvent::TId aId) const;
	virtual string recToString(int aRecNum) const override;
	virtual string getFileSuf() const override { return "dur";}
	// From MPDur
	virtual void operator()(PEvent::TId aEventId, MElem* aNode) override { *NewRec() = {aEventId, GetClock(), aNode};}
    protected:
	virtual TPRec* NewRec();
	/** Finds nearest lower record for start event */
	TPRec* findStartRec(int aPos) const;
    protected:
	/** Events registered */
	TEventsMap mEvents;
	/** Events buffer */
	TPRec* mBuf;
};



/** Event for duration stat indicator */
class PEventDurStat: public PEvent
{
    public:
	/** Constructor
	 * @param aStart - sign of measured interval start
	 */
	PEventDurStat(TId aId = KUndefEid, const string& aDescription = string()): PEvent(aId, aDescription) {} 
    public:
	bool mStart; /** Sign of measured interval start */
};

/** Performance statistical indicator: event to event duration */
class PindDurStat: public PindBase, public MPDurStat
{
    public:
	static const int KId = EPiid_DurStat;
	using TPEvent = PEventDurStat;
	using TEventsMap = std::map<PEvent::TId, const TPEvent*>;
	using TEventsMapElem = std::pair<PEvent::TId, const TPEvent*>;
	using TEvents = std::vector<TPEvent>;

	struct TBufData {
	    int mLevel; /** Level of resursion */
	    Pind::TClock mStart = 0; /** Start clock */
	    int mEventsNum = 0;  /** Num of events triggered */
	    Pind::TClock mCumDur = 0; /** Cumulative duration */
	    TBufData(Pind::TClock aStart, int aEventsNum, Pind::TClock aDur):
		mLevel(0), mStart(aStart), mEventsNum(aEventsNum), mCumDur(aDur) {}
	};

	using TPRec = TBufData;
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
    public:
	PindDurStat(const Idata& aIdata);
	virtual ~PindDurStat();
	void Rec(PEvent::TId aEventId, bool aStart = false);
	// From MPind
	virtual int getBufLen() const { return mBuf.size();}
	virtual int getBufLenLimit() const { return -1;}
	virtual const TPEvent& getEvent(PEvent::TId aId) const;
	virtual string getFileSuf() const override { return "durstat";}
	virtual bool saveToFile(const std::string& aPath) override;
	virtual string recToString(PEvent::TId aRecId) const override;
	// From MPDurStat
	virtual inline void operator()(PEvent::TId aEventId, bool aStart = false) override { Rec(aEventId, aStart);}
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
	using TPinds = std::map<MPind::TTid, PindBase*>;
	using TPindsElem = std::pair<MPind::TTid, PindBase*>;
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
	virtual inline MPClock& Clock() override { return *mPClock;}
	virtual inline MPDur& Dur() override { return *mPDur;}
	virtual MPDurStat& DurStat() override {return *mPDurStat;}
	PindDur* getPindDur() { return mPinds.count(EPiid_Dur) > 0 ? dynamic_cast<PindDur*>(mPinds.at(EPiid_Dur)) : nullptr;}
    protected:
	/** System environment */
	MEnv* mEnv;
	/** Performance indicators */
	TPinds mPinds;
	/** Clock ind recorder */
	MPClock* mPClock;
	/** Duration ind recorder */
	MPDur* mPDur;
	/** Duration stat ind recorder */
	MPDurStat* mPDurStat;
};


#endif
