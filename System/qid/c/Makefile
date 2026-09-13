APP     = qid
SOURCE  = qid.c qid_cpymem.a
MODULE  = qid
CFLAGS  = -b -a=warn -O7 -cq -cw -co -to=OSK -tp=68000
# -olM=4k: l68's -M (stack memory) is an "Object code linker" phase option;
# xcc's ucc-executive syntax routes phase-specific options via a
# "-<phasecode>" prefix with no separating hyphen (Using Ultra C/C++, ch.5:
# "ol" = Object code linker) -- plain "-M=4k" fails with "invalid phase for
# stack space option". The linker's 3072-byte default isn't enough for
# qid's directory-scanning code (verified against the real emulator: a
# directory with ~200 entries overflowed it).
LFLAGS  = -b -i -olM=4k -l=$(MWOS)/OS9/68000/LIB/unix.l -l=$(MWOS)/OS9/68000/LIB/sys_clib.l
DEL     = os9del -qf
COPY    = copy

$(APP): $(SOURCE)
	xcc $(CFLAGS) $(LFLAGS) -mw=$(MWOS) -f=$(MODULE) $(SOURCE)
	$(DEL) $(APP)
	$(COPY) $(MODULE) $(APP)
