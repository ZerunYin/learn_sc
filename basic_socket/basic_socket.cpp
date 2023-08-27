#include <iostream>

#include <tlm>

using std::cout;

using namespace sc_core;
using namespace tlm;

// core interfaces:
// tlm_blocking_transport_if
//    void b_transport(T &trans, sc_time &time);

// tlm_fw_nonblocking_transport_if
//    tlm_sync_enum nb_transport_fw(T &trans, P &phase, sc_time &time);
// tlm_bw_nonblocking_transport_if
//    tlm_sync_enum nb_transport_bw(T &trans, P &phase, sc_time &time);
// TLM_ACCEPTED, TLM_UPDATED, TLM_COMPLETED

// tlm_fw_direct_mem_if
//    bool get_direct_mem_ptr(T &trans, tlm_dmi &dmi);
// tlm_bw_direct_mem_if
//    void invalidate_direct_mem_ptr(uint64 start_range, uint64 end_range);

// tlm_transport_debug_if
//     unsigned int transport_dbg(T &trans);

// Combined interface:
// tlm_fw_transport_if
// tlm_bw_transport_if

struct Initiator : public sc_module, tlm_bw_transport_if<> {
  SC_HAS_PROCESS(Initiator);

  tlm_initiator_socket<> ini_socket;

  tlm_generic_payload gp;
  tlm_phase phase;
  sc_time time;
  tlm_dmi dmi;

  Initiator(sc_module_name name): sc_module(name), ini_socket("ini_socket") {
    SC_THREAD(run);
    ini_socket.bind(*this); // export
  }

  void run() {
    ini_socket->b_transport(gp, time);
    ini_socket->nb_transport_fw(gp, phase, time);
    ini_socket->get_direct_mem_ptr(gp, dmi);
    ini_socket->transport_dbg(gp);
  }

  tlm_sync_enum nb_transport_bw(tlm_generic_payload &gp, tlm_phase &phase, sc_time &time) override {
    cout << "nb_transport_bw\n";
    return TLM_ACCEPTED;
  }

  void invalidate_direct_mem_ptr(sc_dt::uint64 start_range, sc_dt::uint64 end_range) override {
    cout << "invalidate_direct_mem_ptr\n";
  }
};


struct Target : public sc_module, tlm_fw_transport_if<> {
  SC_HAS_PROCESS(Target);

  tlm_target_socket<> tgt_socket;
  tlm_generic_payload* gp;
  tlm_phase* phase;
  sc_time* time;
  tlm_dmi* dmi;

  Target(sc_module_name name): sc_module(name), tgt_socket("tgt_socket") {
    SC_THREAD(run);
    tgt_socket.bind(*this); // export
  }

  void run() {
    wait(1, SC_NS);
    tgt_socket->nb_transport_bw(*gp, *phase, *time);
    tgt_socket->invalidate_direct_mem_ptr(20, 40);
  }

  void b_transport(tlm_generic_payload &gp, sc_time &time) {
    cout << "b_transport\n";
  }

  tlm_sync_enum nb_transport_fw(tlm_generic_payload &gp, tlm_phase &phase, sc_time &time) override {
    cout << "nb_transport_fw\n";
    return TLM_COMPLETED;
  }

  bool get_direct_mem_ptr(tlm_generic_payload &gp, tlm_dmi &dmi) override {
    cout << "get_direct_mem_ptr\n";
    return true;
  }

  int unsigned transport_dbg(tlm_generic_payload &gp) {
    cout << "transport_dbg\n";
    return 0;
  }
};


int sc_main(int argc, char* argv[]) {
  Initiator ini("ini");
  Target tgt("tgt");
  ini.ini_socket(tgt.tgt_socket);

  sc_start(2, SC_NS);

  return 0;
}
