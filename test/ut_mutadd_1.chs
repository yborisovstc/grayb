test : Elem
{
  elem1 : Elem;
  elem1 < elem2 : Elem;
  ./elem1/elem2 < elem2_1 : Elem;
  elem3 : elem1;
  elem3 < elem4 : ./../elem1/elem2;
  ./elem3/elem4 < elem4_1 : Elem;
  elem5 : ./elem1/elem2;
  elem4_1i : ./elem3/elem4/elem4_1;
  _ # "Checking combined node";
  elem6 : Elem
  {
    elem6_1 : Elem;
    elem6_2 : elem3;
  }
}
