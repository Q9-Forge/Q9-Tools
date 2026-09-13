# Qmfree

Microware-C implementation of the OS-9 `mfree` system-memory display.
The delivered file is `Qmfree`; the module header remains `mfree`.

The summary and extended map use the local `q9_gblkmp.r` wrapper around
`F$GBlkMp`. The wrapper copies fragment address/size pairs and returns the
minimum allocation size, fragment count, total RAM, and total free RAM.
