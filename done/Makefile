## ======================================================================
## partial Makefile provided to students
##

GTK_INCLUDE := `pkg-config --cflags gtk+-3.0`
GTK_LIBS := `pkg-config --libs gtk+-3.0`

.PHONY: clean new style feedback submit1 submit2 submit

CFLAGS += -std=c11 -Wall -pedantic -g -D_DEFAULT_SOURCE

# uncomment to check for memory leaks
# CFLAGS += -fsanitize=address
# CPPFLAGS += -fsanitize=address
# LDFLAGS += -fsanitize=address

# a bit more checks if you'd like to (uncomment
 CFLAGS += -Wextra -Wfloat-equal -Wshadow                         \
 -Wpointer-arith -Wbad-function-cast -Wcast-align -Wwrite-strings \
 -Wunreachable-code
 

# uncomment if you want to add DEBUG flag
# CPPFLAGS += -DDEBUG
CPPFLAGS += -DBLARGG

# ----------------------------------------------------------------------
# feel free to update/modifiy this part as you wish

# all those libs are required on Debian, feel free to adapt it to your box
LDLIBS += -lcheck -lm -lrt -pthread -lsubunit
LDFLAGS += -L.
LDLIBS += -lcs212gbfinalext-debug

all:: gbsimulator test-gameboy test-cpu-week08 test-cpu-week09 unit-tests

unit-tests: unit-test-bit unit-test-alu unit-test-bus \
	unit-test-memory unit-test-component unit-test-cpu \
	unit-test-cpu-dispatch-week08 unit-test-cpu-dispatch-week09 \
	unit-test-cartridge unit-test-timer unit-test-alu_ext unit-test-cpu-dispatch \
	unit-test-bit-vector

gbsimulator: LDLIBS += $(GTK_LIBS) -lsid
gbsimulator.o: CFLAGS += $(GTK_INCLUDE)

test-gameboy: LDLIBS += $(GTK_LIBS) -lsid
test-gameboy.o: CFLAGS += $(GTK_INCLUDE)

gbsimulator: gbsimulator.o libsid.so gameboy.o bus.o memory.o \
 component.o error.o bit.o cpu.o alu.o opcode.o cartridge.o timer.o \
 lcdc.h bit_vector.o joypad.h error.o cpu-storage.o cpu-alu.o cpu-registers.o \
 bootrom.o alu_ext.h image.o

gbsimulator.o: gbsimulator.c sidlib.h gameboy.h bus.h memory.h \
 component.h error.h bit.h cpu.h alu.h opcode.h cartridge.h timer.h \
 lcdc.h bit_vector.h joypad.h error.h cpu-storage.h cpu-alu.h cpu-registers.h \
 bootrom.h alu_ext.h image.o

test-cpu-week08: test-cpu-week08.o opcode.o bit.o cpu.o alu.o bus.o \
 memory.o component.o cpu-storage.o cpu-registers.o cpu-alu.o error.o image.o \
 bit_vector.o
test-cpu-week09: test-cpu-week09.o opcode.o bit.o cpu.o alu.o bus.o \
 memory.o component.o cpu-storage.o cpu-registers.o cpu-alu.o error.o image.o \
 bit_vector.o
test-gameboy: test-gameboy.o gameboy.o bus.o memory.o component.o \
 bit.o cpu.o alu.o opcode.o cartridge.o timer.o util.o  \
 bootrom.o cpu-storage.o cpu-registers.o cpu-alu.o error.o \
 lcdc.h joypad.h bit_vector.o image.o


unit-test-alu: unit-test-alu.o alu.o bit.o tests.h
unit-test-bit: unit-test-bit.o alu.o bit.o tests.h
unit-test-bus: unit-test-bus.o bus.o bit.o component.o memory.o tests.h error.o
unit-test-memory: unit-test-memory.o bus.o bit.o component.o memory.o tests.h error.o
unit-test-component: unit-test-component.o bus.o bit.o component.o memory.o tests.h error.o
unit-test-gameboy: unit-test-gameboy.o gameboy.o component.o memory.o bus.o bit.o cpu.o tests.h \
	cpu-storage.o opcode.o cpu-registers.o cpu-alu.o alu.o bootrom.o cartridge.o timer.o error.o \
	alu_ext.h lcdc.h joypad.h bit_vector.o image.o
unit-test-cpu: unit-test-cpu.o tests.h error.o alu.o bit.o opcode.o \
 cpu.o bus.o memory.o component.o cpu-registers.o cpu-storage.o \
 cpu-alu.o bit_vector.o image.o
unit-test-cpu-dispatch-week08: unit-test-cpu-dispatch-week08.o bit.o alu.o cpu.h bus.o cpu-storage.o \
	cpu-registers.o opcode.o component.o memory.o cpu-alu.o error.o lcdc.h joypad.h bit_vector.o image.o
unit-test-cpu-dispatch-week09: unit-test-cpu-dispatch-week09.o bit.o alu.o cpu.h bus.o cpu-storage.o \
	cpu-registers.o opcode.o component.o memory.o cpu-alu.o error.o lcdc.h joypad.h bit_vector.o image.o
unit-test-cartridge: unit-test-cartridge.o tests.h cartridge.o \
 component.o memory.o bus.o bit.o cpu.h alu.o opcode.o error.o image.o bit_vector.o
unit-test-timer: unit-test-timer.o tests.h timer.o \
 component.o memory.o bit.o cpu.o cpu-storage.o cpu-registers.o cpu-alu.o alu.o bus.o opcode.o \
  error.o bit_vector.o image.o
unit-test-alu_ext: unit-test-alu_ext.o tests.h error.o alu.o bit.o \
 alu_ext.h cpu-alu.o cpu-storage.o cpu-registers.o bus.o bit_vector.o cpu.o component.o\
  opcode.o memory.o image.o
unit-test-cpu-dispatch: unit-test-cpu-dispatch.o error.o alu.o \
 bit.o bus.o memory.o component.o opcode.o util.h \
 cpu-storage.o cpu-registers.o cpu-alu.o bit_vector.o image.o
unit-test-bit-vector: unit-test-bit-vector.o tests.h error.o \
 bit_vector.o bit.o image.h image.o


alu.o: alu.c alu.h bit.h error.h
bit.o: bit.c bit.h
error.o: error.c error.h
util.o: util.c
bus.o: bus.c memory.h component.h bit.h bus.h error.h 
memory.o: memory.c memory.h error.h
component.o: component.c memory.h error.h component.h
opcode.o: opcode.c opcode.h bit.h 
cpu.o: cpu.c alu.h bit.h bus.h memory.h component.h error.h cpu.h \
 opcode.h cpu-storage.h util.h
cpu-storage.o: cpu-storage.c error.h cpu-storage.h memory.h opcode.h \
 bit.h cpu.h alu.h bus.h component.h cpu-registers.h gameboy.h util.h \
 lcdc.h joypad.h
cpu-registers.o: cpu-registers.c bit.h cpu.h alu.h bus.h memory.h \
 component.h error.h opcode.h cpu-registers.h
cpu-storage.o: cpu-storage.c error.h cpu-storage.h memory.h opcode.h \
 bit.h cpu.h alu.h bus.h component.h cpu-registers.h gameboy.h util.h
cpu-registers.o: cpu-registers.c bit.h cpu.h alu.h bus.h memory.h \
 component.h error.h opcode.h cpu-registers.h
gameboy.o: gameboy.c bus.h memory.h component.h error.h bit.h gameboy.h \
 cpu.h alu.h opcode.h bootrom.h timer.h util.h lcdc.h joypad.h
cpu-alu.o: cpu-alu.c error.h bit.h alu.h cpu-alu.h opcode.h cpu.h bus.h \
 memory.h component.h cpu-storage.h cpu-registers.h alu_ext.h
bootrom.o: bootrom.c bus.h memory.h component.h error.h bit.h gameboy.h \
 cpu.h alu.h opcode.h bootrom.h lcdc.h joypad.h
cartridge.o: cartridge.c component.h memory.h error.h bus.h bit.h \
 cartridge.h
timer.o: timer.c component.h memory.h error.h bit.h cpu.h alu.h bus.h \
 opcode.h timer.h
bit_vector.o: bit_vector.c bit.h bit_vector.h
test-gameboy.o: test-gameboy.c gameboy.h bus.h memory.h component.h \
 error.h bit.h cpu.h alu.h opcode.h cartridge.h timer.h util.h
image.o: image.c error.h image.h bit_vector.h bit.h

unit-test-alu.o: unit-test-alu.c tests.h error.h alu.h bit.h
unit-test-bit.o: unit-test-bit.c tests.h error.h bit.h
unit-test-bus.o: unit-test-bus.c tests.h error.h bus.h memory.h component.h bit.h util.h
unit-test-component.o: unit-test-component.c tests.h error.h bus.h memory.h component.h
unit-test-memory.o: unit-test-memory.c tests.h error.h bus.h memory.h component.h
unit-test-gameboy.o: unit-test-gameboy.c tests.h error.h bus.h memory.h \
 component.h bit.h gameboy.h cpu.h alu.h opcode.h cpu-storage.h util.h bootrom.h timer.h cartridge.h \
 alu_ext.h lcdc.h joypad.h
unit-test-cpu.o: unit-test-cpu.c tests.h error.h alu.h bit.h opcode.h \
 util.h cpu.h bus.h memory.h component.h cpu-registers.h cpu-storage.h \
 cpu-alu.h
unit-test-cpu-dispatch-week08.o: unit-test-cpu-dispatch-week08.c tests.h \
 error.h alu.h bit.h cpu.h bus.h memory.h component.h opcode.h gameboy.h \
 util.h unit-test-cpu-dispatch.h cpu.c cpu-storage.h cpu-registers.h cpu-alu.h 
test-cpu-week08.o: test-cpu-week08.c opcode.h bit.h cpu.h alu.h bus.h \
 memory.h component.h error.h cpu-storage.h util.h lcdc.h joypad.h
test-cpu-week09.o: test-cpu-week09.c opcode.h bit.h cpu.h alu.h bus.h \
 memory.h component.h error.h cpu-storage.h util.h lcdc.h joypad.h
unit-test-cpu-dispatch-week09.o: unit-test-cpu-dispatch-week09.c tests.h \
 error.h alu.h bit.h cpu.h bus.h memory.h component.h opcode.h gameboy.h \
 util.h unit-test-cpu-dispatch.h cpu.c cpu-storage.h cpu-registers.h cpu-alu.h
unit-test-cartridge.o: unit-test-cartridge.c tests.h error.h cartridge.h \
 component.h memory.h bus.h bit.h cpu.h alu.h opcode.h
unit-test-timer.o: unit-test-timer.c util.h tests.h error.h timer.h \
 component.h memory.h bit.h cpu.h alu.h bus.h opcode.h
unit-test-alu_ext.o: unit-test-alu_ext.c tests.h error.h alu.h bit.h \
 alu_ext.h bit_vector.h cpu.h
unit-test-cpu-dispatch.o: unit-test-cpu-dispatch.c tests.h error.h alu.h \
 bit.h cpu.h bus.h memory.h component.h opcode.h util.h \
 unit-test-cpu-dispatch.h cpu-storage.h cpu-registers.h cpu-alu.h
unit-test-bit-vector.o: unit-test-bit-vector.c tests.h error.h \
 bit_vector.h bit.h image.h
test-image.o: test-image.c error.h util.h bit_vector.h bit.h \
 libsid.so 

TARGETS := 
CHECK_TARGETS := unit-test-bit unit-test-alu unit-test-bus \
	unit-test-memory unit-test-component unit-test-cpu \
	unit-test-cpu-dispatch-week08 unit-test-cpu-dispatch-week09 \
	unit-test-cartridge unit-test-timer unit-test-alu_ext unit-test-cpu-dispatch \
	unit-test-bit-vector
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

test-image.o: CFLAGS += $(GTK_INCLUDE)

test-image: LDLIBS += $(GTK_LIBS) -lsid
test-image: test-image.o error.o util.o bit_vector.o bit.o libsid.so image.o
