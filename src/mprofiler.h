#ifndef __GRAYB_MPROFILER_H
#define __GRAYB_MPROFILER_H

#include <string>

class MElem;

using namespace std;

/** Profiler interface */
class MProfiler
{
    public:
	// Event identificator type
	using TEventId = int;
    public:
	/** Profilers event */
	class TEvent
	{
	    public:
		/** Undefined event id */
		static const TEventId KUndefEid = -1;
		TEvent(const string& aDescription): mDescription(aDescription) {}
		TEvent(const string& aDescription, TEventId aBase): mDescription(aDescription), mBaseId(aBase) {}
		/** Event description */
		string mDescription;
		/** Base event Id */
		TEventId mBaseId = KUndefEid;
	};

	/** Profilers record */
	class TRec
	{
	    public: 
		/** Length of NodeUri field, bytes */
		static const int KUriLen = 120;
		/** Length of Description field, bytes */
		static const int KDescrLen = 40;
		using TUri = char[KUriLen];
		using TNode = MElem*;
		using TClock = long int;
	    public:
		operator string() const;
		void setNode(MElem* aNode);
		/** Set Uri from string. For test purpose only */
		void setNodeUri(const string& aUri);
		void setClock(TClock aClock);
		void setEventId(TEventId aId);
	    public:
		char mNodeUri[KUriLen];
		/** Event Id */
		TEventId mEventId;
		MElem* mNode = nullptr;
		// Clock value, in ns
		TClock mClock = 0;
	    public:
		static const char KFieldSep = '\t';
	};
    public:
	virtual void Enable() = 0;
	/** Get current profilers record */
	virtual TRec& GetRec() = 0;
	virtual void Rec(TEventId aEventId, MElem* aNode) = 0;
	virtual void CommitEvent() = 0;
	virtual bool SaveToFile(const std::string& aPath) = 0;
	virtual TEventId RegisterEvent(const TEvent& aEvent) = 0;
	virtual const TEvent& GetEvent(TEventId aId) const = 0;
	virtual string ToString(const TRec& aRec) const = 0;
};

using TPEvent = MProfiler::TEvent;
using TPrec = MProfiler::TRec;

#endif 
