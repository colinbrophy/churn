FINDDIRS = find . -type d -print

rwildcard := $(foreach d,$(wildcard $1*),$(call rwildcard,$d/) \
	$(filter $(subst *,%,$2),$d))

TARGET		 = churn
TARG_FILE	 = target
BUILD		 = dbg
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

.PHONY: all clean deps debug release profile build_options

all: build_options $(TARGET)
deps: $(DEPS)

debug:
	$(MAKE) BUILD=dbg
release:
	$(MAKE) BUILD=release
profile:
	$(MAKE) BUILD=prof
dbg_CFLAGS = -g
dbg_LDFLAGS =

release_CFLAGS = -O3 -s
release_LDFLAGS = -O3

prof_CFLAGS  = -O3 -g
prof_LDFLAGS = -O3

include $(wildcard $(TARG_FILE))

CFLAGS += $($(addprefix $(BUILD), _CFLAGS))
LDFLAGS += $($(addprefix $(BUILD), _LDFLAGS))

build_options:
	$(if $(filter $(BUILD),$(CURRENT_TARGET)),,\
		$(MAKE) clean; \
		echo "CURRENT_TARGET = $(BUILD)" > $(TARG_FILE) \
	)

$(TARGET): $(TARG_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

.%.c.d: %.c
	$(CC) $(CFLAGS) $(DEPFLAGS) $< | \
	sed -n "H;$$ {g;s@.*:\(.*\)@$*.o $@: \$$\(wildcard\1\)@;p}" > $@
clean:
	$(RM) -r *~ $(OBJS) $(TARGET)

include $(DEPS)
