#include <iostream>

template <bool has_color_val, bool has_normal_val>
struct point_traits
{
   typedef point_traits <has_color_val, has_normal_val>  type;
   static const bool has_color = has_color_val;
   static const bool has_normal = has_normal_val;
};

struct P1 
{
  typedef point_traits <true, true> point_traits;
};
struct P2
{
  typedef point_traits <false, true> point_traits;
};
struct P3
{
  typedef point_traits <false, false> point_traits;
};

template <typename PT, bool has_color = PT::point_traits::has_color >
struct algo
{ // this will never get callled. or compiled :)
  void foo () { std::cerr << "nothing" << std::endl; }
};

template <typename PT>
struct algo <PT,false>
{
  void foo ()
  {
    std::cerr << "false" << std::endl;
    if (PT::point_traits::has_normal)
      std::cerr << "has normal" << std::endl;
    else
      std::cerr << "has no normal" << std::endl;
  }
};

template <typename PT>
struct algo <PT,true>
{ 
  void foo () { std::cerr << "true" << std::endl;
  if (PT::point_traits::has_normal)
    std::cerr << "has normal" << std::endl;
  else
    std::cerr << "has no normal" << std::endl;
  }
};

int main ()
{
  P1 p1;
  P2 p2;
  P3 p3;
  algo<P1> a1; a1.foo ();
  algo<P2> a2; a2.foo ();
  algo<P3> a3; a3.foo ();
}
