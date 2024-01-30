#ifndef MCP23008_H
#define MCP23008_H 

#include <stdint.h>

typedef struct {
    int (*write)(uint8_t addr, const uint8_t *data, uint8_t len);
    int (*read)(uint8_t addr, uint8_t *data, uint8_t len);
} mcp23008_t;

int mcp23008_init(mcp23008_t obj, int (*write_func)(uint8_t, const uint8_t*, uint8_t), int (*read_func)(uint8_t, uint8_t*, uint8_t));

int mcp23008_write_reg(mcp23008_t obj, uint8_t reg, uint8_t data);
int mcp23008_read_reg(mcp23008_t obj, uint8_t reg, uint8_t *data);

#endif // MCP23008_H

