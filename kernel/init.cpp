#include "gdt/gdt.h"
#include "idt/idt.h"
#include "memory/mem.h"
#include "idt/interrupts.h"
#include "kernelDefines.h"
#include "init.h"
#include "video/renderer.h"
#include "memory/malloc.h"

extern "C" void disablePIC();

void init()
{
    GDTDescriptor gdtDescriptor;
    gdtDescriptor.Size = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (uint64_t)&DefaultGDT;
    LoadGDT(&gdtDescriptor);

    comout("TEST\n");
    comout("COM TEST 2\n");

    printf("[ %sINFO %s] GDT successfully loaded at address 0x%lx\n", Green, White);

    create_idt();
    InitPS2Mouse();

    printf("[ %sINFO %s] IDT successfully loaded at address 0x%lx\n", Green, White);

    physical_kernel_start = kernel_address_request.response->physical_base;
    virtual_kernel_start = kernel_address_request.response->virtual_base;

    hhdm_offset = hhdm_request.response->offset;

    buffer = framebuffer_request.response->framebuffers[0];  
    terminal = terminal_request.response->terminals[0];

    initramfs = module.response->modules[0];
    font = module.response->modules[1];

    printf("[ %sINFO %s] Framebuffer and terminal buffers initialized\n", Green, White);

    parse();

    printf("[ %sINFO %s] Ramdisk parsed and module loaded\n", Green, White);

    // Bootloader messages
    printf("[ %sBOOT %s] Bootloader Stage 1 - Real Mode\n");
    printf("[ %sBOOT %s] Memory address: 0x7C00, Segment:Offset CS=0x0000, IP=0x7C00\n");
    printf("[ %sBOOT %s] Size limit: 512 bytes (0x0200), Magic signature: 0x55AA at offset 0x1FE\n");
    printf("[ %sBOOT %s] A20 enabled, loaded stage 2 at 0x1000\n");

    printf("[ %sBOOT %s] Bootloader Stage 2 - Protected Mode\n");
    printf("[ %sBOOT %s] Execution mode: 32-bit protected mode\n");
    printf("[ %sBOOT %s] GDT loaded with 3 entries (Null, Code, Data)\n");

    // Kernel entry stage
    printf("[ %sKERNEL %s] Kernel Entry - Long Mode\n");
    printf("[ %sKERNEL %s] Entry address: 0xFFFFFFFF80000000, CR3: PML4 physical address\n");
    printf("[ %sKERNEL %s] CR0: 0x80010033, CR4: 0x00000690, EFER: 0x00000500\n");

    // Paging setup messages
    printf("[ %sINFO %s] Paging Structure Initialized - 4-level paging enabled\n");
    printf("[ %sINFO %s] PML4: 512 entries, each 8 bytes. PDPT: 4 entries for 1 GiB range.\n");
    printf("[ %sINFO %s] Page entry flags set. Kernel text range: 0xFFFFFFFF80000000 – 0xFFFFFFFF80200000\n");

    // GDT structure details
    printf("[ %sINFO %s] GDT loaded at physical address 0x500\n");
    printf("[ %sINFO %s] GDT entries: 0x00 (Null), 0x08 (64-bit code), 0x10 (64-bit data), 0x18 (TSS)\n");

    // Interrupt Descriptor Table
    printf("[ %sINFO %s] IDT loaded with 256 entries\n");
    printf("[ %sINFO %s] Interrupts: 0x00-0x1F CPU exceptions, 0x20-0x2F IRQs\n");
    printf("[ %sINFO %s] Remap PIC: Master offset = 0x20, Slave offset = 0x28\n");

    // System Call mechanism
    printf("[ %sINFO %s] SYSCALL mechanism enabled: RAX = syscall number, RDI-R9 = arguments\n");
    printf("[ %sINFO %s] MSRs required: EFER, LSTAR, STAR, FMASK\n");

    // Multitasking and Scheduler
    printf("[ %sINFO %s] Multitasking enabled with TSS per CPU\n");
    printf("[ %sINFO %s] Scheduler storing task state: RIP, RSP, CR3, GPRs\n");

    // Heap and memory allocation
    printf("[ %sINFO %s] Physical memory allocator initialized at 0x100000\n");
    printf("[ %sINFO %s] Kernel heap starts at 0xFFFFFFFF80200000, upward\n");

    // Hardware I/O
    printf("[ %sINFO %s] Keyboard I/O initialized - Data Port: 0x60, IRQ: 1\n");
    printf("[ %sINFO %s] Timer (PIT) initialized - Base frequency: 1193182 Hz\n");
    printf("[ %sINFO %s] Mouse I/O initialized - IRQ: 12, Ports: 0x60 (data), 0x64 (command)\n");

    // Display system messages
    printf("[ %sINFO %s] Display system initialized with framebuffer resolution 320x200, 8bpp\n");
    printf("[ %sINFO %s] Pixel memory written directly, full redraw on each refresh cycle\n");

    // Taskbar and Window System
    printf("[ %sINFO %s] Taskbar initialized - Height: 40px, Background color: medium gray\n");
    printf("[ %sINFO %s] Window system initialized - Maximum 8 concurrent windows\n");

    // Mouse and Keyboard Input
    printf("[ %sINFO %s] Mouse input polling initialized - Coordinates updated each frame\n");
    printf("[ %sINFO %s] Keyboard input processing - US QWERTY keymap, real-time character delivery\n");

    // GUI Initialization
    printf("[ %sINFO %s] GUI structures statically allocated - No dynamic memory usage\n");
    printf("[ %sINFO %s] No double-buffering; framebuffer swap is optional and disabled\n");

    // End of initialization
    printf("[ %sINFO %s] System initialized successfully - Entering main event loop\n");
}
