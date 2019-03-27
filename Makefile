
# cd ..
# git clone https://github.com/particle-iot/device-os.git
# cd device-os
# git co mesh-develop
# git submodule update --init
# export GCC_ARM_PATH=~/.particle/toolchains/gcc-arm/5.3.1/bin/
# cd modules
# nice nice make all PLATFORM=xenon

# GCC was installed by vscode extension:
GCC_ARM_PATH ?= $(HOME)/.particle/toolchains/gcc-arm/5.3.1/bin/
export GCC_ARM_PATH

PLATFORM ?= xenon
APPDIR = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
FIRMWARE_PATH ?= $(APPDIR)/../device-os/main/

all:
	$(MAKE) -C $(FIRMWARE_PATH) all PLATFORM=$(PLATFORM) APPDIR=$(APPDIR)
