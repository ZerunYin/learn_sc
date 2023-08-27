#include <iostream>

#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

using std::cout;

using namespace sc_core;
using namespace tlm;
using namespace tlm_utils;

struct Interconnect : public sc_module {
  SC_HAS_PROCESS(Interconnect);

  simple_initiator_socket<Interconnect> simple_ini;
  simple_target_socket<Interconnect> simple_tgt;

  Interconnect(sc_module_name name)
    : sc_module(name),
      simple_ini("simple_ini"),
      simple_tgt("simple_tgt")
  {
    simple_ini->register_nb_transport_bw(this, &Interconnect::nb_transport_bw); // backward path
    simple_ini->register_invalidate_get_direct_mem_ptr(this, &Interconnect::invalidate_direct_mem_ptr);

    simple_tgt->register_b_transport(this, &Interconnect::b_transport);
    simple_tgt->register_nb_transport_fw(this, &Interconnect::nb_transport_fw);
    simple_tgt->register_get_direct_mem_ptr(this, &Interconnect::get_direct_mem_ptr);
    simple_tgt->register_transport_dbg(this, &Interconnect::transport_dbg);
  }

  // backward path
  tlm_sync_enum nb_transport_bw(tlm_generic_payload &gp, tlm_phase &phase, sc_time &time) {
    cout << "nb_transport_bw\n";
    return TLM_ACCEPTED;
  }

  void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) {
    cout << "invalidate_direct_mem_ptr\n";
  }

  // forward path
  void b_transport(tlm_generic_payload &gp, sc_time &time) {
    cout << "b_transport\n";
  }

  tlm_sync_enum nb_transport_fw(tlm_generic_payload &gp, tlm_phase &phase, sc_time &time) {
    cout << "nb_transport_fw\n";
    return TLM_COMPLETED;
  }

  bool get_direct_mem_ptr(tlm_generic_payload &gp, tlm_dmi &dmi) {
    cout << "get_direct_mem_ptr\n";
    return true;
  }

  int unsigned transport_dbg(tlm_generic_payload &gp) {
    cout << "transport_dbg\n";
    return 0;
  }

};


int sc_main(int argc, char* argv[]) {
  Interconnect interconnect("interconnect");

  sc_start(2, SC_NS);

  return 0;
}
