# setup
COMPILER = arm-none-eabi
PROJ_NAME = stm32

OBJ_ROOT_DIR = obj
STM32LIB_DIR = Libraries/STM32F10x_StdPeriph_Driver
CM3CORE_DIR = Libraries/CMSIS/CM3/CoreSupport
DEVICE_DIR = Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x
STARTUP_DIR = $(DEVICE_DIR)/startup/TrueSTUDIO


LD_FILE = ld/STM32F103CB.ld
PROJ_ELF = $(OBJ_ROOT_DIR)/$(PROJ_NAME).elf
PROJ_BIN = $(OBJ_ROOT_DIR)/$(PROJ_NAME).bin
PROJ_MAP = $(OBJ_ROOT_DIR)/$(PROJ_NAME).map
PROJ_ASM = $(OBJ_ROOT_DIR)/$(PROJ_NAME).asm

DEFFLAG = -DDEBUG -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER
COMPILE_OPTS = -fdiagnostics-color=auto -mcpu=cortex-m3 -mthumb -Wall -g -O3 $(DEFFLAG)
INCLUDE_DIRS = -I . -I $(STM32LIB_DIR)/inc -I include -I $(CM3CORE_DIR) -I $(DEVICE_DIR)
LIBRARY_DIRS = -L lib

CC = $(COMPILER)-gcc
CFLAGS = $(COMPILE_OPTS) $(INCLUDE_DIRS)

CXX = $(COMPILER)-g++
CXXFLAGS = $(COMPILE_OPTS) $(INCLUDE_DIRS)

AS = $(COMPILER)-gcc
ASFLAGS = $(COMPILE_OPTS) -c

LD = $(COMPILER)-gcc
LDFLAGS = -Wl,--gc-sections,-Map=$(PROJ_MAP),-cref -n -mcpu=cortex-m3 -mthumb -T $(LD_FILE)

OBJCP = $(COMPILER)-objcopy
OBJCPFLAGS = -O binary

AR = $(COMPILER)-ar
ARFLAGS = cr

DUMP = $(COMPILER)-objdump
DUMPFLAGS = -D


# libstm32.a
STM32_LIB = $(OBJ_ROOT_DIR)/libstm32.a

STM32LIB_SRCS = \
    $(STM32LIB_DIR)/src/misc.c             \
    $(STM32LIB_DIR)/src/stm32f10x_adc.c    \
    $(STM32LIB_DIR)/src/stm32f10x_bkp.c    \
    $(STM32LIB_DIR)/src/stm32f10x_can.c    \
    $(STM32LIB_DIR)/src/stm32f10x_cec.c    \
    $(STM32LIB_DIR)/src/stm32f10x_crc.c    \
    $(STM32LIB_DIR)/src/stm32f10x_dac.c    \
    $(STM32LIB_DIR)/src/stm32f10x_dbgmcu.c \
    $(STM32LIB_DIR)/src/stm32f10x_dma.c    \
    $(STM32LIB_DIR)/src/stm32f10x_exti.c   \
    $(STM32LIB_DIR)/src/stm32f10x_flash.c  \
    $(STM32LIB_DIR)/src/stm32f10x_fsmc.c   \
    $(STM32LIB_DIR)/src/stm32f10x_gpio.c   \
    $(STM32LIB_DIR)/src/stm32f10x_i2c.c    \
    $(STM32LIB_DIR)/src/stm32f10x_iwdg.c   \
    $(STM32LIB_DIR)/src/stm32f10x_pwr.c    \
    $(STM32LIB_DIR)/src/stm32f10x_rcc.c    \
    $(STM32LIB_DIR)/src/stm32f10x_rtc.c    \
    $(STM32LIB_DIR)/src/stm32f10x_sdio.c   \
    $(STM32LIB_DIR)/src/stm32f10x_spi.c    \
    $(STM32LIB_DIR)/src/stm32f10x_tim.c    \
    $(STM32LIB_DIR)/src/stm32f10x_usart.c  \
    $(STM32LIB_DIR)/src/stm32f10x_wwdg.c   \

USER_SRCS = \
    $(CM3CORE_DIR)/core_cm3.c             \
    $(DEVICE_DIR)/system_stm32f10x.c      \
    $(STARTUP_DIR)/startup_stm32f10x_md.s \
    main/main.c \
#    main/stm32f10x_it.c


STM32LIB_OBJS = $(addprefix $(OBJ_ROOT_DIR)/,$(subst .c,.o,$(subst .s,.o,$(STM32LIB_SRCS))))
USER_OBJ = $(addprefix $(OBJ_ROOT_DIR)/,$(subst .c,.o,$(subst .s,.o,$(USER_SRCS))))
ALL_OBJS = $(STM32LIB_OBJS) $(USER_OBJ)

OBJ_DIR = $(sort $(dir $(ALL_OBJS)))

INFO = "info"
COMPILE_TIME = `date`

# all
all: $(INFO) $(OBJ_DIR) $(ALL_OBJS) $(STM32_LIB) $(PROJ_ELF) $(PROJ_BIN) $(PROJ_ASM) $(COMPILE_TIME)



$(INFO):
	@echo ""
	@echo "  COMPILE_OPTS:	" $(COMPILE_OPTS)
	@echo "  LDFLAGS:	" $(LDFLAGS)
	@echo ""
	

$(OBJ_DIR):
	@mkdir -p $@

$(OBJ_ROOT_DIR)/%.o: %.c
	@echo "  CC	" $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_ROOT_DIR)/%.o: %.s
	@echo "  CC	" $<
	@$(CXX) $(ASFLAGS) $< -o $@


$(STM32_LIB): $(STM32LIB_OBJS)
	@echo "  AR	" $@
	@$(AR) $(ARFLAGS) $@ $^

$(PROJ_ELF): $(USER_OBJ) $(STM32_LIB)
	@echo "  LD	" $@
	@$(LD) $(LDFLAGS) $^ --output $@

$(PROJ_BIN): $(PROJ_ELF)
	@echo "  OBJCP	" $@
	@$(OBJCP) $(OBJCPFLAGS) $^ $@

$(PROJ_ASM): $(PROJ_ELF)
	@echo "  DUMP	" $@
	@$(DUMP) $(DUMPFLAGS) $< > $@

$(COMPILE_TIME):
	@echo ""
	@echo "  TIME:	" $@

# flash
flash: $(PROJ_NAME)
	@cp $(PROJ_ELF) jtag/flash
	@cd jtag; openocd -f flash.cfg
	@rm jtag/flash

clean:
	@-rm $(ALL_OBJS) $(STM32_LIB) $(PROJ_ELF) $(PROJ_BIN) $(PROJ_MAP) $(PROJ_ASM)
	-rm -rf $(OBJ_ROOT_DIR)
