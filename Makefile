# This is the name that our final kernel executable will have.
# Change as needed.
override KERNEL := kernel.elf
 
# Convenience macro to reliably declare overridable command variables.
define DEFAULT_VAR =
	ifeq ($(origin $1),default)
		override $(1) := $(2)
	endif
	ifeq ($(origin $1),undefined)
		override $(1) := $(2)
	endif
endef
 
# It is highly recommended to use a custom built cross toolchain to build a kernel.
# We are only using "cc" as a placeholder here. It may work by using
# the host system's toolchain, but this is not guaranteed.
$(eval $(call DEFAULT_VAR,CC,cc))
 
# Same thing for "ld" (the linker).
$(eval $(call DEFAULT_VAR,LD,ld))
 
# User controllable CFLAGS.
CFLAGS ?= -g -Wall -Wextra -Wpedantic -pipe
 
# User controllable preprocessor flags. We set none by default.
CPPFLAGS ?=
 
# User controllable nasm flags.
NASMFLAGS ?= -F dwarf -g
 
# User controllable linker flags. We set none by default.
LDFLAGS ?= 
 
# Internal C flags that should not be changed by the user.
override CFLAGS +=       \
	-I.                  \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-stack-check     \
	-fno-pie             \
	-fno-pic             \
	-fno-threadsafe-statics \
	-m64                 \
	-march=x86-64        \
	-mabi=sysv           \
	-mno-80387           \
	-mno-mmx             \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-mcmodel=kernel      \
	-Wno-unused-parameter \
	-Wno-c++20-extensions \
	-Wno-missing-field-initializers \
	-Wno-address \
	-Wno-int-to-pointer-cast \
	-Wno-pointer-arith \
	-Wno-write-strings \
	-Wno-cast-function-type
 
# Internal linker flags that should not be changed by the user.
override LDFLAGS +=         \
	-nostdlib               \
	-static                 \
	-z max-page-size=0x1000 \
	-T linker.ld
 
# Internal nasm flags that should not be changed by the user.
override NASMFLAGS += \
	-f elf64
 
# Use find to glob all *.c, *.S, and *.asm files in the directory and extract the object names.
override CPPFILES := $(shell find . -type f -name '*.cpp')
override ASFILES := $(shell find . -type f -name '*.S')
override NASMFILES := $(shell find . -type f -name '*.asm')
override OBJ := $(CPPFILES:.cpp=.o) $(ASFILES:.S=.o) $(NASMFILES:.asm=.o)
override HEADER_DEPS := $(CPPFILES:.cpp=.d) $(ASFILES:.S=.d)
 
# Default target.
.PHONY: all
all: dirs $(KERNEL)
 
dirs:
	@mkdir -p build
	@mkdir -p build/kernel/{fs,gdt,idt,memory,userinput,video,audio}
	@mkdir -p bin

# Link rules for the final kernel executable.
$(KERNEL): $(OBJ)
	@echo "   LD   $(subst ./,,$(subst kernel/,,$(OBJ))) ==> $@"
	@$(LD) $(subst ./,build/,$(OBJ)) $(LDFLAGS) -o bin/$@
 
# Include header dependencies.
-include $(HEADER_DEPS)
 
# Compilation rules for *.c files.
%.o: %.cpp
	@echo "   CXX   $@"
	@g++ $(CPPFLAGS) $(CFLAGS) -c $< -o build/$@
 
# Compilation rules for *.S files.
%.o: %.S
	@echo "   AS    $@"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o build/$@
 
# Compilation rules for *.asm (nasm) files.
%.o: %.asm
	@echo "   NASM  $@"
	@nasm $(NASMFLAGS) $< -o build/$@
 
# Remove object files and the final executable.
.PHONY: clean
clean:
	@rm -rf $(KERNEL) $(OBJ) $(HEADER_DEPS) limine initramfs iso_root/ bin/*
	@echo "Cleaned!"

initramfs:
	@tar -cf initramfs --format=ustar -C rootfs/ .

limine.h:
	@wget https://raw.githubusercontent.com/limine-bootloader/limine/trunk/limine.h -q

iso:
	rm -rf limine/
	@make limine.h
	@make
	@make initramfs
	@git clone https://github.com/limine-bootloader/limine.git --branch=v4.x-branch-binary --depth=1 --quiet
	
	@mkdir -p iso_root
 
	@cp bin/kernel.elf initramfs font limine.cfg limine/limine.sys \
	  limine/limine-cd.bin limine/limine-cd-efi.bin iso_root/
 
	@./tools/iso.sh

run: iso
	@qemu-system-x86_64 -cdrom bin/image.iso -bios ./OVMF_CODE.fd -debugcon stdio -m 1G -smp 3 -display sdl

debug: iso
	@qemu-system-x86_64 -cdrom bin/image.iso -bios ./OVMF_CODE.fd -debugcon stdio -m 1G -d int -D log.txt -no-reboot -no-shutdown -display sdl
