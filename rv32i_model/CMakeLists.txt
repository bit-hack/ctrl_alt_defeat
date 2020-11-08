
include_directories(verilator/include)

add_library(rv32i_model

  obj_dir/Vrv32i.h
  obj_dir/Vrv32i__Syms.h
  obj_dir/Vrv32i_cpu_t.cpp
  obj_dir/Vrv32i_cpu_t__Syms.cpp

  obj_dir/Vrv32i.cpp
  obj_dir/Vrv32i__Syms.cpp
  obj_dir/Vrv32i_cpu_t.cpp
  obj_dir/Vrv32i_cpu_t__Syms.cpp

  verilator/include/verilated.cpp

  elf.cpp
  elf.h
  memory.h
  model.cpp
  model.h
  )

target_compile_definitions(rv32i_model
  PUBLIC -DVL_CPPCHECK -D_CRT_SECURE_NO_WARNINGS)
