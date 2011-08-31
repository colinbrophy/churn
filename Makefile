TARGET  := churn
SRCS    := $(wildcard *.c)
OBJS    := ${SRCS:.c=.o}
DEPS    := $(addprefix ., ${SRCS:.c=.dep})
XDEPS   := $(wildcard ${DEPS})
SUBDIRS := scripts

CCFLAGS = -std=c89 -Wall -Wextra -pedantic
LDFLAGS =

CFG = debug

ifeq ($(CFG),debug)
CCFLAGS += -g
LDFLAGS += -g
else ifeq ($(CFG),release)
CCFLAGS += -O3 -s
LDFLAGS += -O3
else ifeq ($(CFG),profile)
CCFLAGS += -O3 -g
LDFLAGS += -O3 -g
endif



.PHONY: all clean distclean subdirs
all:: ${TARGET} subdirs

ifneq (${XDEPS},)
include ${XDEPS}
endif

${TARGET}: ${OBJS}
	${CC} ${LDFLAGS} -o $@ $^ ${LIBS}

${OBJS}: %.o: %.c .%.dep
	${CC} ${CCFLAGS} -o $@ -c $<

${DEPS}: .%.dep: %.c Makefile
	${CC} ${CCFLAGS} -MM $< > $@

subdirs:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

subdirsclean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

clean: subdirsclean
	${RM} *~ *.o *.dep ${TARGET}


distclean:: clean
