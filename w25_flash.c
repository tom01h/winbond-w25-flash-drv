#include "w25_flash.h"
#include "spi.h"

/* sending a dummy byte just to read exchanged value */
static const uint8_t READ_DUMMY = 0xAA;

/* sending a dummy byte that read value is not used */
static const uint8_t EX_DUMMY = 0xEE;

/* opcodes */
static const uint8_t OP_WRITE_ENABLE = 0x06;
static const uint8_t OP_WRITE_ENABLE_VOLREGS = 0x50;
static const uint8_t OP_WRITE_DISABLE = 0x04;
static const uint8_t OP_READ_STATUS_REG_1 = 0x05;
static const uint8_t OP_READ_STATUS_REG_2 = 0x35;
static const uint8_t OP_READ_STATUS_REG_3 = 0x15;
static const uint8_t OP_WRITE_STATUS_REG_1 = 0x01;
static const uint8_t OP_WRITE_STATUS_REG_2 = 0x31;
static const uint8_t OP_WRITE_STATUS_REG_3 = 0x11;
static const uint8_t OP_READ_DATA = 0x03;
static const uint8_t OP_FAST_READ_DATA = 0x0B;
static const uint8_t OP_FAST_READ_DUAL_OUTPUT = 0x3B;
static const uint8_t OP_FAST_READ_QUAD_OUTPUT = 0x6B;
static const uint8_t OP_FAST_READ_DUAL_IO = 0xBB;
static const uint8_t OP_FAST_READ_QUAD_IO = 0xEB;
static const uint8_t OP_SET_BURST_WRAP = 0x77;
static const uint8_t OP_PAGE_PROGRAM = 0x02;
static const uint8_t OP_QUAD_PAGE_PROGRAM = 0x32;
static const uint8_t OP_SECTOR_ERASE = 0x20;
static const uint8_t OP_32KB_BLOCK_ERASE = 0x52;
static const uint8_t OP_64KB_BLOCK_ERASE = 0xD8;
static const uint8_t OP_CHIP_ERASE = 0xC7;
static const uint8_t OP_ERASEPROG_SUSPEND = 0x75;
static const uint8_t OP_ERASEPROG_RESUME = 0x7A;
static const uint8_t OP_POWER_DOWN = 0xB9;
static const uint8_t OP_DEVICE_ID_WAKEUP = 0xAB;
static const uint8_t OP_READ_MANUF_DEVICE_ID = 0x90;
static const uint8_t OP_READ_MANUF_DEVICE_ID_DUAL_IO = 0x92;
static const uint8_t OP_READ_MANUF_DEVICE_ID_QUAD_IO = 0x94;
static const uint8_t OP_READ_UNIQUE_ID_NUM = 0x4B;
static const uint8_t OP_READ_JEDEC_ID = 0x9F;
static const uint8_t OP_READ_SFDP_REGS = 0x5A;
static const uint8_t OP_ERASE_SECURITY_REGS = 0x44;
static const uint8_t OP_PROG_SECURITY_REGS = 0x42;
static const uint8_t OP_READ_SECURITY_REGS = 0x42;
static const uint8_t OP_SECTOR_LOCK = 0x36;
static const uint8_t OP_SECTOR_UNLOCK = 0x36;
static const uint8_t OP_READ_SECTOR_LOCK = 0x36;
static const uint8_t OP_GLOBAL_SECTOR_LOCK = 0x7E;
static const uint8_t OP_GLOBAL_SECTOR_UNLOCK = 0x98;
static const uint8_t OP_ENABLE_RESET = 0x66;
static const uint8_t OP_RESET_DEVICE = 0x99;


void w25_flash_read_jedec(struct jedec_id_t *jedic)
{
    SPI_FLASH_CS_ENABLE();

    SPI_EXCHANGE(OP_READ_JEDEC_ID);
    uint8_t mfid = SPI_EXCHANGE(READ_DUMMY);
    uint8_t memtype = SPI_EXCHANGE(READ_DUMMY);
    uint8_t capacity = SPI_EXCHANGE(READ_DUMMY);

    SPI_FLASH_CS_DISABLE();

    jedic->manuf_id = mfid;
    jedic->mem_type = memtype;
    jedic->capacity = capacity;
}

uint8_t w25_flash_read_device_id()
{
    SPI_FLASH_CS_ENABLE();
    
    SPI_EXCHANGE(OP_DEVICE_ID_WAKEUP);
    SPI_EXCHANGE(EX_DUMMY);
    SPI_EXCHANGE(EX_DUMMY);
    SPI_EXCHANGE(EX_DUMMY);
    uint8_t dev_id = SPI_EXCHANGE(READ_DUMMY);
    
    SPI_FLASH_CS_DISABLE();

    return dev_id;
}

static void noread_op(uint8_t op)
{
    SPI_FLASH_CS_ENABLE();
    SPI_EXCHANGE(OP_WRITE_ENABLE);
    SPI_FLASH_CS_DISABLE();
}

static uint8_t read_reg(uint8_t op)
{
    uint8_t v;
    SPI_FLASH_CS_ENABLE();
    SPI_EXCHANGE(OP_READ_STATUS_REG_1);
    v = SPI_EXCHANGE(READ_DUMMY);
    SPI_FLASH_CS_DISABLE();

    return v;
}

void w25_flash_status_reg1(union w25_flash_status_reg1_t *status1)
{
    uint8_t v = read_reg(OP_READ_STATUS_REG_1);
    status1->v = v;
}

void w25_flash_status_reg2(union w25_flash_status_reg2_t *status2)
{
    uint8_t v = read_reg(OP_READ_STATUS_REG_2);
    status2->v = v;
}

void w25_flash_status_reg3(union w25_flash_status_reg3_t *status3)
{
    uint8_t v = read_reg(OP_READ_STATUS_REG_3);
    status3->v = v;
}

static void spin_while_busy()
{
    union w25_flash_status_reg1_t status1;
    while (1)
    {        
        w25_flash_status_reg1(&status1);
        
        if (!status1.busy)
        {
            break;
        }
    }   
}

void w25_flash_write_enable(void)
{
    noread_op(OP_WRITE_ENABLE);
}

void w25_flash_write_disable(void)
{
    noread_op(OP_WRITE_DISABLE);
}

void w25_flash_write_enable_volreg(void)
{
    noread_op(OP_WRITE_ENABLE_VOLREGS);
}

/* sends op and then 24 bits of addr msb shifted, 
must be used during CS enable - assumed done by caller */
static void send_24bit_addr_op(const uint32_t addr, const uint8_t op)
{
    uint8_t mem1 = ((addr & 0xFFFFFF) >> 16);
    uint8_t mem2 = ((addr & 0xFFFF) >> 8);
    uint8_t mem3 = addr & 0xFF;

    SPI_EXCHANGE(op);
    SPI_EXCHANGE(mem1);
    SPI_EXCHANGE(mem2);
    SPI_EXCHANGE(mem3);
}

static void read_data(const uint32_t addr, void *buf, size_t count, bool fast)
{
    SPI_FLASH_CS_ENABLE();

    send_24bit_addr_op(addr, fast ? OP_FAST_READ_DATA : OP_READ_DATA);

    if (fast)
    {
        /* 
        for fast read, you simply send one dummy byte before reading,
        this allows the device additional time for setting up initial address
        */
        SPI_EXCHANGE(EX_DUMMY);
    }

    uint8_t *byte_buf = buf;
    size_t i;
    for (i = 0; i < count; ++i)
    {
        uint8_t readb = SPI_EXCHANGE(EX_DUMMY);
        *byte_buf++ = readb;
    }

    SPI_FLASH_CS_DISABLE();
    
    spin_while_busy();
}

void w25_flash_read_data(uint32_t addr, uint8_t *buf, size_t count)
{
    read_data(addr, buf, count, false);
}

void w25_flash_fast_read_data(uint32_t addr, uint8_t *buf, size_t count)
{
    read_data(addr, buf, count, true);
}

void w25_flash_page_program(const uint32_t addr, void *buf, size_t count, bool fast)
{
    uint8_t *byte_buf = buf;
    size_t i;
    
    SPI_FLASH_CS_ENABLE();
    
    send_24bit_addr_op(addr, OP_PAGE_PROGRAM);
    
    for (i = 0; i < count; ++i)
    {
        SPI_EXCHANGE(*byte_buf++);
    }

    SPI_FLASH_CS_DISABLE();

    spin_while_busy();
}

void w25_flash_sector_erase(const uint32_t addr)
{
    SPI_FLASH_CS_ENABLE();
    send_24bit_addr_op(addr, OP_SECTOR_ERASE);   
    SPI_FLASH_CS_DISABLE();
    spin_while_busy();
}

void w25_flash_32k_block_erase(const uint32_t addr)
{
    SPI_FLASH_CS_ENABLE();
    send_24bit_addr_op(addr, OP_32KB_BLOCK_ERASE);
    SPI_FLASH_CS_DISABLE();
    spin_while_busy();
}

void w25_flash_64k_block_erase(const uint32_t addr)
{
    SPI_FLASH_CS_ENABLE();
    send_24bit_addr_op(addr, OP_64KB_BLOCK_ERASE);
    SPI_FLASH_CS_DISABLE();
    spin_while_busy();
}

void w25_flash_chip_erase(void)
{
    noread_op(OP_CHIP_ERASE);
    spin_while_busy();
}

void w25_flash_eraseprog_suspend(void)
{
    noread_op(OP_ERASEPROG_SUSPEND);
}

void w25_flash_eraseprog_resume(void)
{
    noread_op(OP_ERASEPROG_RESUME);
}

void w25_flash_power_down(void)
{
    noread_op(OP_POWER_DOWN);
}

void w25_flash_power_resume(void)
{
    w25_flash_read_device_id();
}

uint64_t w25_flash_read_unique_id(void)
{
    SPI_FLASH_CS_ENABLE();
    
    SPI_EXCHANGE(OP_READ_UNIQUE_ID_NUM);
    SPI_EXCHANGE(EX_DUMMY);
    SPI_EXCHANGE(EX_DUMMY);
    SPI_EXCHANGE(EX_DUMMY);
    SPI_EXCHANGE(EX_DUMMY);
    /* unique id is 64 bit, dang */

    uint8_t b8 = SPI_EXCHANGE(EX_DUMMY); 
    uint8_t b7 = SPI_EXCHANGE(EX_DUMMY); 
    uint8_t b6 = SPI_EXCHANGE(EX_DUMMY); 
    uint8_t b5 = SPI_EXCHANGE(EX_DUMMY); 
    uint8_t b4 = SPI_EXCHANGE(EX_DUMMY); 
    uint8_t b3 = SPI_EXCHANGE(EX_DUMMY); 
    uint8_t b2 = SPI_EXCHANGE(EX_DUMMY); 
    uint8_t b1 = SPI_EXCHANGE(EX_DUMMY); 

    SPI_FLASH_CS_DISABLE();

    uint64_t val = b1 | 
        (b2 << 8) | 
        ((uint64_t)b3) << 16 |
        ((uint64_t)b4) << 24 |
        ((uint64_t)b5) << 32 |
        ((uint64_t)b6) << 40 |
        ((uint64_t)b7) << 48 |
        ((uint64_t)b8) << 56;

    return val;
}
