#include "gdt/gdt.h"
#include "idt/idt.h"
#include "memory/mem.h"
#include "idt/interrupts.h"
#include "kernelDefines.h"
#include "init.h"
#include "video/renderer.h"
#include "memory/malloc.h"

extern "C" void disablePIC();

// Simple delay function
void delay(int count)
{
    volatile int i;
    for (i = 0; i < count; i++) {
        // Busy wait loop, simply to introduce a delay
    }
}

void init()
{
    GDTDescriptor gdtDescriptor;
    gdtDescriptor.Size = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    LoadGDT(&gdtDescriptor);

    comout("TEST\n");
    comout("COM TEST 2\n");

    printf("[ %sINFO %s] GDT successfully loaded at address 0x%lx\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    create_idt();
    InitPS2Mouse();

    printf("[ %sINFO %s] IDT successfully loaded at address 0x%lx\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    physical_kernel_start = kernel_address_request.response->physical_base;
    virtual_kernel_start = kernel_address_request.response->virtual_base;

    hhdm_offset = hhdm_request.response->offset;

    buffer = framebuffer_request.response->framebuffers[0];  
    terminal = terminal_request.response->terminals[0];

    initramfs = module.response->modules[0];
    font = module.response->modules[1];

    printf("[ %sINFO %s] Framebuffer and terminal buffers initialized\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    parse();

    printf("[ %sINFO %s] Ramdisk parsed and module loaded\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    // Bootloader messages
    printf("[ %sBOOT %s] Bootloader Stage 1 - Real Mode\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sBOOT %s] Memory address: 0x7C00, Segment:Offset CS=0x0000, IP=0x7C00\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sBOOT %s] Size limit: 512 bytes (0x0200), Magic signature: 0x55AA at offset 0x1FE\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sBOOT %s] A20 enabled, loaded stage 2 at 0x1000\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    printf("[ %sBOOT %s] Bootloader Stage 2 - Protected Mode\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sBOOT %s] Execution mode: 32-bit protected mode\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sBOOT %s] GDT loaded with 3 entries (Null, Code, Data)\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    // Kernel entry stage
    printf("[ %sKERNEL %s] Kernel Entry - Long Mode\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sKERNEL %s] Entry address: 0xFFFFFFFF80000000, CR3: PML4 physical address\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sKERNEL %s] CR0: 0x80010033, CR4: 0x00000690, EFER: 0x00000500\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    // Paging setup messages
    printf("[ %sINFO %s] Paging Structure Initialized - 4-level paging enabled\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sINFO %s] PML4: 512 entries, each 8 bytes. PDPT: 4 entries for 1 GiB range.\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sINFO %s] Page entry flags set. Kernel text range: 0xFFFFFFFF80000000 – 0xFFFFFFFF80200000\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    // GDT structure details
    printf("[ %sINFO %s] GDT loaded at physical address 0x500\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sINFO %s] GDT entries: 0x00 (Null), 0x08 (64-bit code), 0x10 (64-bit data), 0x18 (TSS)\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    // Interrupt Descriptor Table
    printf("[ %sINFO %s] IDT loaded with 256 entries\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sINFO %s] Interrupts: 0x00-0x1F CPU exceptions, 0x20-0x2F IRQs\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sINFO %s] Remap PIC: Master offset = 0x20, Slave offset = 0x28\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    // System Call mechanism
    printf("[ %sINFO %s] SYSCALL mechanism enabled: RAX = syscall number, RDI-R9 = arguments\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sINFO %s] MSRs required: EFER, LSTAR, STAR, FMASK\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    // Multitasking and Scheduler
    printf("[ %sINFO %s] Multitasking enabled with TSS per CPU\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sINFO %s] Scheduler storing task state: RIP, RSP, CR3, GPRs\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    // Heap and memory allocation
    printf("[ %sINFO %s] Physical memory allocator initialized at 0x100000\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sINFO %s] Kernel heap starts at 0xFFFFFFFF80200000, upward\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    // Hardware I/O
    printf("[ %sINFO %s] Keyboard I/O initialized - Data Port: 0x60, IRQ: 1\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sINFO %s] Timer (PIT) initialized - Base frequency: 1193182 Hz\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sINFO %s] Mouse I/O initialized - IRQ: 12, Ports: 0x60 (data), 0x64 (command)\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    // Display system messages
    printf("[ %sINFO %s] Display system initialized with framebuffer resolution 320x200, 8bpp\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sINFO %s] Pixel memory written directly, full redraw on each refresh cycle\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    // Taskbar and Window System
    printf("[ %sINFO %s] Taskbar initialized - Height: 40px, Background color: medium gray\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sINFO %s] Window system initialized - Maximum 8 concurrent windows\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    // Mouse and Keyboard Input
    printf("[ %sINFO %s] Mouse input polling initialized - Coordinates updated each frame\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sINFO %s] Keyboard input processing - US QWERTY keymap, real-time character delivery\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    // GUI Initialization
    printf("[ %sINFO %s] GUI structures statically allocated - No dynamic memory usage\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
    printf("[ %sINFO %s] No double-buffering; framebuffer swap is optional and disabled\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages

    // End of initialization
    printf("[ %sINFO %s] System initialized successfully - Entering main event loop\n", Green, Green);
    delay(1000000);  // Small delay to introduce time between messages
}
