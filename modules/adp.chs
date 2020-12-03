AdpComps : Elem
{
    About = "Agents DES adaptor module";
    UnitAdp : Syst
    {
        About = "MUnit DES adaptor";
        AdpAgent : AMunitAdp;
        InpMagUri : CpStatecInp;
        CompsCount : CpStatecOutp;
        CompNames : CpStatecOutp;
    }
    MelemAdp : Syst
    {
        About = "MElem iface DES adaptor";
        AdpAgent : AMelemAdp;
        InpMut : CpStatecInp;
    }
}
