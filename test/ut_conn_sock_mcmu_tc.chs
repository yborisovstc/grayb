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
        CpC : ConnPointMc
        {
            Provided = If_C;
            Required = If_D;
        }
        CpCp : ConnPointMc
        {
            Provided = If_D;
            Required = If_C;
        }
        Sock1 : ASocketMcm
        {
            Pin1 : ./../CpA;
        }
        Sock1p : ASocketMcm
        {
            Pin1 : ./../CpAp;
        }
        Sock2 : ASocketMcm
        {
            Pin1 : ./../CpA;
            Pin2 : ./../Sock1;
        }
        Sock2p : ASocketMcm
        {
            Pin1 : ./../CpAp;
            Pin2 : ./../Sock1p;
        }
        Sock3 : ASocketMcm
        {
            Pin1 : ./../CpA;
            Pin2 : ./../Sock2;
        }
        Sock3p : ASocketMcm
        {
            Pin1 : ./../CpAp;
            Pin2 : ./../Sock2p;
        }
        Sock3 ~ Sock3p;
        Cps1 : AExtd
        {
            Int : Sock1;
        }
        Cps1 ~ Sock3/Pin2/Pin2;
        V1 : Vert
        {
            Agt : TstAgt;
            Agt < Cpi1 : CpAp;
        }
        V1/Agt/Cpi1 ~ Cps1/Int/Pin1;
        Cpi2 : CpA;
        Cpi2 ~ Sock3p/Pin2/Pin2/Pin1;
    }
}
