PROGNAME = Tetris

SRC_DIR = src

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/**/*.c)
# Resulting object files
# TODO: dump into separate OBJ_DIR directory
OBJ = $(patsubst $(SRC_DIR)/%.c,$(SRC_DIR)/%.o,$(SRC))

.PHONY: all
all : CFLAGS += $(CRELEASE) -I../shared
all : LDFLAGS += $(LDRELEASE)
all : $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

# In case this is being built using Brandon's PPA
LINKSCRIPT_DIR ?= /opt/cs2110-tools
include $(LINKSCRIPT_DIR)/GBAVariables.mak

CFLAGS += -Wstrict-prototypes -Wold-style-definition

debug : CFLAGS += $(CDEBUG) -I../shared
debug : LDFLAGS += $(LDDEBUG)
debug : $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

$(PROGNAME).gba : $(PROGNAME).elf
	@echo "[LINK] Linking objects together to create $(PROGNAME).gba"
	@$(OBJCOPY) -O binary $(PROGNAME).elf $(PROGNAME).gba

$(PROGNAME).elf : crt0.o $(GCCLIB)/crtbegin.o $(GCCLIB)/crtend.o $(GCCLIB)/crti.o $(GCCLIB)/crtn.o $(OBJ) libc_sbrk.o
	$(CC) -o $(PROGNAME).elf $^ $(LDFLAGS)

$(OBJ_DIR)/%.o : $(SRC)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# These are unnecessary if building with nix
.PHONY : emu
emu : med

.PHONY : vba
vba : CFLAGS += $(CRELEASE) -I../shared
vba : LDFLAGS += $(LDRELEASE)
vba : $(PROGNAME).gba
	@echo "[EXECUTE] Running Emulator VBA-M"
	@echo "          Please see emulator.log if this fails"
	@vbam $(VBAOPT) $(PROGNAME).gba >emulator.log 2>&1

.PHONY : med
med : CFLAGS += $(CRELEASE) -I../shared
med : LDFLAGS += $(LDRELEASE)
med : $(PROGNAME).gba
	@echo "[EXECUTE] Running emulator Mednafen"
	@echo "          Please see emulator.log if this fails"
# Default mednafen keybinds are "Gaaarbage", in the words of Farzam
	@mkdir -p ~/.mednafen/
	@cp mednafen-09x.cfg ~/.mednafen/
	@mednafen $(MEDOPT) $(PROGNAME).gba >emulator.log 2>&1

.PHONY : clean
clean :
	@echo "[CLEAN] Removing all compiled files"
	rm -f *.o *.elf *.gba *.log */*.o
