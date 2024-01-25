TARGETS = test_lkm2 test_lkm4
KDIR=/lib/modules/`uname -r`/build

SOURCES = $(TARGETS:=.c)

all: $(TARGETS) kbuild

$(TARGETS): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $@.c

kbuild:
	make -C $(KDIR) M=`pwd`

clean:
	rm -f $(TARGETS)
	make -C $(KDIR) M=`pwd` clean

