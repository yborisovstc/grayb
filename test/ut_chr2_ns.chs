root : Elem
{
    $ # "Test of chrmoo2 namespaces";
    s1 : Syst;
    ./s1 < {
        s1_1 : Syst {
           cp : Vert;
           cp2 : Vert;
        }
        s1_2 : Syst;
        s1_2 < {
            cp : Vert;
            cp2 : Vert;
        }
        s1_1/cp ~ s1_2/cp;
        ./s1_1 @ {
           cp ~ s1_2/cp;
        }
    }
}
