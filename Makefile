FINDDIRS = find . -type d -print

rwildcard := $(foreach d,$(wildcard $1*),$(call rwildcard,$d/) \
	$(filter $(subst *,%,$2),$d))

OBJS_DIR	:= build/obj
TARGET		 = churn
TARGETS		 = churn
TARG_FILE	 = target
BUILD		 = dbg
TARG_SRCS	:= $(foreach dir,$(SUBDIRS),$(notdir \
	$(call rwildcard $(dir),*.c))) $(wildcard *.c)
TARG_OBJS	:= $(addprefix $(OBJS_DIR)/, $(TARG_SRCS:.c=.o))

SRCS		:= $(TARG_SRCS)
OBJS		:= $(TARG_OBJS)
DEP_DIR		:= build/deps
DEPS		:= $(addprefix $(DEP_DIR)/, ${SRCS:.c=.c.d})
OTHERDIRS	:= scripts data notes include $(DEP_DIR)
SUBDIRS 	:= $(filter_out $(OTHERDIRS), $(shell $(FINDDIRS)))

CFLAGS		:= -std=c89 -Wall -Wextra -pedantic -Iinclude
LDFLAGS 	:= -std=c89 -Wall -Wextra -pedantic -Iinclude
DEPFLAGS	:= $(if $(filter cc gcc, $(CC)),-MM -MG, -M)

.PHONY: all clean deps debug release profile build_options

all: build_options $(TARGETS) scripts
deps: $(DEPS)

debug:
	$(MAKE) BUILD=dbg all
release:
	$(MAKE) BUILD=release all
profile:
	$(MAKE) BUILD=prof all
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


$(OBJS_DIR)/%.o:
	$(CC) $(CFLAGS) -o $@ -c $*.c

$(DEP_DIR)/%.c.d: %.c
	$(CC) $(CFLAGS) $(DEPFLAGS) $< | \
	sed -n "H;$$ {g;s@.*:\(.*\)@$*.o $@: \$$\(wildcard\1\)@;p}" > $@
clean:
	$(RM) -r *~ $(OBJS) $(TARGETS)

include $(wildcard $(DEPS))
include scripts/Makefile