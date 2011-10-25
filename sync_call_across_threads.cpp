#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <pthread.h>
#include <iostream>

boost::mutex print_mtx;
boost::mutex token_mutex;
boost::mutex callback_mutex;

bool go_on;
int g_token;

boost::condition_variable token_needed;
boost::condition_variable token_available;

boost::mutex data_ready_mutex;
bool data_ready;

boost::mutex data_needed_mutex;
bool data_needed;

// this function must produce one item per "callback" call
void produce ()
{
  { // lock token_mutex and "produce" one token
    boost::mutex::scoped_lock token_lock(token_mutex);
    g_token++;
    boost::mutex::scoped_lock print_lock (print_mtx);
    std::cerr << pthread_self() << ": produced " << g_token << std::endl;
  }
  
  {
    boost::lock_guard<boost::mutex> lock(data_ready_mutex);
    data_ready=true;
  }

  token_available.notify_all ();
}

// this function must synchronously call produce across thread boundaries
void callback ()
{
  boost::mutex::scoped_lock callback_lock (callback_mutex);

  {
    boost::lock_guard<boost::mutex> lock(data_needed_mutex);
    data_needed=true;
  }
  token_needed.notify_all ();

  boost::unique_lock<boost::mutex> lock(data_ready_mutex);
  while (!data_ready)
    token_available.wait(lock);
  data_ready = false;

  boost::mutex::scoped_lock print_lock (print_mtx);
  boost::mutex::scoped_lock token_lock (token_mutex);
  std::cerr << pthread_self() << ": consumed " << g_token << std::endl;
}

// multiple consumer threads just call callback (which is itself mutually exclusive)
void *consumer (void*)
{
  for (int i = 0; i < 50; ++i)
  {
    callback ();
    sleep (0.5);
  }

  return NULL;
}

void *worker (void*)
{
  {
    boost::mutex::scoped_lock print_lock (print_mtx);
    std::cerr << "worker " << pthread_self () << std::endl;
  }

  // init worker / producer stuff
  g_token = 77;

  // wait for actual request to process one token
  while (go_on)
  {
    // wait for "data needed!"
    boost::unique_lock<boost::mutex> lock (data_needed_mutex);
    while (!data_needed && go_on)
      token_needed.wait(lock);
    data_needed = false;
    produce ();
  }
}

int main (int argc, char** argv)
{
  go_on = true;
  pthread_t ptid1, ptid2, wptid;

  data_needed = false;
  data_ready = false;

  pthread_create (&ptid1, NULL, consumer, NULL);
  {
    boost::mutex::scoped_lock print_lock (print_mtx);
    std::cerr << "created consumer 1 with ID " << ptid1 << std::endl;
  }
  pthread_create (&ptid2, NULL, consumer, NULL);
  {
    boost::mutex::scoped_lock print_lock (print_mtx);
    std::cerr << "created consumer 2 with ID " << ptid2 << std::endl;
  }
  pthread_create (&wptid, NULL, worker, NULL);
  {
    boost::mutex::scoped_lock print_lock (print_mtx);
    std::cerr << "created worker with ID " << wptid << std::endl;
  }

  pthread_join (ptid1, NULL);
  pthread_join (ptid2, NULL);
  go_on = false;
  token_needed.notify_all ();
  pthread_join (wptid, NULL);

  return EXIT_SUCCESS; // tremendous success!
}
