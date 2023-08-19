#include <iostream>
#include <systemc>

using namespace sc_core;
using std::cout;
using std::endl;

SC_MODULE(Producer) {
  SC_CTOR(Producer) {
    SC_CTHREAD(run, m_clk.pos());
    reset_signal_is(m_reset, false);
  }

  void run() {
    cout << sc_time_stamp() << ": producer reset\n";
    m_value = 0;
    while(true) {
      do {
        m_valid = true; // handshake every cycle after reset
        wait();
      } while(!m_ready);
      m_value = m_value + 1;
    }
  }

  sc_in<bool> m_clk; // sc_in_clk m_clk;
  sc_in<bool> m_reset;

  sc_inout<bool> m_valid;
  sc_inout<int>  m_value;
  sc_in<bool>    m_ready;
};

SC_MODULE(Consumer) {
  SC_CTOR(Consumer) {
    SC_CTHREAD(run, m_clk.pos());
    reset_signal_is(m_reset, false);
  }

  void run() {
    cout << sc_time_stamp() << ": consumer reset\n";
    while(true) {
      do {
        m_ready = true;
        wait();
      } while(!m_valid);
      cout << sc_time_stamp() << ": " << m_value << "\n";
    }
  }

  sc_in<bool> m_clk;
  sc_in<bool> m_reset;

  sc_in<bool>    m_valid;
  sc_in<int>     m_value;
  sc_inout<bool> m_ready;
};

SC_MODULE(Tb) {
  SC_CTOR(Tb): c("c"), p("p") {
    SC_CTHREAD(run, m_clk.pos());
    c.m_clk(m_clk);
    c.m_reset(m_reset);
    c.m_valid(m_valid);
    c.m_value(m_value);
    c.m_ready(m_ready);

    p.m_clk(m_clk);
    p.m_reset(m_reset);
    p.m_valid(m_valid);
    p.m_value(m_value);
    p.m_ready(m_ready);
  }

  void run() {
    for(int state = 0; state < 100; state++) {
      m_reset = state % 20 == 12 ? false : true;
      if(!m_reset)
        cout << "tb reset\n";
      wait();
    }
    sc_stop();
  }

  Consumer c;
  Producer p;

  sc_in_clk m_clk;
  sc_signal<bool> m_reset;
  sc_signal<bool> m_valid;
  sc_signal<bool> m_ready;
  sc_signal<int>  m_value;
};


int sc_main(int argc, char* argv[]) {
  sc_clock clk("clk", 1, SC_NS);
  Tb tb("tb");
  tb.m_clk(clk);

  sc_start(120, SC_NS);

  return 0;

}
