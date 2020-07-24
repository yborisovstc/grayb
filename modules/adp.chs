AdpComps : Elem
{
    About = "Agents DES adaptor module";
    UnitAdp : /*/Modules/DesComps/Des
    {
        AdpAgent : AMunitAdp;
        About = "MUnit DES adaptor";
        CompsCount : AStatec;
        CompsCount < Value = "SI 0";
        AdpCp : CpStatecOutp;
        CompsCount/Inp ~ AdpCp;
    }
}
