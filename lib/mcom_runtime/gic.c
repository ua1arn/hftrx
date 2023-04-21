// ----------------------------------------------------------
// PL390 - Generic Interrupt Controller
//
// initial code had been provided by ARM Support team
// adopted by A. Nikolaev
//
//
// GIC exercise
// ----------------------------------------------------------

#include "gic.h"
#include "mcom02.h"

handler_funct_ptr func_handlers[INTERRUPTS_NUM][CORE_CNT];
handler_funct_ptr exception_handlers[SIZE][CORE_CNT];

__attribute__((used)) const unsigned int ulICCIAR = 0x3900010c;
__attribute__((used)) const unsigned int ulICCEOIR = 0x39000110;
__attribute__((used)) const unsigned int ulICCPMR = 0x39000104;

volatile struct pl390_gic_dist_if *gic_dist;
volatile struct pl390_gic_cpu_if *gic_cpu;

// ------------------------------------------------------------
unsigned int get_core_number()
{
    unsigned int core_number;
    // get core number
    core_number = (unsigned int)&core_number;
    asm("MRC p15, 0, r8, c0, c0, 5");
    asm("MOV %0, r8" : "=r"(core_number));

    return core_number & 0x3;
}

// ------------------------------------------------------------
void initGICPointers(unsigned int dist, unsigned int cpu)
{
    gic_dist = (volatile struct pl390_gic_dist_if *)dist;
    gic_cpu = (volatile struct pl390_gic_cpu_if *)cpu;

    return;
}

// ------------------------------------------------------------
//  Global enable of the Interrupt Distributor
void enableGIC(void) { gic_dist->ICDDCR = 1; }

// Global disable of the Interrupt Distributor
void disableGIC(void) { gic_dist->ICDDCR = 0; }

// ------------------------------------------------------------
// Enables the interrupt source number ID
void enableIntID(unsigned int ID)
{
    unsigned int bank;

    bank = ID >> 5; // There are 32 IDs per register, need to work out which
                    // register to access
    ID = ID & 0x1f; // ... and which bit within the register

    ID = 1 << ID; // Move a '1' into the correct bit position

    gic_dist->ICDISER[bank] = ID;
}

// Disables the interrupt source number ID

void disableIntID(unsigned int ID)
{
    unsigned int bank;

    bank = ID >> 5; // There are 32 IDs per register, need to work out which
                    // register to access
    ID = ID & 0x1f; // ... and which bit within the register

    ID = 1 << ID; // Move a '1' into the correct bit position

    gic_dist->ICDICER[bank] = ID;
}

// ------------------------------------------------------------
// Sets the priority of the specified ID
void setIntPriority(unsigned int ID, unsigned int priority)
{
    unsigned int bank, tmp;

    priority = priority & 0xFF; // Priority field is 8-bits, mask off unused bit
    bank = ID / 4;              // There are 4 IDs per register, need to work out which
                                // register to access
    ID = ID & 0x3;              // ... and which field within the register
    ID = ID * 8; // Convert from which field to a bit offset (8-bits per field)

    priority = priority << ID; // Move prioity value into correct bit position

    tmp = gic_dist->ICDIPR[bank]; // Read the current value in the register
    tmp =
        tmp &
        ~(0xFF << ID); // Blank out the field holding the value we're modifying
    tmp = tmp | priority; // OR in the new priority
    gic_dist->ICDIPR[bank] = tmp;
}

// Returns the priority of the specified ID
unsigned int getIntPriority(unsigned int ID)
{
    unsigned int bank, tmp;

    bank = ID / 4; // There are 4 IDs per register, need to work out which
                   // register to access
    ID = ID & 0x3; // ... and which field within the register
    ID = ID * 8; // Convert from which field to a bit offset (8-bits per field)

    tmp = gic_dist->ICDIPR[bank];

    tmp = tmp >> ID;  // Shift desired field to bit position 0
    tmp = tmp & 0xFF; // Mask off the other bits

    return tmp;
}

// ------------------------------------------------------------
// Sets the target CPUs of the specified ID
// For 'target' use one of the above defines
void setIntTarget(unsigned int ID, unsigned int target)
{
    unsigned int bank, tmp;

    target = target & 0xFF; // Target field is 8-bits, mask off unused bit
    bank = ID >> 2;          // There are 4 IDs per register, need to work out which
                            // register to access
    ID = ID & 0x3;          // ... and which field within the register
    ID = ID * 8; // Convert from which field to a bit offset (8-bits per field)

    target = target << ID; // Move prioity value into correct bit position

    tmp = gic_dist->ICDIPTR[bank]; // Read the current value in the register
    tmp =
        tmp &
        ~(0xFF << ID);  // Blank out the field holding the value we're modifying
    tmp = tmp | target; // OR in the new target
    gic_dist->ICDIPTR[bank] = tmp;
}

// Returns the target CPUs of the specified ID
unsigned int getIntTarget(unsigned int ID)
{
    unsigned int bank, tmp;

    bank = ID >> 2; // There are 4 IDs per register, need to work out which
                   // register to access
    ID = ID & 0x3; // ... and which field within the register
    ID = ID * 8; // Convert from which field to a bit offset (8-bits per field)

    tmp = gic_dist->ICDIPTR[bank];

    tmp = tmp >> ID;  // Shift desired field to bit position 0
    tmp = tmp & 0xFF; // Mask off the other bits

    return tmp;
}

// ----------------------------------------------------------
// Configures the specified ID as being level or edge triggered
void configureID(unsigned int ID, unsigned int conf)
{
    unsigned int bank, tmp;

    conf = conf & 0x3; // Mask out unused bits

    bank = ID >> 4; // There are 16 IDs per register, need to work out which
                    // register to access / 16
    ID = ID & 0xF;  // ... and which field within the register
    ID = ID * 2; // Convert from which field to a bit offset (2-bits per field)

    conf = conf << ID; // Move prioity value into correct bit position

    tmp = gic_dist->ICDICFR[bank]; // Read current vlase
    tmp = tmp & ~(0x3 << ID);      // Clear the bits for the specified field
    tmp = tmp | conf;              // OR in new configuration
    gic_dist->ICDICFR[bank] = tmp; // Write updated value back
}

// ----------------------------------------------------------
// Sets the pending bit of the specified ID
void setIntPending(unsigned int ID)
{
    unsigned int bank;

    bank = ID >> 5; // There are 32 IDs per register, need to work out which
                    // register to access
    ID = ID & 0x1f; // ... and which bit within the register

    ID = 1 << ID; // Move a '1' into the correct bit position

    gic_dist->ICDISPR[bank] = ID;
}

// Clears the pending bit of the specified ID
void clearIntPending(unsigned int ID)
{
    unsigned int bank;

    bank = ID >> 5; // There are 32 IDs per register, need to work out which
                    // register to access
    ID = ID & 0x1f; // ... and which bit within the register

    ID = 1 << ID; // Move a '1' into the correct bit position

    gic_dist->ICDICPR[bank] = ID;
}

// Returns the value of the status bit of the specified ID
unsigned int getIntPending(unsigned int ID)
{
    unsigned int bank, tmp;

    bank = ID >> 5; // There are 32 IDs per register, need to work out which
                    // register to access
    ID = ID & 0x1f; // ... and which bit within the register

    tmp = gic_dist->ICDISPR[bank]; // Read the register containing the ID we are
                                   // interested in
    tmp = tmp >> ID; // Shift the status bit for specified ID to position 0
    tmp = tmp & 0x1; // Mask off the rest of the register

    return tmp;
}

// ------------------------------------------------------------
// Send a software generate interrupt
void sendSGI(unsigned int ID, unsigned int cpu_list, unsigned int filter_list,
             unsigned int SATT)
{
    // Ensure unused bits are clear, and shift into correct bit position
    ID = ID & 0xF;
    SATT = (SATT & 0x1) << 15;
    cpu_list = (cpu_list & 0xFF) << 16;
    filter_list = (filter_list & 0x3) << 24;

    // Combine fields
    ID = ID | SATT | cpu_list | filter_list;

    gic_dist->ICDSGIR = ID;
}

// ------------------------------------------------------------
// Sets the specified ID as secure
void makeIntSecure(unsigned int ID)
{
    unsigned int bank, tmp;

    bank = ID >> 5; // There are 32 IDs per register, need to work out which
                    // register to access
    ID = ID & 0x1f; // ... and which bit within the register

    ID = 1 << ID; // Move a '1' into the correct bit position
    ID = ~ID;     // Invert to get mask

    tmp = gic_dist->ICDISR[bank]; // Read current value
    tmp = tmp & ID;
    gic_dist->ICDISR[bank] = tmp;
}

// Set the specified ID as non-secure
void makeIntNonSecure(unsigned int ID)
{
    unsigned int bank, tmp;

    bank = ID >> 5; // There are 32 IDs per register, need to work out which
                    // register to access
    ID = ID & 0x1f; // ... and which bit within the register

    ID = 1 << ID; // Move a '1' into the correct bit position

    tmp = gic_dist->ICDISR[bank]; // Read current value
    tmp = tmp | ID;               // Or with bit mask to set the bit
    gic_dist->ICDISR[bank] = tmp; // Write-back
}

// Returns the security of the specified ID
unsigned int getIntSecurity(unsigned int ID)
{
    // TBD
    return 0;
}

// ------------------------------------------------------------
// CPU Interface functions
// ------------------------------------------------------------

// Enables the processor interface
// Must been done one each core seperately
void enableCPUInterface(void)
{
    unsigned int tmp;

    tmp = gic_cpu->ICCICR;
    tmp = tmp | 0x1; // Set bit 0
    gic_cpu->ICCICR = tmp;
}

// Disables the processor interface
void disableCPUInterface(void)
{
    unsigned int tmp;

    tmp = gic_cpu->ICCICR;
    tmp = tmp | 0xFFFFFFE; // Clear bit 0
    gic_cpu->ICCICR = tmp;
}

// Enables the sending of secure interrupts as FIQs
void enableSecureFIQs(void)
{
    unsigned int tmp;

    tmp = gic_cpu->ICCICR;
    tmp = tmp | 0x8; // Set bit 3
    gic_cpu->ICCICR = tmp;
}

// Disables the sending of secure interrupts as FIQs
void disableSecureFIQs(void)
{
    unsigned int tmp;

    tmp = gic_cpu->ICCICR;
    tmp = tmp | 0xFFFFFFF7; // Clear bit 3
    gic_cpu->ICCICR = tmp;
}

//  Returns the value of the Interrupt Acknowledge Register
unsigned int readIntAck(void) { return gic_cpu->ICCIAR; }

// Writes ID to the End Of Interrupt register
void writeEOI(unsigned int ID) { gic_cpu->ICCEOIR = ID; }

// Sets the Priority mask register for the core run on
// The reset value masks ALL interrupts!
void setPriorityMask(unsigned int priority)
{
    gic_cpu->ICCPMR = (priority & 0xFF);
}

// Sets the Binary Point Register for the core run on
void setBinaryPoint(unsigned int priority)
{
    gic_cpu->ICCBPR = (priority & 0xFF);
}

// ------------------------------------------------------------
// End of pl390_gic.c
// ------------------------------------------------------------

/* Global interrupt functions */
void enable_IRQ(void)
{
    __asm__("MRS r1, CPSR");
    __asm__("BIC r1, r1, #0x80");
    __asm__("MSR CPSR_c, r1");
}

void disable_IRQ(void)
{
    __asm__("MRS r1, CPSR");
    __asm__("ORR r1, r1, #0x80");
    __asm__("MSR CPSR_c, r1");
}

void enable_FIQ(void)
{
    __asm__("MRS r1, CPSR");
    __asm__("BIC r1, r1, #0x40");
    __asm__("MSR CPSR_c, r1");
}

void disable_FIQ(void)
{
    __asm__("MRS r1, CPSR");
    __asm__("ORR r1, r1, #0x40");
    __asm__("MSR CPSR_c, r1");
}

void C_int_handler(unsigned int ulICCIAR)
{

    unsigned int id, mark;
    handler_funct_ptr current_function;
    //
    //clearIntPending(ulICCIAR);// Не помогает
    //
    // calculate which CPU cause IRQ
    id = ulICCIAR & 0x3FF;
    mark = ulICCIAR - id;

    if (id < INTERRUPTS_NUM) 
    {
        if (mark == 0) 
        {
            current_function = func_handlers[id][0];
        } 
        else 
        {
            current_function = func_handlers[id][1];
        }
        //---------------------------------------------
        if (current_function) 
        {
            current_function((int)id);
        } 
        else 
        {
            // TODO: error message
        }
    }
}

void C_exception_handler(unsigned int exception_id)
{
    unsigned int core_number = get_core_number();

    // call handler
    exception_handlers[exception_id][core_number](exception_id);
}

void attach_irq_handler(unsigned int ID, handler_funct_ptr funct_pointer,
                        unsigned int core)
{

    func_handlers[ID][core] = funct_pointer;
}

void attach_exception_handler(enum EXCEPTIONS ID,
                              handler_funct_ptr funct_pointer,
                              unsigned int core)
{
    exception_handlers[ID][core] = funct_pointer;
}

void reset_irq_handlers(void)
{
    unsigned int core_number = get_core_number();

    if (core_number >= CORE_CNT)
        return;

    int i;
    for (i = 0; i < INTERRUPTS_NUM; i++) {
        func_handlers[i][core_number] = 0;
    }
}

void dummy_exception_handler(int id) { asm("WFI"); }

void dummy_prefetch_abort_handler(int id)
{
    /*asm("LDR r5, = 0x00800000");
    asm("STR r4, [r5]");
    asm("MRC p15, 0, r0, c5, c0, 1"); // IFSR
    asm("STR r0, [r5]");
    asm("MRC p15, 0, r0, c6, c0, 2"); // IFAR
    asm("STR r0, [r5]");*/
    asm("WFI");
}

void dummy_data_abort_handler(int id)
{
    /*asm("LDR r5, = 0x00800000");
    asm("STR r4, [r5]");
    asm("MRC p15, 0, r0, c5, c0, 0"); // DFSR
    asm("STR r0, [r5]");
    asm("MRC p15, 0, r0, c6, c0, 0"); // DFAR
    asm("STR r0, [r5]");*/
    asm("WFI");
}

void reset_exception_handlers(void)
{

    unsigned int core_number = get_core_number();
    if (core_number >= CORE_CNT)
        return;

    exception_handlers[UNDEFINED_INSTRUCTION][core_number] =
        dummy_exception_handler;
    exception_handlers[SOFTWARE_INTERRUPT][core_number] =
        dummy_exception_handler;
    exception_handlers[PREFETCH_ABORT][core_number] =
        dummy_prefetch_abort_handler;
    exception_handlers[DATA_ABORT][core_number] = dummy_data_abort_handler;
    exception_handlers[FIQ_PROCESSING][core_number] = dummy_exception_handler;
}

/****************************************************
 Group of procedures for old tests compatibility
 Do not use it in new tests!!!!
 ******************************************************/

void InterruptEnable(void)
{
    // Enable interrupt controller
    enable_IRQ();
}

void InterruptDisable(void) { disable_IRQ(); }

void open_mask(int ID)
{
    enableIntID((unsigned int)ID);
    setIntTarget((unsigned int)ID, 1);
    setIntPriority((unsigned int)ID, 0);
}

void openIM_mask(int ID)
{
    // without body
}

void close_mask(int ID) { disableIntID((unsigned int)ID); }

void closeIM_mask(int ID) {}

void prefetchAbortInterrupt(int adr) { dummy_prefetch_abort_handler(0); }
void dataAbortInterrupt(int adr) { dummy_data_abort_handler(0); }
void FIQInterrupt() {}
void vApplicationIRQHandler(unsigned int ulICCIAR) { C_int_handler(ulICCIAR); }

// *****************************************************
void risc_initialize_gic()
{
    initGICPointers(GIC_BASE, ICC_BASE);
    reset_irq_handlers();
    reset_exception_handlers();
    enableGIC();
    enableCPUInterface();
    setPriorityMask(0xFF);
    enable_IRQ();
}

void risc_enable_interrupt(INTERRUPT_TYPE ID, unsigned int target,
                           unsigned int priority)
{
    enableIntID(ID);
    setIntTarget(ID, target);
    setIntPriority(ID, priority);
}

void risc_disable_interrupt(INTERRUPT_TYPE ID)
{
    unsigned int bank;

    bank = ID >> 5; // There are 32 IDs per register, need to work out which
                    // register to access
    ID = ID & 0x1f; // ... and which bit within the register

    ID = 1 << ID; // Move a '1' into the correct bit position

    gic_dist->ICDICER[bank] = ID;
}

void risc_disable_interrupts() { gic_cpu->ICCPMR = 0; }

void risc_enable_interrupts() { gic_cpu->ICCPMR = 0xFF; }

enum ERL_ERROR risc_register_interrupt(handler_funct_ptr funct_pointer,
                                       INTERRUPT_TYPE ID, unsigned int core)
{
    if (funct_pointer != 0)
        func_handlers[ID][core] = funct_pointer;
    else
        return ERL_UNITIALIZED_ARG;
    return ERL_NO_ERROR;
}
