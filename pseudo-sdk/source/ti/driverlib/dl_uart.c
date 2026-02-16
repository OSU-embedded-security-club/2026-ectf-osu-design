#define _GNU_SOURCE
#include "driverlib.h"

typedef struct {
  int fd;
} UartSimulator;

#include <fcntl.h>
#include <poll.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "dl_log.h"

int US_init_new(UartSimulator *simu);
char *US_get_name(UartSimulator *this);
ssize_t US_read(UartSimulator *this, uint8_t *buff, size_t max_size);
ssize_t US_write(UartSimulator *this, uint8_t *buff, size_t len);
void US_get_pollfd(UartSimulator *this, struct pollfd *dst);

#define HOST_INST UART0
#define HSM_INST UART1

#define HOST_INST_INT_IRQN UART0_IRQHandler
#define HSM_INST_INT_IRQN UART1_IRQHandler

extern void UART0_IRQHandler(void);
extern void UART1_IRQHandler(void);

static UartSimulator host_impl;
static UartSimulator hsm_impl;

static UartSimulator *get_sim(UART_Regs *uart) {
  if (uart == HOST_INST) {
    return &host_impl;
  } else if (uart == HSM_INST) {
    return &hsm_impl;
  }

  DL_LOG_ERROR("Invalid UART instance: %p", uart);
  exit(1);
}

void __dl_uart_poll(void) {
  struct pollfd fds[2];
  US_get_pollfd(&host_impl, &fds[0]);
  US_get_pollfd(&hsm_impl, &fds[1]);

  if (0 > poll(fds, 2, -1)) {
    DL_LOG_ERROR("poll failed");
    exit(1);
  }

  if (fds[0].revents & POLLIN) {
    HOST_INST_INT_IRQN();
  }

  if (fds[1].revents & POLLIN) {
    HSM_INST_INT_IRQN();
  }
}

#define UART_NAME(UART) (UART == HOST_INST ? "HOST" : "HSM")

// Acts as an init function for the UART
void DL_UART_enablePower(UART_Regs *uart) {
  UartSimulator *sim = get_sim(uart);
  if (US_init_new(sim)) {
    DL_LOG_ERROR("Failed to initialize UART %s", UART_NAME(uart));
    exit(1);
  }

  DL_LOG_INFO("UART %s enabled with name: %s and fd %d", UART_NAME(uart),
              US_get_name(sim), sim->fd);
}

void DL_UART_transmitDataBlocking(UART_Regs *uart, uint8_t data) {
  UartSimulator *sim = get_sim(uart);
  while (US_write(sim, &data, 1) != 1) {
    // Busy wait for transmit to succeed
  }
}

uint8_t DL_UART_receiveData(const UART_Regs *uart) {
  UartSimulator *sim = get_sim((UART_Regs *)uart);
  uint8_t data = 0;
  if (US_read(sim, &data, 1) == 1) {
    return data;
  }
  return 0;
}

uint8_t DL_UART_receiveDataBlocking(const UART_Regs *uart) {
  UartSimulator *sim = get_sim((UART_Regs *)uart);
  uint8_t data = 0;
  while (US_read(sim, &data, 1) != 1) {
    // Busy wait for data to be available
  }
  return data;
}

//==================
//    UART SIMU
//==================
#define BAUD_RATE B115200

int US_init_new(UartSimulator *this) {
  struct termios term;

  // Create the device
  int fd = posix_openpt(O_RDWR | O_NONBLOCK);
  if (-1 == fd)
    return 1;
  grantpt(fd);
  unlockpt(fd);

  // Configure it
  if (tcgetattr(fd, &term))
    return 1;
  cfmakeraw(&term);
  cfsetspeed(&term, BAUD_RATE);

  if (tcsetattr(fd, TCSANOW, &term))
    return 1;

  this->fd = fd;
  return 0;
}

char *US_get_name(UartSimulator *this) { return ptsname(this->fd); }

ssize_t US_read(UartSimulator *this, uint8_t *buff, size_t max_size) {
  return read(this->fd, buff, max_size);
}

ssize_t US_write(UartSimulator *this, uint8_t *buff, size_t len) {
  return write(this->fd, buff, len);
}

void US_get_pollfd(UartSimulator *this, struct pollfd *dst) {
  *dst = (struct pollfd){.fd = this->fd, .events = POLLIN, .revents = 0};
}