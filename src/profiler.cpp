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


bool GProfiler::SaveToFile(const string& aPath)
{
    bool res = true;
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

Pind::Pind(const string& aDescription, int aBufLenLIm, const TEvents& aEvents):
    mBuf(nullptr), mDescription(aDescription), mBufLenLim(aBufLenLIm)
{
    for (auto ev : aEvents) {
	mEvents.insert(TEventsMapElem(ev.mId, &ev));
    }
}

const PEvent& Pind::getEvent(PEvent::TId aId) const
{
    __ASSERT(mEvents.count(aId));
    return *mEvents.at(aId);
}


PRec* PindClock::NewRec()
{
    PRecClock* res = nullptr;
    if (mPos < (mBufLenLim - 1)) {
	mPos++;
	PRecClock* res = static_cast<PRecClock*>(&mBuf[mPos]);
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

string PindClock::recToString(const PRec& aRec) const
{
    string res;
    res = to_string(aRec.mEventId) + PRec::KFieldSep;
    return res;
}
