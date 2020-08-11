AdpComps : Elem
{
    About = "Agents DES adaptor module";
    UnitAdp : /*/Modules/DesComps/Des
    {
        About = "MUnit DES adaptor";
        AdpAgent : AMunitAdp;
        CompsCount : AStatec;
        CompsCount < Value = "SI 0";
        AdpCp : CpStatecOutp;
        CompsCount/Inp ~ AdpCp;
        CompUid : AStatec;
        CompUid < Value = "SS _";
        CompUid/Inp ~ AdpCp;
        InpCompIdx : CpStatecInp;
    }
    MelemAdp : /*/Modules/DesComps/Des
    {
        About = "MElem iface DES adaptor";
        AdpAgent : AMelemAdp;
        AdpCp : CpStatecOutp;
        MutationApplied : AStatec;
	MutationApplied < Debug.Update = y;
        MutationApplied < Value = "SS _";
        MutationApplied/Inp ~ AdpCp;
        InpMut : CpStatecInp;
    }
}
