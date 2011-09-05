SCRIPTS_DIR	:= scripts
SCRIPTS_SRCS    := $(wildcard $(SCRIPTS_DIR)/*.c)
SCRIPTS_TARGETS := $(SCRIPTS_SRCS:.c=)
SCRIPTS_DEPS	:=  $(addprefix $(DEP_DIR)/, $(SCRIPTS_SRCS:.c=.c.d))

TARGETS 	+= $(SCRIPTS_TARGETS)
DEPS		+= $(SCRIPTS_DEPS)
SCRS		+= $(SCRIPTS_SRCS)

.PHONY: deps scripts
scripts: $(SCRIPTS_TARGETS)
deps: $(SCRIPTS_DEPS)

$(SCRIPTS_DEPS): $(DEP_DIR)/%.c.d: %.c
	$(CC) $(CFLAGS) -MM $< | \
	sed -n "H;$$ {g;s@.*:\(.*\)@$*.o $@: \$$\(wildcard\1\)@;p}" | \
	sed -e "s/\.o//g" > $@

$(SCRIPTS_TARGETS): %: %.c $(DEP_DIR)/%.c.d $(OBJS_DIR)/utils.o
	$(CC) $(CFLAGS) $< $(OBJS_DIR)/utils.o -o $@

