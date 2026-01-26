#include "hal.h"
#include "hal_impl.h"
#include "simu_state.h"

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/poll.h>
#include <termios.h>
#include <unistd.h>
#include <poll.h>


//=================
//   Core HAL
//=================


noreturn void HAL_on_error(){
  fprintf(stdout, "SIMU: Exiting on error");
  exit(1);
}

void HAL_led_on(){
  fprintf(stdout, "SIMU: Led on");
}
void HAL_led_off(){
  fprintf(stdout, "SIMU: Led off");
}

static inline UartSimulator* uart_from(UartInterface i) {
  return i == UART_control ? &control_sim : &transfer_sim;
}

size_t HAL_read_uart(UartInterface interface, uint8_t* buffer) {
  ssize_t size = US_read(uart_from(interface), buffer);
  if (size == -1) {
    fprintf(stderr, "SIMU: Failed to read from UART %s; %s\n", 
            interface == UART_control ? "control" : "transfer", strerror(errno));
    exit(EXIT_FAILURE);
  }

  return (size_t) size;
}
void HAL_write_uart(UartInterface interface, uint8_t* buffer, size_t len) {
  ssize_t written = US_write(uart_from(interface), buffer, len);
  if (written != len) {
    fprintf(stderr, "SIMU: failed to write to UART %s: %s\n",
            interface == UART_control ? "control" : "transfer", strerror(errno));
    exit(EXIT_FAILURE);
  }
}



//==================
//    UART SIMU
//==================

#define BAUD_RATE B115200

int US_init_new(UartSimulator *this) {
  struct termios term;

  // Create the device
  int fd = posix_openpt(O_RDWR);
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

  this -> fd = fd;
  return 0;
}

char* US_get_name(UartSimulator* this) {
  return ptsname(this -> fd);
}

ssize_t US_read(UartSimulator *this, uint8_t *buff) {
  return read(this -> fd, buff, MAX_READ_SIZE);
}

ssize_t US_write(UartSimulator *this, uint8_t *buff, size_t len) {
  return write(this -> fd, buff, len);
}

void US_get_pollfd(UartSimulator *this, struct pollfd *dst) {
  *dst = (struct pollfd){
    .fd = this->fd,
    .events = POLLIN,
    .revents = 0
  };
}
