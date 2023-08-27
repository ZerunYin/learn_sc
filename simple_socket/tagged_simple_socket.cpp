#include <iostream>

#include <tlm>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

using std::cout;

using namespace sc_core;
using namespace tlm;
using namespace tlm_utils;

template <int NUM_INITIATOR = 2, int NUM_TARGET = 2>
struct Bus : public sc_module {
  SC_HAS_PROCESS(Bus);

  simple_initiator_socket_tagged<Bus> *simple_ini[NUM_INITIATOR];
  simple_target_socket_tagged<Bus> *simple_tgt[NUM_TARGET];

  tlm_generic_payload gp;
  tlm_phase phase;
  sc_time time;
  tlm_dmi dmi;

  Bus(sc_module_name name)
    : sc_module(name)
  {
    char socket_name[256];
    for(int i = 0; i < NUM_INITIATOR; i++) {
      sprintf(socket_name, "simple_ini_%0d", i);
      simple_ini[i] = new simple_initiator_socket_tagged<Bus>(socket_name);

      simple_ini[i]->register_nb_transport_bw(this, &Bus::nb_transport_bw, i);
      simple_ini[i]->register_invalidate_direct_mem_ptr(this, &Bus::invalidate_direct_mem_ptr, i);
    }

    for(int i = 0; i < NUM_TARGET; i++) {
      sprintf(socket_name, "simple_tgt_%0d", i);
      simple_tgt[i] = new simple_target_socket_tagged<Bus>(socket_name);

      simple_tgt[i]->register_b_transport(this, &Bus::b_transport, i);
      simple_tgt[i]->register_nb_transport_fw(this, &Bus::nb_transport_fw, i);
      simple_tgt[i]->register_get_direct_mem_ptr(this, &Bus::get_direct_mem_ptr, i);
      simple_tgt[i]->register_transport_dbg(this, &Bus::transport_dbg, i);

      simple_ini[NUM_TARGET - 1 - i]->bind(*simple_tgt[i]);
    }

    SC_THREAD(run);
  }

  ~Bus() {
    for(int i = 0; i < NUM_INITIATOR; i++) {
      delete simple_ini[i];
    }
    for(int i = 0; i < NUM_INITIATOR; i++) {
      delete simple_tgt[i];
    }
  }

  void run() {
    cout << "Run\n";

    (*simple_ini[0])->b_transport(gp, time);
    (*simple_ini[1])->b_transport(gp, time);
    // (*simple_ini[0])->b_transport(gp, time);
    // (*simple_ini[0])->b_transport(gp, time);
  }

  // backward path
  tlm_sync_enum nb_transport_bw(int id, tlm_generic_payload &gp, tlm_phase &phase, sc_time &time) {
    cout << "nb_transport_bw\n";
    return TLM_ACCEPTED;
  }

  void invalidate_direct_mem_ptr(int id, sc_dt::uint64 start_range, sc_dt::uint64 end_range) {
    cout << "invalidate_direct_mem_ptr\n";
  }

  // forward path
  void b_transport(int id, tlm_generic_payload &gp, sc_time &time) {
    cout << id << ": b_transport\n";
  }

  tlm_sync_enum nb_transport_fw(int id, tlm_generic_payload &gp, tlm_phase &phase, sc_time &time) {
    cout << "nb_transport_fw\n";
    return TLM_COMPLETED;
  }

  bool get_direct_mem_ptr(int id, tlm_generic_payload &gp, tlm_dmi &dmi) {
    cout << "get_direct_mem_ptr\n";
    return true;
  }

  int unsigned transport_dbg(int id, tlm_generic_payload &gp) {
    cout << "transport_dbg\n";
    return 0;
  }

};


int sc_main(int argc, char* argv[]) {
  Bus<> bus("bus");
  sc_start(2, SC_NS);
  return 0;
}
