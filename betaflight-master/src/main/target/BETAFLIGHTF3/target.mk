
F3_TARGETS   += $(TARGET)
FEATURES     = VCP SDCARD
TARGET_FLAGS = -DSPRACINGF3

TARGET_SRC = \
            drivers/accgyro_mpu.c \
            drivers/accgyro_spi_mpu6000.c \
            drivers/compass_ak8975.c \
            drivers/compass_hmc5883l.c \
            drivers/display_ug2864hsweg01.h \
            drivers/flash_m25p16.c \
            drivers/light_ws2811strip.c \
            drivers/light_ws2811strip_stm32f30x.c \
            drivers/serial_softserial.c \
            drivers/sonar_hcsr04.c

