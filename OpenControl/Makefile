# setup
COMPILER = arm-none-eabi
PROJ_NAME = stm32

OBJ_ROOT_DIR = obj
STM32LIB_DIR = lib/STM32F10x_StdPeriph_Driver
CM3CORE_DIR  = lib/CoreSupport
DEVICE_DIR   = startup/STM32F10x
STARTUP_DIR  = $(DEVICE_DIR)
RTX_SRC_DIR  = lib/rtx/SRC
RTX_INC_DIR  = lib/rtx/INC $(RTX_DIR_SRC)


LD_FILE  = ld/STM32F103CB.ld
RTXLIB   = $(OBJ_ROOT_DIR)/librtx.a
STM32LIB = $(OBJ_ROOT_DIR)/libstm32.a
PROJ_ELF = $(OBJ_ROOT_DIR)/$(PROJ_NAME).elf
PROJ_BIN = $(OBJ_ROOT_DIR)/$(PROJ_NAME).bin
PROJ_MAP = $(OBJ_ROOT_DIR)/$(PROJ_NAME).map
PROJ_ASM = $(OBJ_ROOT_DIR)/$(PROJ_NAME).asm

DEFFLAG = -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -D__CORTEX_M3 -D__CMSIS_RTOS
#DEFFLAG = -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -D__MICROLIB
COMPILE_OPTS = -fdiagnostics-color=auto -mcpu=cortex-m3 -mthumb -Wall -g -O3  -ffunction-sections $(DEFFLAG)
INCLUDE_DIRS = -I . -I $(STM32LIB_DIR)/inc -I include -I $(CM3CORE_DIR) -I $(DEVICE_DIR) -I $(RTX_INC_DIR)
#INCLUDE_DIRS = -I . -I $(STM32LIB_DIR)/inc -I include -I $(CM3CORE_DIR) -I $(DEVICE_DIR)
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

SIZE = $(COMPILER)-size

RTXLIB_SRCS = $(wildcard $(RTX_SRC_DIR)/*.c)  $(wildcard $(RTX_SRC_DIR)/*.s)

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
    $(DEVICE_DIR)/system_stm32f10x.c      \
    $(STARTUP_DIR)/startup_stm32f10x_md.s \
    $(CM3CORE_DIR)/RTX_Conf_CM.c                     \
    main/main.c                           \
#    main/stm32f10x_it.c
#    $(CM3CORE_DIR)/core_cm3.c             


RTXLIB_OBJS   = $(addprefix $(OBJ_ROOT_DIR)/,$(subst .c,.o,$(subst .s,.o,$(RTXLIB_SRCS))))
STM32LIB_OBJS = $(addprefix $(OBJ_ROOT_DIR)/,$(subst .c,.o,$(subst .s,.o,$(STM32LIB_SRCS))))
USER_OBJ = $(addprefix $(OBJ_ROOT_DIR)/,$(subst .c,.o,$(subst .s,.o,$(USER_SRCS))))
ALL_OBJS = $(RTXLIB_OBJS) $(STM32LIB_OBJS) $(USER_OBJ)

OBJ_DIR = $(sort $(dir $(ALL_OBJS)))

INFO = "info"
COMPILE_TIME = `date`

# all
#all: $(INFO) $(OBJ_DIR) $(ALL_OBJS) $(STM32LIB) $(PROJ_ELF) $(PROJ_BIN) $(PROJ_ASM) $(COMPILE_TIME)
all: $(INFO) $(OBJ_DIR) $(ALL_OBJS) $(STM32LIB) $(RTXLIB) $(PROJ_ELF) $(PROJ_BIN) $(PROJ_ASM) $(COMPILE_TIME)



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


$(RTXLIB): $(RTXLIB_OBJS)
	@echo "  AR	" $@
	@$(AR) $(ARFLAGS) $@ $^

$(STM32LIB): $(STM32LIB_OBJS)
	@echo "  AR	" $@
	@$(AR) $(ARFLAGS) $@ $^

#$(PROJ_ELF): $(USER_OBJ) $(STM32LIB)
$(PROJ_ELF): $(USER_OBJ) $(STM32LIB) $(RTXLIB)
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
	@$(SIZE) -B $(PROJ_ELF)
	@echo ""
	@echo "  TIME:	" $@

clean:
#	@-rm $(ALL_OBJS) $(STM32_LIB) $(RTXLIB) $(PROJ_ELF) $(PROJ_BIN) $(PROJ_MAP) $(PROJ_ASM)
#	@-rm $(ALL_OBJS) $(STM32LIB) $(PROJ_ELF) $(PROJ_BIN) $(PROJ_MAP) $(PROJ_ASM)
	-rm -rf $(OBJ_ROOT_DIR)
