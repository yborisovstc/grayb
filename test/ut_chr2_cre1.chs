root : Elem
{
    $ # "Test of commentary mut";
    e1 : Elem;
    ./e1 < {
        e1_1 : Elem;
        e1_2 : Elem;
        e1_2 < {
            e1_2_1 : Elem;
        }
    }
    e1 < ee1_1 : e1_1;
    $ # "Test of Remove mut";
    ./e1/ee1_1 ! $;
}
