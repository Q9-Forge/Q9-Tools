APP=Qservice
SOURCE=service.c
MODULE=service
CFLAGS=-b -a=warn -O7 -cq -cw -co -to=OSK -tp=68000 -v=$(MWOS)/SRC/DEFS/SPF/BSD
LFLAGS=-b -i -l=$(MWOS)/OS9/68000/LIB/unix.l -l=$(MWOS)/OS9/68000/LIB/sys_clib.l -l=$(MWOS)/OS9/68000/LIB/os_lib.l -l=$(MWOS)/OS9/68000/LIB/conv_lib.l -l=$(MWOS)/OS9/68000/LIB/item.l -l=$(MWOS)/OS9/68000/LIB/netdb.l
DEL=os9del -qf
COPY=copy
$(APP): $(SOURCE)
	xcc $(CFLAGS) $(LFLAGS) -mw=$(MWOS) -f=$(MODULE) $(SOURCE)
	$(DEL) $(APP)
	$(COPY) $(MODULE) $(APP)
