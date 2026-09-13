APP     = Qload
SOURCE  = load.c
MODULE  = load
CFLAGS  = -b -a=warn -O7 -cq -cw -co -to=OSK -tp=68000
LFLAGS  = -b -i -l=$(MWOS)/OS9/68000/LIB/unix.l -l=$(MWOS)/OS9/68000/LIB/sys_clib.l
DEL     = os9del -qf
COPY    = copy

$(APP): $(SOURCE)
	xcc $(CFLAGS) $(LFLAGS) -mw=$(MWOS) -f=$(MODULE) $(SOURCE)
	$(DEL) $(APP)
	$(COPY) $(MODULE) $(APP)
