testroot : Elem
{
    Modules : AImports
    {
        $ + /DesComps/Des;
    }
    test : ./Modules/DesComps/Des
    {
        About = "Simple test verifies IRM in case of socket loopback connection (SLC)";
        Sock1 : ASocketMcm
        {
            Block : ASocketMcm
            {
                Pin1 : CpStatecInp;
            }
        }
        Sock2 : ASocketMcm
        {
            Block : ASocketMcm
            {
                Pin1 : CpStatecOutp;
            }
        }
        Syst1 : Syst
        {
            State1 : AStatec;
            ./State1 - Value = "I 0";
            ./State1 - Debug.Update = y;
            S1Cp : Sock1;
            ./S1Cp/Block/Pin1 ~ State1;
        }
        Syst2 : Syst
        {
            State2 : AStatec;
            ./State2 - Value = "I 0";
            ./State2 - Debug.Update = y;
            S2Cp : Sock2;
            ./State2/Inp ~ ./S2Cp/Block/Pin1;
        }
        Syst1/S1Cp ~ Syst2/S2Cp;
    }
}
