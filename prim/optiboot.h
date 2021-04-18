/*------------------------ Optiboot header file ----------------------------|
 |                                                                          |
 | June 2015 by Marek Wodzinski, https://github.com/majekw                  |
 | Modified June 2016 by MCUdude, https://github.com/MCUdude                |
 | Modified Sep 2020 for mega0/xTiny (optiboot_x)                           |
 |  by Bill Westfield https://github.com/westfw                             |
 | Modified Nov 2020 change the read() function to not change data          |
 | Released to public domain                                                |
 |                                                                          |
 | This header file gives possibility to use SPM instruction                |
 | from Optiboot bootloader memory.                                         |
 |                                                                          |
 | There are 5 convenient functions available here:                         |
 | * optiboot_page_erase - to erase a FLASH page                            |
 | * optiboot_page_fill - to put words into temporary buffer                |
 | * optiboot_page_write - to write contents of temporary buffer into FLASH |
 | * optiboot_readPage - higher level function to read a flash page and     |
 |                         store it in an array                             |
 | * optiboot_writePage - higher level function to write content to         |
 |                         a flash page                                     |
 |                                                                          |
 | For some hardcore users, you could use 'do_spm' as raw entry to          |
 | bootloader spm function.                                                 |
 |-------------------------------------------------------------------------*/


#ifndef _OPTIBOOT_H_
#define _OPTIBOOT_H_  1

#ifdef SPMCSR
// Mega0 does not have SPMCSR
#include <avr/boot.h>
#endif
#include "Arduino.h"

// figure out whether we have SPM or NVMCTRL
#ifdef FUSE_BOOTEND
#define USE_NVMCTRL 1
#define NVMCTRL_CMD_COPY_gc (NVMCTRL_CMD_gm+1)  // one beyond existing commands.
#define SPM_PAGESIZE (PROGMEM_PAGE_SIZE)
#endif

/*
 * Main 'magic' function - enter to bootloader do_spm function
 *
 * address - address to write (in bytes) but must be even number
 * command - one of __BOOT_PAGE_WRITE, __BOOT_PAGE_ERASE or __BOOT_PAGE_FILL
 * data - data to write in __BOOT_PAGE_FILL. In __BOOT_PAGE_ERASE or
 *          __BOOT_PAGE_WRITE it control if boot_rww_enable is run
 *         (0 = run, !0 = skip running boot_rww_enable)
 *
 */

// 'typedef' (in following line) and 'const' (few lines below)
//   are a way to define external function at some arbitrary address
typedef void (*do_spm_t)(uint16_t address, uint8_t command, uint16_t data);
typedef void (*do_nvmctrl_t)(uint16_t address, uint8_t command, uint8_t data);


/*
 * Devices with more than 64KB of flash:
 * - have larger bootloader area (1KB) (they are BIGBOOT targets)
 * - have RAMPZ register :-)
 * - need larger variable to hold address (pgmspace.h uses uint32_t)
 */
#ifdef RAMPZ
typedef uint32_t optiboot_addr_t;
#else
typedef uint16_t optiboot_addr_t;
#endif

#ifdef USE_NVMCTRL
// Mega0/xTiny/etc.   Bootloader is in low memory.
const do_spm_t do_nvmctrl = (do_spm_t)((PROGMEM_START + 2) >> 1);
#else
#if FLASHEND > 65534
const do_spm_t do_spm = (do_spm_t)((FLASHEND - 1023 + 2) >> 1);
#else
const do_spm_t do_spm = (do_spm_t)((FLASHEND - 511 + 2) >> 1);
#endif
#endif

#ifndef USE_NVMCTRL

// SPM-based functions

/*
 * The same as do_spm but with disable/restore interrupts state
 * required to succesfull SPM execution
 *
 * On devices with more than 64kB flash, 16 bit address is not enough,
 * so there is also RAMPZ used in that case.
 */
void do_spm_cli(optiboot_addr_t address, uint8_t command, uint16_t data) {
  uint8_t sreg_save;

  sreg_save = SREG;  // save old SREG value
  asm volatile("cli");  // disable interrupts
#ifdef RAMPZ
  RAMPZ = (address >> 16) & 0xff;  // address bits 23-16 goes to RAMPZ
#ifdef EIND
  uint8_t eind = EIND;
  EIND = FLASHEND / 0x20000;
#endif
  do_spm((address & 0xffff), command, data); // do_spm accepts only lower 16 bits of address
#ifdef EIND
  EIND = eind;
#endif
#else
  do_spm(address, command, data); // 16 bit address - no problems to pass directly
#endif
  SREG = sreg_save; // restore last interrupts state
}


// Erase page in FLASH
void optiboot_page_erase(optiboot_addr_t address) {
  do_spm_cli(address, __BOOT_PAGE_ERASE, 0);
}


// Write word into temporary buffer
void optiboot_page_fill(optiboot_addr_t address, uint16_t data) {
  do_spm_cli(address, __BOOT_PAGE_FILL, data);
}


//Write temporary buffer into FLASH
void optiboot_page_write(optiboot_addr_t address) {
  do_spm_cli(address, __BOOT_PAGE_WRITE, 0);
}


/*
 * Higher level functions for reading and writing from flash
 * See the examples for more info on how to use these functions
 */

// Function to write data to a flash page
void optiboot_writePage(const uint8_t allocated_flash_space[],
                        uint8_t data_to_store[], uint16_t page)
{
  uint16_t word_buffer = 0;

  // Erase the flash page
  optiboot_page_erase((optiboot_addr_t)(void*) &allocated_flash_space[SPM_PAGESIZE * (page - 1)]);

  // Copy ram buffer to temporary flash buffer
  for (uint16_t i = 0; i < SPM_PAGESIZE; i++)
  {
    if (i % 2 == 0) // We must write words
      word_buffer = data_to_store[i];
    else
    {
      word_buffer += (data_to_store[i] << 8);
      optiboot_page_fill((optiboot_addr_t)(void*) &allocated_flash_space[i + SPM_PAGESIZE * (page - 1)], word_buffer);
    }
  }

  // Writing temporary buffer to flash
  optiboot_page_write((optiboot_addr_t)(void*) &allocated_flash_space[SPM_PAGESIZE * (page - 1)]);
}

#else   // Newer Mega0/xTiny chips with NVMCTRL

/*
 * The same as do_nvmctrl but with disable/restore interrupts state
 * required to succesfull execution
 *
 * Currently, there are no mega0/xTint parts with more than 64k, and when there are
 * they'll need extra effort beyond just RAMPZ :-(
 */
void do_nvmctrl_cli(optiboot_addr_t address, uint8_t command, uint16_t data)
{
  uint8_t sreg_save;

  sreg_save = SREG;  // save old SREG value
  asm volatile("cli");  // disable interrupts
  do_nvmctrl(address, command, data); // 16 bit address - no problems to pass directly
  SREG = sreg_save; // restore last interrupts state
}


// Erase page in FLASH
void optiboot_page_erase(optiboot_addr_t address)
{
  // set page by writing to address.
  do_nvmctrl(address + MAPPED_PROGMEM_START, NVMCTRL_CMD_COPY_gc, 0xFF);
  do_nvmctrl_cli(0, NVMCTRL_CMD_PAGEERASE_gc, 0); // do actual erase
}


// Write word into temporary buffer
void optiboot_page_fill(optiboot_addr_t address, uint16_t data)
{
  do_nvmctrl(address + MAPPED_PROGMEM_START, NVMCTRL_CMD_COPY_gc, data & 0xFF);
  do_nvmctrl(address + MAPPED_PROGMEM_START, NVMCTRL_CMD_COPY_gc, data >> 8);
}


//Write temporary buffer into FLASH
void optiboot_page_write(optiboot_addr_t address)
{
  do_nvmctrl_cli(address, NVMCTRL_CMD_PAGEWRITE_gc, 0);
}


/*
 * Higher level functions for reading and writing from flash
 * See the examples for more info on how to use these functions
 */

// Function to write data to a flash page
void optiboot_writePage(const uint8_t allocated_flash_space[],
                        uint8_t data_to_store[], uint16_t page)
{
  const uint8_t *adjusted_address;
  // Copy ram buffer to temporary flash buffer
  for (uint16_t i = 0; i < SPM_PAGESIZE; i++)
  {
    adjusted_address = &allocated_flash_space[i + SPM_PAGESIZE * (page - 1)];
    adjusted_address += MAPPED_PROGMEM_START;
    do_nvmctrl((optiboot_addr_t)(void*) adjusted_address,
               NVMCTRL_CMD_COPY_gc, data_to_store[i]);
  }
  do_nvmctrl_cli(0, NVMCTRL_CMD_PAGEERASEWRITE_gc, 0);
}

#endif // USE_NVMTRL

// Function to read a flash page and store it in an array (storage_array[])
// (these can be shared between old and new AVRs.)
void optiboot_readPage(const uint8_t allocated_flash_space[],
                       uint8_t storage_array[], uint16_t page)
{
  uint8_t read_character;
  for (uint16_t j = 0; j < SPM_PAGESIZE; j++)
  {
    read_character = pgm_read_byte(&allocated_flash_space[j + SPM_PAGESIZE * (page - 1)]);
    storage_array[j] = read_character;
  }
}
#endif /* _OPTIBOOT_H_ */
