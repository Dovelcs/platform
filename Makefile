# 输出文件名
target = imux6ull
# 编译器
CROSSCOMPILE= arm-linux-gnueabihf-gcc
CURRENT_DIR := $(CURDIR)

# 这里是nfs的路径，如果没有nfs，可以不填
INSTALL_PATH = /home/qiu/nfs/rootfs/flash

#在这里添加头文件路径
INCLUDEPATH += -I$(CURRENT_DIR)/platform
INCLUDEPATH += -I$(CURRENT_DIR)/arch
INCLUDEPATH += -I$(CURRENT_DIR)/device

#在这里添加源文件路径
SRCPATH = $(wildcard $(CURRENT_DIR)/*.c)
SRCPATH += $(wildcard $(CURRENT_DIR)/platform/*.c)
SRCPATH += $(wildcard $(CURRENT_DIR)/arch/*.c)
SRCPATH += $(wildcard $(CURRENT_DIR)/device/*.c)

OBJ = $(patsubst %.c, %.o, $(SRCPATH))

CFLAGS = -Wall -std=gnu99 -fcommon -static

LDFLAGS = -Wl,-rpath,/mnt/lib
# SYSROOT = --sysroot=/usr/local/arm/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf/arm-linux-gnueabihf/libc
#arm-linux-gnueabihf-gcc -Wall Appfunc.c -o app -L/home/qiu/linux/functest/ -ltest -Wl,-rpath,.
LIB_PATH = -L$(CURRENT_DIR)/lib
LDLIBS = -lreadline -lncurses -lcares -lcrypt -lrt -pthread

.PHONY: all
all: force $(target)

force:

%.o: %.c
	$(CROSSCOMPILE) $(CFLAGS) $(LDFLAGS) $(INCLUDEPATH) -g -c $< -o $@

$(target):$(OBJ)
	$(CROSSCOMPILE) $(CFLAGS) -g $^ -o $@  $(LIB_PATH) $(LDLIBS) $(SYSROOT)

.PHONY: clean
clean:
	rm -f $(OBJ) $(target)
	

.PHONY: install

install:
	@if [ ! -d $(INSTALL_PATH) ]; then \
		echo "can't find $(INSTALL_PATH)"; \
	else \
		cp $(target) $(INSTALL_PATH); \
		echo "install $(target) to $(INSTALL_PATH)"; \
	fi