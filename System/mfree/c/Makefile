APP     = Qmfree
SOURCE  = mfree.c
MODULE  = mfree
WRAPPER = q9_gblkmp.r
CFLAGS  = -b -a=warn -O7 -cq -cw -co -to=OSK -tp=68000
LFLAGS  = -b -i -l=$(MWOS)/OS9/68000/LIB/unix.l -l=$(MWOS)/OS9/68000/LIB/sys_clib.l -l=$(MWOS)/OS9/68000/LIB/cpu.l
DEL     = os9del -qf
COPY    = copy

$(APP): $(SOURCE) $(WRAPPER)
	xcc $(CFLAGS) $(LFLAGS) -mw=$(MWOS) -f=$(MODULE) $(WRAPPER) $(SOURCE)
	$(DEL) $(APP)
	$(COPY) $(MODULE) $(APP)

$(WRAPPER): q9_gblkmp.a
	r68 -q -bt -m0 -o=$(WRAPPER) -u=$(MWOS)/OS9/68030/PORTS/CB030 -u=$(MWOS)/OS9/SRC/DEFS -u=$(MWOS)/OS9/SRC/MACROS q9_gblkmp.a
