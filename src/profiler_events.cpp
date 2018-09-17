
#include "profiler_events.h"

using namespace PEvents;

/** Profile events register */
const Pind::TEvents KPindClock_Events = {
    {Env_Start_Constr, "Env start constr"},
    {Env_Root_Created, "Env Root Created"},
    {Env_End_Constr, "Env End Constr"},
    {Elem_Constr_Start, "Elem constructor start"},
    {Elem_Constr_End, "Elem constructor end"},
    {Elem_Constr_Dur, "Elem constructor duration"}
};

/** Clock indicator data */
const PindClock::Idata KPindClockIdata = {
    "Clock indicator",
    1024,
    &KPindClock_Events
};

const GProfiler::TIdata KPInitData = {
    { EPiid_Clock, &KPindClockIdata }
};

;
