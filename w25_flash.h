#ifndef W25_FLASH_H
#define W25_FLASH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define WINBOND_MANU_ID 0xEF


union w25_flash_status_reg1_t {
    struct
    {
        bool busy : 1;
        bool wen : 1;

        union {
            struct
            {
                uint8_t bp0 : 1;
                uint8_t bp1 : 1;
                uint8_t bp2 : 1;
            };
            uint8_t bp : 3;
        };

        bool tb : 1;
        bool sec : 1;
        bool srp : 1;
    };

    uint8_t v : 8;
};

union w25_flash_status_reg2_t {
    struct
    {
        bool srl : 1;
        bool qe : 1;
        bool : 1;

        union {

            struct
            {
                uint8_t lb1 : 1;
                uint8_t lb2 : 1;
                uint8_t lb3 : 1;
            };
            uint8_t lb : 3;
        };

        bool cmp : 1;
        bool sus : 1;
    };
    uint8_t v : 8;
};

union w25_flash_status_reg3_t {
    struct
    {
        bool : 1;
        bool : 1;
        bool wps : 1;
        bool : 1;
        bool : 1;
        uint8_t drv : 2;
        bool : 1;
    };
    uint8_t v : 8;
};

struct jedec_id_t
{
    uint8_t manuf_id;
    uint8_t mem_type;
    uint8_t capacity;
};

void w25_flash_read_jedec(struct jedec_id_t *jedic);
uint8_t w25_flash_read_device_id(void);
void w25_flash_status_reg1(union w25_flash_status_reg1_t *status1);
void w25_flash_status_reg2(union w25_flash_status_reg2_t *status2);
void w25_flash_status_reg3(union w25_flash_status_reg3_t *status3);
void w25_flash_write_enable(void);
void w25_flash_write_disable(void);
void w25_flash_write_enable_volreg(void);
void w25_flash_read_data(uint32_t addr, uint8_t *buf, size_t count);
void w25_flash_fast_read_data(uint32_t addr, uint8_t *buf, size_t count);
void w25_flash_page_program(const uint32_t addr, void *buf, size_t count, bool fast);
void w25_flash_sector_erase(const uint32_t addr);
void w25_flash_32k_block_erase(const uint32_t addr);
void w25_flash_64k_block_erase(const uint32_t addr);
void w25_flash_chip_erase(void);
void w25_flash_eraseprog_suspend(void);
void w25_flash_eraseprog_resume(void);
void w25_flash_power_down(void);
void w25_flash_power_resume(void);
uint64_t w25_flash_read_unique_id(void);


#endif /* W25_FLASH_H */
