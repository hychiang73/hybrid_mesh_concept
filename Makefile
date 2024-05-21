K_DIR=/home/parallels/Workplace/nvidia/kernel_out/build
#K_DIR=/home/parallels/workspace/jetson-dev/kernel_out/build
ccflags-y += -Wall

obj-m += mac6021/

obj-m += test/ 

obj-$(CONFIG_BRIDGE)        += bridge/

all:
	$(MAKE) -C $(K_DIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(K_DIR) M=$(PWD) clean
