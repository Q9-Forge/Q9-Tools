APP=Qqsort
SOURCE=qsort.c
MODULE=qsort
CFLAGS=-b -a=warn -O7 -cq -cw -co -to=OSK -tp=68000
LFLAGS=-b -i -l=$(MWOS)/OS9/68000/LIB/unix.l -l=$(MWOS)/OS9/68000/LIB/sys_clib.l
$(APP): $(SOURCE)
	xcc $(CFLAGS) $(LFLAGS) -mw=$(MWOS) -f=$(MODULE) $(SOURCE)
	copy $(MODULE) $(APP)
