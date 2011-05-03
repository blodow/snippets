#include <vector>
#include <iostream>
#include <boost/shared_ptr.hpp>

template <typename T1>
struct Device
{
  typedef std::vector<T1> type;
};

template <typename T1>
struct Host
{
  typedef std::vector<T1> type;
};

template <typename T1> 
struct t1
{
  template <typename T2>
  inline T2 foo (int a)
  {
    std::cerr << __PRETTY_FUNCTION__ << std::endl;
    return T2();
  }
};

template <template <typename> class Storage>
struct PointCloud
{
  typename Storage<float>::type points;
};

template <template <typename> class Storage>
void bar ()
{
  PointCloud<Storage> pc;
  boost::shared_ptr<const PointCloud<Storage> > ptr (&pc); 
  t1<boost::shared_ptr<PointCloud<Storage> > > bla;
  bla.template foo<typename Storage<double>::type > (2);
}

int main ()
{
  bar <Device> ();
  
}
