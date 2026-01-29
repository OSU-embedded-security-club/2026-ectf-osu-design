#pragma once
#include <poll.h>
#include <stdio.h>
#include <stdint.h>



typedef struct {
  int fd;
} UartSimulator;



int US_init_new(UartSimulator* simu);
char* US_get_name(UartSimulator* this);
ssize_t US_read(UartSimulator *this, uint8_t *buff, size_t max_size);
ssize_t US_write(UartSimulator *this, uint8_t *buff, size_t len);
void US_get_pollfd(UartSimulator *this, struct pollfd *dst);
