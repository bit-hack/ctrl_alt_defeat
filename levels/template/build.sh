TOOLCHAIN=../riscv64-unknown-elf-gcc-8.3.0-2020.04.0-x86_64-w64-mingw32

BASE=out

$TOOLCHAIN/bin/riscv64-unknown-elf-gcc-8.3.0.exe \
-Iincludes \
-static \
-mcmodel=medany \
-nostdlib \
-nostartfiles \
-march=rv32im \
-mabi=ilp32 \
-fno-reorder-functions \
-o $BASE.elf \
-O3 \
-T ../link.ld \
../entry.S \
main.c

$TOOLCHAIN/bin/riscv64-unknown-elf-objdump.exe \
-S \
-t \
$BASE.elf > $BASE.dasm
