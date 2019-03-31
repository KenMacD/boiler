
# cd ..
# git clone https://github.com/particle-iot/device-os.git
# cd device-os
# git co mesh-develop
# git submodule update --init
# export GCC_ARM_PATH=~/.particle/toolchains/gcc-arm/5.3.1/bin/
# cd modules
# nice nice make all PLATFORM=xenon

# obj files: ../device-os/build/target/user/...

# GCC was installed by vscode extension:
GCC_ARM_PATH ?= $(HOME)/.particle/toolchains/gcc-arm/5.3.1/bin/
export GCC_ARM_PATH

# MODULAR_FIRMWARE to make clean remove the correct directory
MODULAR_FIRMWARE ?= y
export MODULAR_FIRMWARE

PLATFORM ?= xenon
export PLATFORM

APPDIR = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
export APPDIR

FIRMWARE_PATH ?= $(APPDIR)/../device-os/

all:
	$(MAKE) -C $(FIRMWARE_PATH)/main all

clean:
	$(MAKE) -C $(FIRMWARE_PATH)/user clean
	-$(RM) -r target/
