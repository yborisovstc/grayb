root : Elem
{
    $ # "Test of chrmoo2 transforming to tree-like";
    s1 : Syst;
    $ # "Just placeholder";
    ./s1 < {
        s1_u1 : Unit;
    }
    ./s1 < s1_u2 : Unit;
    ./s1 < {
        s1_1 : Syst
        {
            cp : Vert;
            cp2 : Vert;
        }
        s1_2 : Syst;
        s1_2 < {
            cp : Vert;
            cp2 : Vert;
            s1_2_u1 : Unit
            {
                About = "Just Unit";
                s1_2_u1_1 : Unit;
            }
        }
        s1_1/cp ~ s1_2/cp;
        s1_1 < s1_1_u1 : Unit;
        s1_1 < s1_1_u2 : Unit;
        $ # "Just placeholder";
        s1_1 < s1_1_u3 : Unit;
    }
}
