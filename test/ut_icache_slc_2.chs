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
            Pin1 : CpStatecInp;
            Pin1Back : CpStatecOutp;
        }
        Sock1p : ASocketMcm
        {
            Pin1 : CpStatecOutp;
            Pin1Back : CpStatecInp;
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
            ./State1/Inp ~ ./S1Cp/Pin1Back;
        }
        Syst2 : Syst
        {
            State2 : AStatec;
            ./State2 - Value = "I 0";
            ./State2 - Debug.Update = y;
            S2Cp : Sock2;
            ./State2/Inp ~ ./S2Cp/Block/Pin1;
        }
        S1CpP : Sock1p;
        ./Syst1/S1Cp ~ S1CpP;
        S1CpP/Pin1 ~ S1CpP/Pin1Back;
    }
}
