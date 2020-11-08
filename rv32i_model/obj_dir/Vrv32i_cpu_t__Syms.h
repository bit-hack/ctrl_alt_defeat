// Verilated -*- C++ -*-
// DESCRIPTION: Verilator output: Symbol table internal header
//
// Internal details; most calling programs do not need this header,
// unless using verilator public meta comments.

#ifndef _VRV32I_CPU_T__SYMS_H_
#define _VRV32I_CPU_T__SYMS_H_  // guard

#include "verilated.h"

// INCLUDE MODULE CLASSES
#include "Vrv32i_cpu_t.h"

// SYMS CLASS
class Vrv32i_cpu_t__Syms : public VerilatedSyms {
  public:
    
    // LOCAL STATE
    const char* __Vm_namep;
    bool __Vm_didInit;
    
    // SUBCELL STATE
    Vrv32i_cpu_t*                  TOPp;
    
    // CREATORS
    Vrv32i_cpu_t__Syms(Vrv32i_cpu_t* topp, const char* namep);
    ~Vrv32i_cpu_t__Syms() {}
    
    // METHODS
    inline const char* name() { return __Vm_namep; }
    
} VL_ATTR_ALIGNED(VL_CACHE_LINE_BYTES);

#endif  // guard
