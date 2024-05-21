# SPDX-License-Identifier: GPL-2.0
K_DIR=/home/parallels/workspace/jetson-dev/kernel_out/build

obj-m += mac6021.o

# mac6021 objects
mac6021-y := main.o mesh.o rx.o tx.o 

ccflags-y += -Wall

all:
	$(MAKE) -C $(K_DIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(K_DIR) M=$(PWD) clean

