#include <cstdio>
#include <ctime>
#include <time.h>

#include "profiler.h"
#include "guri.h"
#include "melem.h"

using namespace std;


MPind::TClock Pind::getClockResolution() const
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


GProfiler::GProfiler(MEnv* aEnv, const TIdata& aInitData): mEnv(aEnv)
{
    for (auto idata : aInitData) {
	Pind* ind = nullptr; 
	if (idata.first == EPiid_Clock) {
	    ind = new PindClock(static_cast<const PindClock::Idata&>(*idata.second));
	}
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
	Pind* ind = indit.second;
	// Adding suffix to file name */
	string efname = aFileName + "~" + ind->getFileSuf() + ".csv";
	ind->saveToFile(efname);
    }
    /*
    FILE* fp = fopen(aPath.c_str(), "w+");
    if (fp) {
	for (int count = 0; count <= mPos; count++) {
	    TPrec& pev = mBuf[count];
	    // Record data
	    string recs = ToString(pev);
	    // Relative data
	    TPrec* brec = FindBaseRec(count);
	    if (brec) {
		// Base event
		TEventId beid = GetEvent(pev.mEventId).mBaseId;
		const TEvent& be = GetEvent(beid);
		recs += to_string(beid);
		recs += TRec::KFieldSep;
		recs +=  (beid != TEvent::KUndefEid) ?  be.mDescription : "Undefined";
		recs += TRec::KFieldSep;
		// Relative clock
		recs += to_string(pev.mClock - brec->mClock);
		recs += TRec::KFieldSep;
	    }
	    recs += "\n";
	    int fpres = fputs(recs.c_str(), fp);
	    if (fpres == EOF) {
		res = false; break;
	    }

	}
    } else {
	res = false;
    }
    */
    return res;
}


Pind::TClock Pind::GetClock() const
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

/*
MProfiler::TEventId GProfiler::RegisterEvent(const TPEvent& aEvent)
{
    TEventId res = 0;
    mEvents.push_back(aEvent);
    res = mEvents.size() - 1;
    return res;
}
*/


#if 0
string GProfiler::ToString(const TRec& aRec) const
{
    string res;
    // Event
    TEventId eid = aRec.mEventId;
    res += to_string(eid);
    res += TRec::KFieldSep;
    string descr = GetEvent(eid).mDescription;
    res += descr;
    res += TRec::KFieldSep;
    // Uri
    string suri;
    if (aRec.mNode) {
	GUri uri;
	aRec.mNode->GetUri(uri);
	suri = uri.toString(ETrue);
    }
    res += suri;
    res += TRec::KFieldSep;
    // Absolute clock
    res += to_string(aRec.mClock);
    res += TRec::KFieldSep;
    return res;
}

void GProfiler::Rec(TEventId aEventId, MElem* aNode)
{
    TRec& rec = GetRec();
    rec.setEventId(aEventId);
    rec.setNode(aNode);
}

MProfiler::TRec* GProfiler::FindBaseRec(int aPos) const
{
    TRec* res = nullptr;
    TRec& cur = mBuf[aPos];
    TEventId base = GetEvent(cur.mEventId).mBaseId;
    for (int ct = aPos - 1; ct >= 0; ct--) {
	TRec* rec = &mBuf[ct];
	if (rec->mEventId == base) {
	    res = rec; break;
	}
    }
    return res;
}
#endif

bool Pind::saveToFile(const std::string& aPath)
{
    bool res = true;
    FILE* fp = fopen(aPath.c_str(), "w+");
    if (fp) {
	for (int count = 0; count <= mPos; count++) {
	    // Record data
	    string recs = recToString(count);
	    recs += "\n";
	    int fpres = fputs(recs.c_str(), fp);
	    if (fpres == EOF) {
		res = false; break;
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

PRec* PindClock::NewRec()
{
    PRecClock* res = nullptr;
    if (mPos < (mBufLenLim - 1)) {
	mPos++;
	res = static_cast<PRecClock*>(&mBuf[mPos]);
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
    res = to_string(rec.mEventId) + PRec::KFieldSep + event.mDescription + PRec::KFieldSep + to_string(rec.mClock) + PRec::KFieldSep;
    return res;
}


PindDur::PindDur(const PindDur::Idata& aIdata): Pind(aIdata.mDescription, -1)
{
    for (const PEventDur& ev : aIdata.mEvents) {
	mEvents.insert(TEventsMapElem(ev.mId, &ev));
	mBuf.insert(TBufElem(ev.mId, {0,0}));
    }
}

PindDur::~PindDur()
{
}

const PEvent& PindDur::getEvent(PEvent::TId aId) const
{
    __ASSERT(mEvents.count(aId));
    return *mEvents.at(aId);
}

void PindDur::Rec(PEvent::TId aEventId, bool aStart)
{
    __ASSERT(mBuf.count(aEventId) > 0);
    Pind::TClock cl = GetClock();
    TBufData& data = mBuf.at(aEventId);
    if (aStart) {
	data.mStartClock = cl;
    } else {
	__ASSERT(data.mStartClock != 0);
	data.mDur = cl = data.mStartClock;
    }
}
