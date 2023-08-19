#include <systemc>
#include <iostream>

using namespace sc_core;
using std::cout;

struct C_if : virtual public sc_interface {
  virtual void run() = 0;
};

// typedef sc_module sc_channel 
// typedef sc_module sc_behavior

struct C : public sc_channel, public C_if {
  SC_CTOR(C) {}
  void run() override {
    cout << sc_time_stamp() << ": " << name() << " run called\n";
  }
};

SC_MODULE(D) {
  C m_c;
  sc_export<C_if> IFP;

  SC_CTOR(D): m_c("m_c"), IFP("IFP") {
    IFP(m_c);
  }
};

SC_MODULE(E) {
  C m_c;
  D m_d;
  sc_export<C_if> IFP0; // bind m_c
  sc_export<C_if> IFP1; // bind m_d.m_c

  SC_CTOR(E): m_c("m_c"), m_d("m_d"), IFP0("IFP0"), IFP1("IFP1") {
    IFP0(m_c);
    IFP1(m_d.IFP);
  }
};


SC_MODULE(X) {
  sc_port<C_if> P0;
  sc_port<C_if> P1;

  SC_CTOR(X) {
    SC_THREAD(run);
  }
  void run() {
    for(int i = 0; i < 4; i++) {
      wait(10, SC_NS);
      P0->run();
      wait(10, SC_NS);
      P1->run();
    }
  }
};

int sc_main(int argc, char* argv[]) {
  X x("x");
  E e("e");
  x.P0(e.IFP0);
  x.P1(e.IFP1);
  sc_start();
  return 0;
}
