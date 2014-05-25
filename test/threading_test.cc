#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE threading_test
#include <cmath>
#include <boost/test/unit_test.hpp>

#include "mtl/threading/threadpool.h"

using namespace std;
using namespace mtl::threading;

BOOST_AUTO_TEST_CASE(FutureTest)
{
  const int kR1 = 100;
  const int kR2 = 105;
  ThreadPool<int> pool;
  pool.start(3);

  auto f1 = pool.runTask([]() { return kR1; });
  auto f2 = pool.runTask([]() { return kR2; });
  f1.wait();
  f2.wait();

  BOOST_CHECK_EQUAL(f1.get(), kR1);
  BOOST_CHECK_EQUAL(f2.get(), kR2);
  pool.stop();
}

BOOST_AUTO_TEST_CASE(WaitTest)
{
  const int kR1 = 100;
  const int kR2 = 105;
  int r1 = 0, r2 = 0;
  ThreadPool<void> pool;
  pool.start(8);

  pool.addTask([&r1]() { r1 = kR1; });
  pool.addTask([&r2]() { r2 = kR2; });
  pool.wait();
  BOOST_CHECK_EQUAL(r1, kR1);
  BOOST_CHECK_EQUAL(r2, kR2);
}

