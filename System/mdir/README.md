# Qmdir

Microware-C implementation of the OS-9 `mdir` module-directory display.
The delivered file is `Qmdir`; the module header remains `mdir`.

The directory copy is obtained with `F$GModDr`. The safe listing and extended
pointer metadata are functional. Header names and type/name filtering require
the Q9 kernel implementation of `F$CpyMem` and are kept explicitly guarded
until that service is available.
