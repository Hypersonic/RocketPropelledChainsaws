MACHINE = $(shell uname -s)
SOURCES = src
BUILD   = build
DEVEXT  = -dev

.PHONY: all clean

FLAGS  := -O3 -Werror -Wall -Wextra -Wfloat-equal -Wshadow -Wcast-align -Wunreachable-code -Wunused-variable -std=c++11 -Isrc/
BANK_FLAGS := -DBANK
ATM_FLAGS := -DATM
CFLAGS = 
ifeq ($(MACHINE), Linux)
	LIBS = -pthread -lssl -lcryptopp
else
	LIBS = -lpthread -lssl -lcryptopp
endif
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
	ATM_BNRY  := $(ATM_BNRY)$(DEVEXT)
	BANK_BNRY  := $(BANK_BNRY)$(DEVEXT)
	FLAGS += -g -DDEBUG_MODE
	MODE   = debug
endif




all: $(BANK_BNRY) $(ATM_BNRY)

clean:
	$(RM) $(BUILD) $(BANK_BNRY) $(ATM_BNRY) $(BANK_BNRY)$(DEVEXT) $(ATM_BNRY)$(DEVEXT) *.card *.auth


$(DIRS):
	$(MKDIR) $@

$(BANK_BNRY): $(BANK_OBJS)
	$(CXX) $(FLAGS) -DBANK $(BANK_OBJS) -o $@ $(LIBS)

$(ATM_BNRY): $(ATM_OBJS)
	$(CXX) $(FLAGS) -DATM $(ATM_OBJS) -o $@ $(LIBS)

$(BANK_OBJS): | $(DIRS)

$(ATM_OBJS): | $(DIRS)

$(BUILD)/atm/$(MODE)/%.o: %.cpp
	$(CXX) $(FLAGS) $(ATM_FLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD)/bank/$(MODE)/%.o: %.cpp
	$(CXX) $(FLAGS) $(BANK_FLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)
