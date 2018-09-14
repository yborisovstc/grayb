#ifndef __GRAYB_PROFILER_H
#define __GRAYB_PROFILER_H

#include <vector>

#include "mprofiler.h"

class MEnv;

/** Default profiler */
class GProfiler: public MProfiler
{
    public:
	GProfiler(MEnv* aEnv);
	virtual ~GProfiler();
	// From MProfiler
	virtual void Enable() override;
	virtual TPrec& GetRec() override;
	virtual void Rec(TEventId aEventId, MElem* aNode) override;
	virtual void CommitEvent() override;
	virtual bool SaveToFile(const std::string& aPath) override;
	virtual TEventId RegisterEvent(const TPEvent& aEvent) override;
	virtual const TEvent& GetEvent(TEventId aId) const override;
	virtual string ToString(const TRec& aRec) const override;
    protected:
	TPrec::TClock GetClock() const;
	/** Finds nearest lower record for base event */
	TRec* FindBaseRec(int aPos) const;
    protected:
	MEnv* mEnv;
	static const int KBufLen;
	/** Events buffer */
	TPrec* mBuf;
	/** Insertion position in events buffer */
	int mPos = -1;
	TPrec mCache;
	/** Events registered */
	std::vector<TPEvent> mEvents;
};


#endif
