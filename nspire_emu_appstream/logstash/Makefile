GCC = gcc
LD = gcc
GCCFLAGS = -Wall -W
OBJS = $(patsubst %.c,%.o,$(wildcard *.c)) $(patsubst %.S,%.o,$(wildcard *.S)) $(CPPOBJS)
EXE = run_agent.exe
DISTDIR = .

all: $(EXE)

%.o: %.c
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	mkdir -p $(DISTDIR)
	$(LD) $^ -o $(DISTDIR)/$@ $(LDFLAGS)

clean:
	rm -f *.o $(DISTDIR)/$(EXE)
