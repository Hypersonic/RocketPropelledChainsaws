PROGRAM = rktchain
SOURCES = src
BUILD   = build
DEVEXT  = -dev

FLAGS  := -O2 -Werror -Wall -Wextra -Wfloat-equal -Wshadow -Wcast-align -Wunreachable-code -Wunused-variable -std=c++11
CFLAGS = 
LIBS   = 
MKDIR  = mkdir -p
RM     = rm -rf

MODE = release
BANK_BNRY = bank
ATM_BNRY = atm
SDRS = $(shell find $(SOURCES) -type d | xargs echo)
SRCS = $(filter-out %.inc.cpp,$(foreach d,$(SDRS),$(wildcard $(addprefix $(d)/*,.cpp))))
BANK_OBJS = $(patsubst %.cpp,%.o,$(addprefix $(BUILD)/bank/$(MODE)/,$(SRCS)))
ATM_OBJS = $(patsubst %.cpp,%.o,$(addprefix $(BUILD)/atm/$(MODE)/,$(SRCS)))
DEPS = $(OBJS:%.o=%.d)
DIRS = $(sort $(dir $(BANK_OBJS)))
DIRS += $(sort $(dir $(ATM_OBJS)))

ifdef DEBUG
	BNRY  := $(BNRY)$(DEVEXT)
	FLAGS += -g -DDEBUG_MODE
	MODE   = debug
endif

.PHONY: all clean

all: $(BANK_BNRY) $(ATM_BNRY)

clean:
	$(RM) $(BUILD) $(PROGRAM) $(PROGRAM)$(DEVEXT)

$(DIRS):
	$(MKDIR) $@

$(BANK_BNRY): $(BANK_OBJS)
	$(CXX) $(FLAGS) -DBANK $(LIBS) $(BANK_OBJS) -o $@

$(ATM_BNRY): $(ATM_OBJS)
	$(CXX) $(FLAGS) -DATM $(LIBS) $(ATM_OBJS) -o $@

$(BANK_OBJS): | $(DIRS)

$(ATM_OBJS): | $(DIRS)

$(BUILD)/atm/$(MODE)/%.o: %.cpp
	$(CXX) $(FLAGS) -DATM $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD)/bank/$(MODE)/%.o: %.cpp
	$(CXX) $(FLAGS) -DBANK $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)
