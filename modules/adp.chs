AdpComps : Elem
{
    About = "Agents DES adaptor module";
    UnitAdp : Syst
    {
        About = "MUnit DES adaptor";
        AdpAgent : AMunitAdp;
        AdpCp : CpStatecOutp;
        CompsCount : CpStatecOutp;
        CompNames : CpStatecOutp;
    }
    MelemAdp : /*/Modules/DesComps/Des
    {
        About = "MElem iface DES adaptor";
        AdpAgent : AMelemAdp;
        AdpCp : CpStatecOutp;
        MutationApplied : AStatec;
        MutationApplied < Debug.Update = y;
        MutationApplied < Value = "MUT none";
        MutationApplied/Inp ~ AdpCp;
        $ # "MutationApplied ~ InpMut via internal binding";
        InpMut : CpStatecInp;
    }
}
