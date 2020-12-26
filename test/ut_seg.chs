test : Elem
{
  Logspec : Elem
  {
    Creation : Elem;
  }
  elem1 : Elem
  {
    elem2 : Elem;
  }
  elem3 : ./elem1;
  ./elem3 < {
    Logspec : Elem
    {
      Creation : Elem;
    }
    elem4 : ./../elem1/elem2;
    elem4_2 : ./../elem1;
  }
  ./elem1 < {
    unit1_1 : Unit;
    ./unit1_1 - {
      unit1_1_1 : Unit;
      ./unit1_1_1 - {
        unit1_1_1_1 : Unit;
        unit1_1_1_2 : Unit;
      }
    }
  }
  ./elem1 - unit1_2 : Unit;
  ./elem1/unit1_1 - unit1_1_2 : Unit;
  elem5 : ./elem1/elem2;
}
