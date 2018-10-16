#include <cstdio>
#include <ctime>
#include <time.h>

#include "profiler.h"
#include "guri.h"
#include "melem.h"

using namespace std;


MPind::TClock PindBase::getClockResolution() const
{
    int res = -1;
    clockid_t cid;
    struct timespec ts;
    int rr = pthread_getcpuclockid(pthread_self(), &cid);
    if (rr == 0) {
	rr = clock_getres(cid, &ts);
	res = ts.tv_sec * 1000000000 + ts.tv_nsec;
    }
    return res;
}

MPind::TClock PindBase::GetClock() const
{
    Pind::TClock res = -1;
    clockid_t cid;
    struct timespec ts;
    int s = pthread_getcpuclockid(pthread_self(), &cid);
    if (s == 0) {
	s = clock_gettime(cid, &ts);
	res = ts.tv_sec * 1000000000 + ts.tv_nsec;
    }
    return res;
}



GProfiler::GProfiler(MEnv* aEnv, const TIdata& aInitData): mEnv(aEnv),
    mPClock(nullptr), mPDur(nullptr), mPDurStat(nullptr)
{
    for (auto idata : aInitData) {
	PindBase* ind = nullptr; 
	if (idata.first == EPiid_Clock) {
	    ind = new PindClock(static_cast<const PindClock::Idata&>(*idata.second));
	    mPClock = dynamic_cast<MPClock*>(ind);
	    __ASSERT(mPClock);
	} else if (idata.first == EPiid_Dur) {
	    ind = new PindDur(static_cast<const PindDur::Idata&>(*idata.second));
	    mPDur = dynamic_cast<MPDur*>(ind);
	    __ASSERT(mPClock);
	} else if (idata.first == EPiid_DurStat) {
	    const PindDurStat::Idata& initd = static_cast<const PindDurStat::Idata&>(*idata.second);
	    ind = new PindDurStat(initd);
	    mPDurStat = dynamic_cast<MPDurStat*>(ind);
	    __ASSERT(mPDurStat);
	}
	auto resl = ind->getClockResolution();
	mPinds.insert(TPindsElem(idata.first, ind));
    }
}

GProfiler::~GProfiler()
{
}

void GProfiler::Enable()
{
}

MPind* GProfiler::getPind(int aId)
{
    __ASSERT(mPinds.count(aId));
    return mPinds.at(aId);
}


bool GProfiler::SaveToFile(const string& aFileName)
{
    bool res = true;
    for (auto indit : mPinds) {
	PindBase* ind = indit.second;
	// Adding suffix to file name */
	string efname = aFileName + "~" + ind->getFileSuf() + ".csv";
	ind->saveToFile(efname);
    }
    return res;
}




bool Pind::saveToFile(const std::string& aPath)
{
    bool res = true;
    FILE* fp = fopen(aPath.c_str(), "w+");
    if (fp) {
	for (int count = 0; count <= mPos; count++) {
	    // Record data
	    string recs = recToString(count);
	    if (!recs.empty()) {
		recs += "\n";
		int fpres = fputs(recs.c_str(), fp);
		if (fpres == EOF) {
		    res = false; break;
		}
	    }
	}
    } else {
	res = false;
    }
    return res;
}


PindClock::PindClock(const Idata& aIdata): Pind(aIdata.mDescription, aIdata.mBufLim)
{
    __ASSERT(mBufLenLim > 0);
    mBuf = new PRecClock[mBufLenLim];
    for (const PEvent& ev : *(aIdata.mEvents)) {
	mEvents.insert(TEventsMapElem(ev.mId, &ev));
    }
}

PindClock::~PindClock()
{
    delete mBuf;
}

const PEvent& PindClock::getEvent(PEvent::TId aId) const
{
    __ASSERT(mEvents.count(aId));
    return *mEvents.at(aId);
}

PRecClock* PindClock::NewRec()
{
    PRecClock* res = nullptr;
    if (mPos < (mBufLenLim - 1)) {
	mPos++;
	res = &mBuf[mPos];
	res->setClock(GetClock());
    }
    return res;
}


void PindClock::Rec(PEvent::TId aEventId, MElem* aNode)
{
    PRec* rec = NewRec();
    if (rec) {
	PRecClock* pcrec = static_cast<PRecClock*>(rec);
	rec->setEventId(aEventId);
	pcrec->setNode(aNode);
    }
}

string PindClock::recToString(int aRecNum) const
{
    string res;
    __ASSERT(aRecNum <= mPos);
    PRecClock& rec = mBuf[aRecNum];
    const PEvent& event = getEvent(rec.mEventId);
    string suri;
    if (rec.mNode) {
	GUri uri;
	rec.mNode->GetUri(uri);
	suri = uri.toString(ETrue);
    }
    res = field(to_string(rec.mEventId)) + field(event.mDescription) + field(suri) + field(to_string(rec.mClock));
    return res;
}

PindDur::PindDur(const Idata& aIdata): Pind(aIdata.mDescription, aIdata.mBufLim)
{
    __ASSERT(mBufLenLim > 0);
    mBuf = new TPRec[mBufLenLim];
    for (const TPEvent& ev : aIdata.mEvents) {
	mEvents.insert(TEventsMapElem(ev.mId, &ev));
    }
}

PindDur::~PindDur()
{
}

void PindDur::Rec(PEvent::TId aEventId, MElem* aNode)
{
    TPRec* rec = NewRec();
    if (rec) {
	rec->setEventId(aEventId);
	rec->setNode(aNode);
    }
}

PindDur::TPRec* PindDur::NewRec()
{
    TPRec* res = nullptr;
    if (mPos < (mBufLenLim - 1)) {
	mPos++;
	res = &mBuf[mPos];
	res->setClock(GetClock());
    } 
    return res;
}

string PindDur::recToString(int aRecNum) const
{
    string res;
    __ASSERT(aRecNum <= mPos);
    TPRec& rec = mBuf[aRecNum];
    const TPEvent& event = getEvent(rec.mEventId);
    string suri;
    if (event.mStartEvent != PEvent::KUndefEid) { // Omitting start events record
	if (rec.mNode) {
	    GUri uri;
	    rec.mNode->GetUri(uri);
	    suri = uri.toString(ETrue);
	}
	const TPRec* startrec = findStartRec(aRecNum);
	MPind::TClock dur = rec.mClock - startrec->mClock;

	res = field(to_string(rec.mEventId)) + field(event.mDescription) + field(suri) + field(to_string(dur));
    }
    return res;
}

PindDur::TPRec* PindDur::findStartRec(int aPos) const
{
    TPRec* res = nullptr;
    TPRec& cur = mBuf[aPos];
    PEvent::TId start = getEvent(cur.mEventId).mStartEvent;
    for (int ct = aPos - 1; ct >= 0; ct--) {
	TPRec* rec = &mBuf[ct];
	if (rec->mEventId == start) {
	    res = rec; break;
	}
    }
    return res;
}

const PindDur::TPEvent& PindDur::getEvent(PEvent::TId aId) const
{
    __ASSERT(mEvents.count(aId));
    return *mEvents.at(aId);
}




PindDurStat::PindDurStat(const PindDurStat::Idata& aIdata): PindBase(aIdata.mDescription)
{
    for (const TPEvent& ev : aIdata.mEvents) {
	mEvents.insert(TEventsMapElem(ev.mId, &ev));
	mBuf.insert(TBufElem(ev.mId, {0, 0, 0}));
    }
}

PindDurStat::~PindDurStat()
{
}

const PindDurStat::TPEvent& PindDurStat::getEvent(PEvent::TId aId) const
{
    __ASSERT(mEvents.count(aId));
    return *mEvents.at(aId);
}

void PindDurStat::Rec(PEvent::TId aEventId, bool aStart)
{
    __ASSERT(mBuf.count(aEventId) > 0);
    TBufData& data = mBuf.at(aEventId);
    if (aStart) {
	if (data.mLevel++ == 0) {
	    Pind::TClock cl = GetClock();
	    data.mStart = cl;
	}
    } else if (--data.mLevel == 0) {
	__ASSERT(data.mStart != 0);
	data.mEventsNum++;
	Pind::TClock cl = GetClock();
	data.mCumDur += cl - data.mStart;
    }
}

string PindDurStat::recToString(PEvent::TId aRecId) const
{
    string res;
    const TPRec& rec = mBuf.at(aRecId);
    const TPEvent& event = getEvent(aRecId);
    string avdurs = rec.mEventsNum > 0 ? to_string(rec.mCumDur/rec.mEventsNum) : "n/a";

    res = field(to_string(event.mId)) + field(event.mDescription) + field(to_string(rec.mCumDur)) +
	field(to_string(rec.mEventsNum)) + field(avdurs);
    return res;
}

bool PindDurStat::saveToFile(const std::string& aPath)
{
    bool res = true;
    FILE* fp = fopen(aPath.c_str(), "w+");
    if (fp) {
	for (auto irec : mBuf) {
	    // Record data
	    string recs = recToString(irec.first);
	    if (!recs.empty()) {
		recs += "\n";
		int fpres = fputs(recs.c_str(), fp);
		if (fpres == EOF) {
		    res = false; break;
		}
	    }
	}
    } else {
	res = false;
    }
    return res;
}

