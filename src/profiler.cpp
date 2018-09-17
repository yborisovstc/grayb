#include <cstdio>
#include <ctime>
#include <time.h>

#include "profiler.h"
#include "guri.h"
#include "melem.h"

using namespace std;

void TPrec::setNodeUri(const string& aUri)
{
}

void TPrec::setNode(MElem* aNode)
{
    mNode = aNode;
}

TPrec::operator string() const
{
    string res;
    // Uri
    string suri;
    if (mNode) {
	GUri uri;
	mNode->GetUri(uri);
	suri = uri.toString(ETrue);
    }
    res = suri;
    res += KFieldSep;
    // Events id
    string eid = to_string(mEventId);
    res += eid;
    res += KFieldSep;
    // Clock
    res += to_string(mClock);
    res += KFieldSep;
    return res;
}

void TPrec::setClock(TClock aClock)
{
    mClock = aClock;
}

void TPrec::setEventId(TEventId aId)
{
    mEventId = aId;
}

const int GProfiler::KBufLen = 1024;

GProfiler::GProfiler(MEnv* aEnv, const TProfilerEvents& aEvents): mEnv(aEnv), mEvents(aEvents),  mPos(-1)
{
    clockid_t cid;
    struct timespec ts;
    int s = pthread_getcpuclockid(pthread_self(), &cid);
    if (s == 0) {
	s = clock_getres(cid, &ts);
    }
    int ticks = CLOCKS_PER_SEC;
    mBuf = new TPrec[KBufLen];
}

GProfiler::~GProfiler()
{
    delete mBuf;
}

void GProfiler::Enable()
{
}

bool GProfiler::SaveToFile(const string& aPath)
{
    bool res = true;
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
    return res;
}


TPrec& GProfiler::GetRec()
{
    if (mPos < (KBufLen - 1)) {
	mPos++;
	TPrec& res = mBuf[mPos];
	res.setClock(GetClock());
    } else {
	mCache.setClock(GetClock());
	return mCache;
    }
}

void GProfiler::CommitEvent()
{
}

TPrec::TClock GProfiler::GetClock() const
{
    TPrec::TClock res = -1;
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

const TPEvent& GProfiler::GetEvent(TEventId aId) const
{
//    return mEvents.at(aId);
    __ASSERT(mEvents.count(aId) != 0);
    return mEvents.at(aId);
}

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
