# Qdevs

Microware-C implementation of the OS-9 `devs` system device-table display.
The delivered file is `Qdevs`; the module header remains `devs`.

The current implementation safely reads the device-table dimensions through
`_getsys()`. The individual entries remain kernel memory. A full name listing
is pending the Q9 kernel implementation of `F$CpyMem`.
