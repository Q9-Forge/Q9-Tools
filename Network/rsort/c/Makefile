APP=Qrsort
SOURCE=rsort_bundle.c
MODULE=rsort
CFLAGS=-b -a=warn -O7 -cq -cw -co -to=OSK -tp=68000 -v=$(MWOS)/SRC/DEFS/SPF/BSD -v=$(MWOS)/SRC/DEFS/SPF
LFLAGS=-b -i -l=$(MWOS)/OS9/68000/LIB/rpc.l -l=$(MWOS)/OS9/68000/LIB/socket.l -l=$(MWOS)/OS9/68000/LIB/netdb.l -l=$(MWOS)/OS9/68000/LIB/ndblib.l -l=$(MWOS)/OS9/68000/LIB/unix.l -l=$(MWOS)/OS9/68000/LIB/sys_clib.l -l=$(MWOS)/OS9/68000/LIB/os_lib.l -l=$(MWOS)/OS9/68000/LIB/conv_lib.l -l=$(MWOS)/OS9/68000/LIB/item.l
all: $(APP) Qsortd
$(APP): $(SOURCE)
	xcc $(CFLAGS) $(LFLAGS) -mw=$(MWOS) -f=$(MODULE) $(SOURCE)
	os9del -qf $(APP)
	copy $(MODULE) $(APP)
Qsortd: rsort_server.c
	xcc $(CFLAGS) $(LFLAGS) -mw=$(MWOS) -f=sortd $(MWOS)/SRC/SPF/RPC/DEMO/SORT/sort_xdr.c $(MWOS)/SRC/SPF/RPC/DEMO/SORT/sort_proc.c $(MWOS)/SRC/SPF/RPC/DEMO/SORT/sort_svc.c
	os9del -qf Qsortd
	copy sortd Qsortd
