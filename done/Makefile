## ======================================================================
## partial Makefile provided to students
##

GTK_INCLUDE := `pkg-config --cflags gtk+-3.0`
GTK_LIBS := `pkg-config --libs gtk+-3.0`

.PHONY: clean new style feedback submit1 submit2 submit

CFLAGS += -std=c11 -Wall -pedantic -g

# a bit more checks if you'd like to (uncomment
 #CFLAGS += -Wextra -Wfloat-equal -Wshadow                         \
 -Wpointer-arith -Wbad-function-cast -Wcast-align -Wwrite-strings \
 -Wunreachable-code \
 -Wcast-qual -Winit-self \
 -Wconversion
 #-Wstrict-prototypes -Wmissing-declarations
 

# uncomment if you want to add DEBUG flag
CPPFLAGS += -DDEBUG

# ----------------------------------------------------------------------
# feel free to update/modifiy this part as you wish

# all those libs are required on Debian, feel free to adapt it to your box
LDLIBS += -lcheck -lm -lrt -pthread -lsubunit

all:: 

unit-test-alu: unit-test-alu.o alu.o bit.o tests.h
unit-test-bit: unit-test-bit.o alu.o bit.o tests.h
unit-test-bus: unit-test-bus.o bus.o bit.o component.o memory.o tests.h
unit-test-memory: unit-test-memory.o bus.o bit.o component.o memory.o tests.h
unit-test-component: unit-test-component.o bus.o bit.o component.o memory.o tests.h
unit-test-gameboy: unit-test-gameboy.o gameboy.o component.o memory.o bus.o bit.o cpu.o tests.h \
	cpu-storage.o opcode.o cpu-registers.o
unit-test-cpu: unit-test-cpu.o cpu-storage.o cpu-registers.o cpu.o  bit.o component.o \
	memory.o bus.o gameboy.o tests.h opcode.o
unit-test-cpu-dispatch-week08: unit-test-cpu-dispatch-week08.o bit.o alu.o cpu.h bus.o cpu-storage.o \
	cpu-registers.o opcode.o component.o memory.o

alu.o: alu.c alu.h bit.h error.h
bit.o: bit.c bit.h
error.o: error.c error.h
util.o: util.c
bus.o: bus.c memory.h component.h bit.h bus.h error.h 
memory.o: memory.c memory.h error.h
component.o: component.c memory.h error.h component.h
opcode.o: opcode.c opcode.h bit.h
cpu.o: cpu.c alu.h bit.h bus.h memory.h component.h error.h cpu.h \
 opcode.h cpu-storage.h
cpu-storage.o: cpu-storage.c error.h cpu-storage.h memory.h opcode.h \
 bit.h cpu.h alu.h bus.h component.h cpu-registers.h gameboy.h util.h
cpu-registers.o: cpu-registers.c bit.h cpu.h alu.h bus.h memory.h \
 component.h error.h opcode.h cpu-registers.h
cpu.o: cpu.c alu.h bit.h bus.h memory.h component.h error.h cpu.h \
 opcode.h cpu-storage.h
cpu-storage.o: cpu-storage.c error.h cpu-storage.h memory.h opcode.h \
 bit.h alu.h bus.h component.h cpu-registers.h gameboy.h util.h
cpu-registers.o: cpu-registers.c bit.h cpu.h alu.h bus.h memory.h \
 component.h error.h opcode.h cpu-registers.h
gameboy.o: gameboy.c bus.h memory.h component.h error.h bit.h gameboy.h \
 cpu.h alu.h opcode.h

unit-test-alu.o: unit-test-alu.c tests.h error.h alu.h bit.h
unit-test-bit.o: unit-test-bit.c tests.h error.h bit.h
unit-test-bus.o: unit-test-bus.c tests.h error.h bus.h memory.h component.h bit.h util.h
unit-test-component.o: unit-test-component.c tests.h error.h bus.h memory.h component.h
unit-test-memory.o: unit-test-memory.c tests.h error.h bus.h memory.h component.h
unit-test-gameboy.o: unit-test-gameboy.c tests.h error.h bus.h memory.h \
 component.h bit.h gameboy.h cpu.h
unit-test-cpu.o: unit-test-cpu.c tests.h error.h alu.h bit.h opcode.h \
 util.h cpu.h bus.h memory.h component.h cpu-registers.h cpu-storage.h \
 cpu-alu.h
unit-test-cpu-dispatch-week08.o: unit-test-cpu-dispatch-week08.c tests.h \
 error.h alu.h bit.h cpu.h bus.h memory.h component.h opcode.h gameboy.h \
 util.h unit-test-cpu-dispatch.h cpu.c cpu-storage.h cpu-registers.h

sidlib.o: sidlib.c sidlib.h

TARGETS := 
CHECK_TARGETS := unit-test-bit unit-test-alu unit-test-bus \
	unit-test-memory unit-test-component unit-test-gameboy unit-test-cpu \
	unit-test-cpu-dispatch-week08
OBJS = 
OBJS_NO_STATIC_TESTS =
OBJS_STATIC_TESTS = 
OBJS = $(OBJS_STATIC_TESTS) $(OBJS_NO_STATIC_TESTS)

# ----------------------------------------------------------------------
# This part is to make your life easier. See handouts how to make use of it.


clean::
	-@/bin/rm -f *.o *~ $(CHECK_TARGETS)

new: clean all

static-check:
	scan-build -analyze-headers --status-bugs -maxloop 64 make CC=clang new

style:
	astyle -n -o -A8 -xt0 *.[ch]

# all those libs are required on Debian, adapt to your box
$(CHECK_TARGETS): LDLIBS += -lcheck -lm -lrt -pthread -lsubunit

check:: $(CHECK_TARGETS)
	$(foreach target,$(CHECK_TARGETS),./$(target) &&) true

# target to run tests
check:: all
	@if ls tests/*.*.sh 1> /dev/null 2>&1; then \
	  for file in tests/*.*.sh; do [ -x $$file ] || echo "Launching $$file"; ./$$file || exit 1; done; \
	fi

IMAGE=chappeli/feedback:latest
feedback:
	@docker pull $(IMAGE)
	@docker run -it --rm -v ${PWD}:/home/tester/done $(IMAGE)

SUBMIT_SCRIPT=../provided/submit.sh
submit1: $(SUBMIT_SCRIPT)
	@$(SUBMIT_SCRIPT) 1

submit2: $(SUBMIT_SCRIPT)
	@$(SUBMIT_SCRIPT) 2

submit:
	@printf 'what "make submit"??\nIt'\''s either "make submit1" or "make submit2"...\n'

libsid.so: sidlib.c
	$(CC) -fPIC -shared $(CPPFLAGS) $(CFLAGS) $(GTK_INCLUDE) $^ -o $@

libsid_demo.o: CFLAGS += $(GTK_INCLUDE)

libsid_demo: LDFLAGS += -L.
libsid_demo: LDLIBS += $(GTK_LIBS) -lsid
libsid_demo: libsid_demo.o libsid.so
