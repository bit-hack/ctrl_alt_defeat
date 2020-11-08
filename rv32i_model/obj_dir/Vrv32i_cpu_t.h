// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Primary design header
//
// This header should be included by all source files instantiating the design.
// The class here is then constructed to instantiate the design.
// See the Verilator manual for examples.

#ifndef _VRV32I_CPU_T_H_
#define _VRV32I_CPU_T_H_  // guard

#include "verilated.h"

//==========

class Vrv32i_cpu_t__Syms;

//----------

VL_MODULE(Vrv32i_cpu_t) {
  public:
    
    // PORTS
    // The application code writes and reads these signals to
    // propagate new values into/out from the Verilated model.
    VL_IN8(clk,0,0);
    VL_IN8(reset,0,0);
    VL_IN8(hold,0,0);
    VL_OUT8(out_write_mask,3,0);
    VL_IN(in_data,31,0);
    VL_OUT(out_mem_addr,31,0);
    VL_OUT(out_data,31,0);
    
    // LOCAL SIGNALS
    // Internals; generally not touched by application code
    CData/*0:0*/ rv32i_cpu_t__DOT__mem_write;
    CData/*2:0*/ rv32i_cpu_t__DOT__mem_width;
    CData/*3:0*/ rv32i_cpu_t__DOT__mask;
    CData/*4:0*/ rv32i_cpu_t__DOT__rd;
    CData/*2:0*/ rv32i_cpu_t__DOT__access_width;
    CData/*0:0*/ rv32i_cpu_t__DOT__will_read;
    CData/*0:0*/ rv32i_cpu_t__DOT__will_store;
    CData/*0:0*/ rv32i_cpu_t__DOT__write_rd;
    CData/*1:0*/ rv32i_cpu_t__DOT__phi;
    IData/*31:0*/ rv32i_cpu_t__DOT__mem_in;
    IData/*31:0*/ rv32i_cpu_t__DOT__pc;
    IData/*31:0*/ rv32i_cpu_t__DOT__inst;
    IData/*31:0*/ rv32i_cpu_t__DOT__immi;
    IData/*31:0*/ rv32i_cpu_t__DOT__pc_branch;
    IData/*31:0*/ rv32i_cpu_t__DOT__next_pc;
    IData/*31:0*/ rv32i_cpu_t__DOT__rhs;
    IData/*31:0*/ rv32i_cpu_t__DOT__res_alu;
    IData/*31:0*/ rv32i_cpu_t__DOT__X[32];
    
    // LOCAL VARIABLES
    // Internals; generally not touched by application code
    CData/*5:0*/ __Vtableidx1;
    CData/*4:0*/ __Vtableidx2;
    CData/*0:0*/ __Vclklast__TOP__clk;
    IData/*31:0*/ rv32i_cpu_t__DOT____Vcellinp__out_shift____pinNumber3;
    static CData/*0:0*/ __Vtable1_rv32i_cpu_t__DOT__write_rd[64];
    static CData/*3:0*/ __Vtable2_rv32i_cpu_t__DOT__mask[32];
    
    // INTERNAL VARIABLES
    // Internals; generally not touched by application code
    Vrv32i_cpu_t__Syms* __VlSymsp;  // Symbol table
    
    // CONSTRUCTORS
  private:
    VL_UNCOPYABLE(Vrv32i_cpu_t);  ///< Copying not allowed
  public:
    /// Construct the model; called by application code
    /// The special name  may be used to make a wrapper with a
    /// single model invisible with respect to DPI scope names.
    Vrv32i_cpu_t(const char* name = "TOP");
    /// Destroy the model; called (often implicitly) by application code
    ~Vrv32i_cpu_t();
    
    // API METHODS
    /// Evaluate the model.  Application must call when inputs change.
    void eval();
    /// Simulation complete, run final blocks.  Application must call on completion.
    void final();
    
    // INTERNAL METHODS
  private:
    static void _eval_initial_loop(Vrv32i_cpu_t__Syms* __restrict vlSymsp);
  public:
    void __Vconfigure(Vrv32i_cpu_t__Syms* symsp, bool first);
  private:
    static QData _change_request(Vrv32i_cpu_t__Syms* __restrict vlSymsp);
  public:
    static void _combo__TOP__3(Vrv32i_cpu_t__Syms* __restrict vlSymsp);
  private:
    void _ctor_var_reset() VL_ATTR_COLD;
  public:
    static void _eval(Vrv32i_cpu_t__Syms* __restrict vlSymsp);
  private:
#ifdef VL_DEBUG
    void _eval_debug_assertions();
#endif  // VL_DEBUG
  public:
    static void _eval_initial(Vrv32i_cpu_t__Syms* __restrict vlSymsp) VL_ATTR_COLD;
    static void _eval_settle(Vrv32i_cpu_t__Syms* __restrict vlSymsp) VL_ATTR_COLD;
    static void _sequent__TOP__1(Vrv32i_cpu_t__Syms* __restrict vlSymsp);
    static void _settle__TOP__2(Vrv32i_cpu_t__Syms* __restrict vlSymsp) VL_ATTR_COLD;
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);

//----------


#endif  // guard
