PROGNAME = Tetris

SRC_DIR = ./src
OBJ_DIR = ./build

VPATH = $(SRC_DIR)

# Source files
SRC = $(shell find $(SRC_DIR) -name "*.c" -or -name "*.s")
# Resulting object files
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

.PHONY: all
all: CFLAGS += $(CRELEASE) -I../shared
all: LDFLAGS += $(LDRELEASE)
all: $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

include ${LINKSCRIPT_DIR}/GBAVariables.mak

CFLAGS += -Wstrict-prototypes -Wold-style-definition

debug: CFLAGS += $(CDEBUG) -I../shared
debug: LDFLAGS += $(LDDEBUG)
debug: $(PROGNAME).gba
	@echo "[FINISH] Created $(PROGNAME).gba"

$(PROGNAME).gba: $(PROGNAME).elf
	@echo "[LINK] Linking objects together to create $(PROGNAME).gba"
	@$(OBJCOPY) -O binary $(PROGNAME).elf $(PROGNAME).gba

$(PROGNAME).elf: crt0.o $(GCCLIB)/crtbegin.o $(GCCLIB)/crtend.o $(GCCLIB)/crti.o $(GCCLIB)/crtn.o $(OBJ) libc_sbrk.o
	$(CC) -o $(PROGNAME).elf $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: med
med: CFLAGS += $(CRELEASE) -I../shared
med: LDFLAGS += $(LDRELEASE)
med: $(PROGNAME).gba
	@echo "[EXECUTE] Running emulator Mednafen"
	@echo "          Please see emulator.log if this fails"
# Default mednafen keybinds are "Gaaarbage", in the words of Farzam
	@mkdir -p ~/.mednafen/
	@cp mednafen-09x.cfg ~/.mednafen/
	@mednafen $(MEDOPT) $(PROGNAME).gba >emulator.log 2>&1

.PHONY: clean
clean:
	@echo "[CLEAN] Removing all compiled files"
	rm -rf $(OBJ_DIR) *.o *.elf *.gba *.log
