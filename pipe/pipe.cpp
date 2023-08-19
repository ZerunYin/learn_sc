#include <iostream>
#include <systemc>

using namespace sc_core;
using std::cout;

template<typename T, int N>
SC_MODULE(Pipe) {
  SC_CTOR(Pipe) {
    m_in(m_pipe[0]);
    m_out(m_pipe[N-1]);
    SC_METHOD(rachet);
    sensitive << m_clk.pos();
  }

  void rachet() {
    for(int i = N - 1; i > 0; i--) {
      m_pipe[i].write(m_pipe[i-1].read());
    }
  }

  sc_in_clk m_clk;
  sc_signal<T> m_pipe[N];
  sc_export<sc_signal_inout_if<T>> m_in;
  sc_export<sc_signal_in_if<T>> m_out;
};

template<typename T>
SC_MODULE(Reader) {
  SC_CTOR(Reader) {
    SC_METHOD(extract);
    sensitive << m_clk.pos();
    dont_initialize();
  }

  void extract() {
    cout << sc_time_stamp().to_double() << ": " << m_from_pipe.read() << "\n";
  }
  sc_in_clk m_clk;
  sc_in<T> m_from_pipe; // in
};

template<typename T>
SC_MODULE(Writer) {
  SC_CTOR(Writer) {
    SC_METHOD(insert);
    sensitive << m_clk.pos();
    dont_initialize();
  }

  void insert() {
    m_to_pipe.write(entry);
    entry++;
  }

  sc_in_clk m_clk;
  sc_inout<T> m_to_pipe; // out
  T entry{};
};


int sc_main(int argc, char* argv[]) {
  sc_clock clk("clk", 0.5, SC_NS);
  Pipe<int, 4> pipe("pipe");
  Reader<int> r("r");
  Writer<int> w("w");

  pipe.m_clk(clk);
  w.m_clk(clk);
  r.m_clk(clk);

  w.m_to_pipe(pipe.m_in);
  r.m_from_pipe(pipe.m_out);

  sc_start(20, SC_NS);

  return 0;
}
