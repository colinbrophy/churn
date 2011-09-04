FINDDIRS = find . -type d -print

rwildcard = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/) \
	$(filter $(subst *,%,$2),$d))

TARGET		 = churn
TARG_DBG	 = .dbg
TARG_RELEASE	 = .release
TARG_PROF	 = .prof
TARGETS		:= $(TARG_DBG) $(TARG_RELEASE) $(TARG_PROF)
TARG_SRCS	:= $(foreach dir,$(SUBDIRS),$(notdir \
	$(call rwildcard $(dir),*.c))) $(wildcard *.c)
TARG_OBJS	:= $(TARG_SRCS:.c=.o)

SRCS		:= $(TARG_SRCS)
OBJS		:= $(TARG_OBJS)
DEPS		:= $(addprefix ., ${SRCS:.c=.c.d})
OTHERDIRS	 = scripts data notes
SUBDIRS 	:= $(filter_out $(OTHERDIRS), $(shell $(FINDDIRS)))

CFLAGS		:= -std=c89 -Wall -Wextra -pedantic
LDFLAGS 	:= -std=c89 -Wall -Wextra -pedantic
DEPFLAGS	:= $(if $(filter cc gcc, $(CC)),-MM -MG, -M)

.PHONY: all clean deps

all: debug
deps: $(DEPS)

debug: CFLAGS += -g
debug: $(TARG_DBG) $(TARGET)
	touch $(TARG_RELEASE)
	touch $(TARG_PROF)

release: CFLAGS += -O3 -s
release: LDFLAGS += -O3
release: $(TARG_RELEASE) $(TARGET)
	touch $(TARG_DBG)
	touch $(TARG_PROF)

profile: CFLAGS += -O3 -g
profile: LDFLAGS += -O3
profile: $(TARG_PROF) $(TARGET)
	touch $(TARG_RELEASE)
	touch $(TARG_DBG)

$(TARGETS): clean

$(TARGET): $(TARG_OBJS)
	echo $(TARG_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

.%.c.d: %.c
	$(CC) $(CFLAGS) $(DEPFLAGS) $< | \
	sed -n "H;$$ {g;s@.*:\(.*\)@$*.o $@: \$$\(wildcard\1\)@;p}" > $@
clean:
	$(RM) -r *~ $(OBJS) $(TARGET)

include $(DEPS)
