
#include "profiler_events.h"

using namespace PEvents;

/** Profile events register */
const PindClock::TEvents KPindClock_Events = {
    {Env_Start_Constr, "Env start constr"},
    {Env_Root_Created, "Env Root Created"},
    {Env_End_Constr, "Env End Constr"},
    {Elem_Constr_Start, "Elem constructor start"},
    {Elem_Constr_End, "Elem constructor end"},
};

/** Clock indicator data */
const PindClock::Idata KPindClockIdata = {
    "Clock indicator",
    1024,
    &KPindClock_Events
};


/** Init data for duration indicator */
const PindDur::Idata KPindDurIdata = {
    "Duration indicator",
    {
	{Elem_Constr_Dur, "Elem constructor duration"}
    }
};


/** Init data for profiler */
const GProfiler::TIdata KPInitData = {
    { EPiid_Clock, &KPindClockIdata },
    { EPiid_Dur, &KPindDurIdata }
};
