#ifndef MCP23008_H
#define MCP23008_H

#include <stdint.h>

typedef int (*write_ptr)(uint8_t addr, const uint8_t *data, uint8_t len);
typedef int (*read_ptr)(uint8_t addr, uint8_t *data, uint8_t len);
typedef struct {
  write_ptr write;
  read_ptr read;
} mcp23008_t;

int mcp23008_init(mcp23008_t *obj, write_ptr write, read_ptr read);

int mcp23008_write_reg(mcp23008_t obj, uint8_t reg, uint8_t data);
int mcp23008_read_reg(mcp23008_t obj, uint8_t reg, uint8_t *data);

#endif // MCP23008_H
