#include <iostream>
#include <systemc>

using namespace sc_core;
using std::cout;

class Barrier {
public:
  void initialize(int n) {
    m_num_threads = n;
    cout << "NUM_THREADS: " << m_num_threads << "\n"
         << "EVENT: " << barrier_event.name() << "\n";
  }

  void wait() {
    m_num_threads--;
    if(m_num_threads) {
      sc_core::wait(barrier_event);
    } else {
      barrier_event.notify(SC_ZERO_TIME); // create delta notification
      sc_core::wait(barrier_event);
    }
  }

private:
  int m_num_threads;
  sc_event barrier_event {"barrier_event"};
};

SC_MODULE(X) {
  SC_CTOR(X) {
    barrier.initialize(2);
    SC_THREAD(a);
    SC_THREAD(b);
  }

  void a() {
    wait(10, SC_NS);
    barrier.wait();
    cout << sc_time_stamp() << ": a barrier\n";
  }

  void b() {
    wait(15, SC_NS);
    barrier.wait();
    cout << sc_time_stamp() << ": b barrier\n";
  }

  Barrier barrier;
};

int sc_main(int argc, char* argv[]) {
  X x("x");

  sc_start(100, SC_NS);
  cout << "End\n";

  return 0;

}
