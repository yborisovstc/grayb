testroot : Elem
{
    test : Vert
    {
        CpA : ConnPointMc
        {
            Provided = MTestIface1;
            Required = MTestIface2;
        }
        CpAp : ConnPointMc
        {
            Provided = MTestIface2;
            Required = MTestIface1;
        }
        CpAE : AExtd
        {
            Int : CpA;
        }
        CpAE2 : AExtd
        {
            Int : CpAp;
        }
        V1 : Vert
        {
            Agt : TstAgt;
            Agt < Cpi1 : CpAp;
        }
        V1/Agt/Cpi1 ~ CpAE/Int;
        V1/Agt/Cpi1 ~ CpAE2;
    }
}
