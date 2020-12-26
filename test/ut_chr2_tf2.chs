root : Elem
{
    $ # "Test of chrmoo2 transforming to tree-like: melding to upper targ";
    s1 : Syst;
    ./s1 < s1_1 : Syst;
    $ # "Placeholder 2";
    ./s1 < $ # "Hello";
    ./s1/s1_1 < s1_1_u1 : Unit;
}
