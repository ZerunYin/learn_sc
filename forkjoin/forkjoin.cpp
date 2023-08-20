#define SC_INCLUDE_DYNAMIC_PROCESSES
#include <iostream>
#include <systemc>

using namespace sc_core;
using std::cout;

// spawn function/method

void void_func(double d) {
  cout << "void_func called\n";
}

int copy_func(double d) {
  cout << "copy_func called: " << int(d) << "\n";
  return int(d);
}

int ref_func(const double& d) {
  cout << "ref_func called: " << int(d) << "\n";
  return int(d);
}

SC_MODULE(Top) {
  SC_CTOR(Top) {
    SC_THREAD(main);
  }

  void main() {
    int result;
    sc_event e0, e1, e2, e3;
    
    e0.notify(120, SC_NS);
    // ch 5.5, 5.6 of standard
    SC_FORK
      sc_spawn(&result,
        sc_bind(&Top::round_robin, this, "0", sc_ref(e0), sc_ref(e1), 3), "0"), // defined in sc_spawn.h
      sc_spawn(&result,
        sc_bind(&Top::round_robin, this, "1", sc_ref(e1), sc_ref(e2), 3), "1"),
      sc_spawn(&result,
        sc_bind(&Top::round_robin, this, "2", sc_ref(e2), sc_ref(e3), 3), "2"),
      sc_spawn(&result,
        sc_bind(&Top::round_robin, this, "3", sc_ref(e3), sc_ref(e0), 3), "3"),
    SC_JOIN // sc_core::sc_join join; join.add_process(i); join.wait();

    // thread pool?
    for(int i = 0; i < 10; i++) {
      sc_spawn(&result, sc_bind(&Top::wait_and_end, this, i));
    }

    // sc_spawn_options opts;
    // opts.set_xxx(...);

    sc_process_handle handle = sc_spawn(sc_bind(&void_func, 3.14));
    wait(handle.terminated_event());
    
    double d0 = 3.14;
    wait(sc_spawn(&result, sc_bind(&ref_func, sc_cref(d0))).terminated_event());

    double d1 = 6.28;
    wait(sc_spawn(&result, sc_bind(&copy_func, d1)).terminated_event());

  }

  int round_robin(const char* str, sc_event& receive, sc_event& send, int cnt) {
    while(cnt-- > 0) {
      wait(receive);
      cout << sc_time_stamp() << ": " << str << "\n";
      wait(10, SC_NS);
      send.notify();
    }
    return 0;
  }

  int wait_and_end(int i) {
    wait(10 - i + 1, SC_NS);
    cout << sc_time_stamp() << ": Thread " << i << " exit\n";
    return 0;
  }
};



int sc_main(int argc, char* argv[]) {
  Top top("top");

  sc_start(500, SC_NS);

  return 0;

}
