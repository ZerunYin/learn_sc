#include <iostream>
#include <systemc>
#include <list>

using namespace sc_core;
using std::cout;

struct Mutex : public sc_mutex {
  enum Policy {FIFO, RANDOM};

  explicit Mutex(Policy p) : policy_(p) {}

  int lock() override {
    if(in_use()) {
      event_list_.emplace_back();
      sc_core::wait(event_list_.back());
    }
    m_owner = sc_get_current_process_b();
    return 0;
  }
  int unlock() override {
    if(m_owner != sc_get_current_process_b())
      return -1;

    auto iter = next_event();
    if(iter != event_list_.end()) {
      iter->notify();
      event_list_.erase(iter);
    }
    return 0;
  }

  std::list<sc_event>::iterator next_event() {
    return event_list_.begin();
  }

  Policy policy_;
  std::list<sc_event> event_list_;
};


struct Top: public sc_module {
  SC_HAS_PROCESS(Top);

  Top(const sc_module_name& name): sc_module(name), mutex_(Mutex::Policy::FIFO) {
    SC_THREAD(t0);
    SC_THREAD(t1);
    SC_THREAD(t2);
  }
  void t0() {
    wait(1, SC_NS);
    mutex_.lock();
    cout << sc_time_stamp() << ": t0 gets mutex\n";
    wait(10, SC_NS);
    mutex_.unlock();
  }
  void t1() {
    wait(1, SC_NS);
    mutex_.lock();
    cout << sc_time_stamp() << ": t1 gets mutex\n";
    wait(10, SC_NS);
    mutex_.unlock();
  }
  void t2() {
    wait(1, SC_NS);
    mutex_.lock();
    cout << sc_time_stamp() << ": t2 gets mutex\n";
    wait(10, SC_NS);
    mutex_.unlock();
  }
  Mutex mutex_;
};



int sc_main(int argc, char* argv[]) {
  Top top("top");

  sc_start(120, SC_NS);

  return 0;

}
