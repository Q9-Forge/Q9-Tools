# Qprocs

Microware-C implementation of the OS-9 `procs` process-table display.
The delivered file is `Qprocs`; the module header remains `procs`.

The implementation uses `F$GPrDBT` and `F$GPrDsc` to copy process data into
the user address space. Kernel pointers in a copied descriptor are displayed
as addresses; they are not dereferenced, because the module name itself is
not user-readable through that pointer.
