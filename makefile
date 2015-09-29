PROGRAM = rktchain
SOURCES = src
BUILD   = build
DEVEXT  = -dev

CC     = g++
FLAGS  = -O2 -Werror -Wall -Wextra -Wfloat-equal -Wshadow -Wcast-align -Wunreachable-code -Wunused-variable -std=c++11
CFLAGS = 
LIBS   = 
MKDIR  = mkdir -p
RM     = rm -rf

MODE = release
BNRY = $(PROGRAM)
SDRS = $(shell find $(SOURCES) -type d | xargs echo)
SRCS = $(filter-out %.inc.cpp,$(foreach d,$(SDRS),$(wildcard $(addprefix $(d)/*,.cpp))))
OBJS = $(patsubst %.cpp,%.o,$(addprefix $(BUILD)/$(MODE)/,$(SRCS)))
DEPS = $(OBJS:%.o=%.d)
DIRS = $(sort $(dir $(OBJS)))

ifdef DEBUG
	BNRY  := $(BNRY)$(DEVEXT)
	FLAGS += -g -fsanitize=address -DDEBUG_MODE
	MODE   = debug
endif

.PHONY: all clean run

all: $(BNRY)

clean:
	$(RM) $(BUILD) $(PROGRAM) $(PROGRAM)$(DEVEXT)

run: $(BNRY)
	./$(BNRY)

$(DIRS):
	$(MKDIR) $@

$(BNRY): $(OBJS)
	$(CC) $(FLAGS) $(LIBS) $(OBJS) -o $@

$(OBJS): | $(DIRS)

$(BUILD)/$(MODE)/%.o: %.cpp
	$(CC) $(FLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)
