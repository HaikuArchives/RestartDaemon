OUTPUT_DIR=bin
DEPEND_DIR=depends
OBJECT_DIR=objects

APP1NAME=restart_daemon
APP2NAME=micro_restarter
SAMPLEAPPNAME=rd_sample_app
APP1SIG="\"application/x-vnd.bls-restart_daemon\""
APP2SIG="\"application/x-vnd.bls-micro_restart\""
SAMPLESIG="\"application/x-vnd.bls-rdsampleapp\""

APP1VERSION="0.10"
APP2VERSION="0.10"

BUILDDATE=$(shell date)
BUILDDATE2=$(shell date +%s)

APP1SRC= \
	source/rdmain.cpp \
	source/rdapp.cpp \
	source/rddbicon.cpp
APP2SRC= \
	source/mrmain.cpp \
	source/mrapp.cpp
SAMPLESRC= \
	source/sampleappmain.cpp \
	source/sampleapp.cpp
	
APP1RSRC=source/rd.rsrc
APP2RSRC=source/mr.rsrc
SAMPLERSRC=source/sample.rsrc

COMPILE_FLAGS= -O2

ifneq ($(MAKECMDGOALS),clean)
ifeq "$(shell if [ -d $(DEPEND_DIR) ] ; then echo 1 ; else mkdir -p $(DEPEND_DIR) $(OBJECT_DIR) $(OUTPUT_DIR) ; echo 0 ; fi)" "0"
endif
endif

all: app1 app2 sampleapp
	@echo Everything has been built.

app1: $(OUTPUT_DIR)/$(APP1NAME)
	@echo Done building $(APP1NAME)

app2: $(OUTPUT_DIR)/$(APP2NAME)
	@echo Done building $(APP2NAME)

sampleapp: $(OUTPUT_DIR)/$(SAMPLEAPPNAME)
	@echo Done building $(SAMPLEAPPNAME)

clean:
	-@rm -rf $(OUTPUT_DIR) $(DEPEND_DIR) $(OBJECT_DIR) && echo Done cleaning.

$(OUTPUT_DIR)/$(APP1NAME): $(foreach file,$(notdir $(APP1SRC:.cpp=.o)),$(OBJECT_DIR)/$(file)) $(APP1RSRC)
	@echo Building $(APP1NAME) 
	@$(CC) -o$@ $(COMPILE_FLAGS) $(foreach file,$(notdir $(APP1SRC:.cpp=.o)),$(OBJECT_DIR)/$(file)) -lbe -lroot -lstdc++.r4
	@strip $@
	@xres -o $(OUTPUT_DIR)/$(APP1NAME) $(APP1RSRC)
	@mimeset -F $(OUTPUT_DIR)/$(APP1NAME)
	@echo "$(APP1NAME) has been built, linked, and resourced."
	
$(OUTPUT_DIR)/$(APP2NAME): $(foreach file,$(notdir $(APP2SRC:.cpp=.o)),$(OBJECT_DIR)/$(file)) $(APP2RSRC)
	@echo Building $(APP2NAME)
	@$(CC) -o$@ $(COMPILE_FLAGS) $(foreach file,$(notdir $(APP2SRC:.cpp=.o)),$(OBJECT_DIR)/$(file)) -lbe -lroot -lstdc++.r4
	@strip $@
	@xres -o $(OUTPUT_DIR)/$(APP2NAME) $(APP2RSRC)
	@mimeset -F $(OUTPUT_DIR)/$(APP2NAME)
	@echo "$(APP2NAME) has been built, linked, and resourced."

$(OUTPUT_DIR)/$(SAMPLEAPPNAME): $(foreach file,$(notdir $(SAMPLESRC:.cpp=.o)),$(OBJECT_DIR)/$(file)) $(SAMPLERSRC)
	@echo Building $@
	@$(CC) -o$(OUTPUT_DIR)/$(SAMPLEAPPNAME) $(COMPILE_FLAGS) $(foreach file,$(notdir $(SAMPLESRC:.cpp=.o)),$(OBJECT_DIR)/$(file)) -lbe -lroot -lstdc++.r4
	@strip $@
	@xres -o $(OUTPUT_DIR)/$(SAMPLEAPPNAME) $(SAMPLERSRC)
	@mimeset -F $(OUTPUT_DIR)/$(SAMPLEAPPNAME)
	@echo "$(SAMPLEAPPNAME) has been built, linked, and resourced."
	
$(OBJECT_DIR)/%.o: source/%.cpp $(DEPEND_DIR)/%.d
	@echo Updating object file $@
	@$(CC) $(CFLAGS) $(COMPILE_FLAGS) -DAPP1SIG=$(APP1SIG) -DAPP2SIG=$(APP2SIG) -DSAMPLESIG=$(SAMPLESIG) -DAPP1VERSION -DAPP2VERSION -DBUILDDATE -c $< -o $@

$(DEPEND_DIR)/%.d: source/%.cpp
	@echo Updating dependency file $@
	@set -e; $(CC) -M $(CFLAGS) $(COMPILE_FLAGS) -DAPP1SIG=$(APP1SIG) -DAPP2SIG=$(APP2SIG) -DAPP1VERSION -DAPP2VERSION -DBUILDDATE $< | sed 's/\($*\)\.o[ :]*/$(OBJECT_DIR)\/\1.o $(DEPEND_DIR)\/\1.d : /g' > $@;[ -s $@ ] || rm -f $@

ifneq ($(MAKECMDGOALS),clean)
-include $(foreach file,$(APP1SRC:.cpp=.d),$(DEPEND_DIR)/$(notdir $(file))) $(foreach file,$(APP2SRC:.cpp=.d),$(DEPEND_DIR)/$(notdir $(file))) $(foreach file,$(SAMPLESRC:.cpp=.d),$(DEPEND_DIR)/$(notdir $(file)))
endif