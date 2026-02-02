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
#include <time.h>

#define _POSIX_C_SOURCE 200809L

#include <poll.h>


//=================
//   Core HAL
//=================


noreturn void HAL_on_error(){
  fprintf(stderr, "SIMU: Exiting on error");
  exit(1);
}

static bool led_on = false;
void HAL_led_on(){
  led_on = true;
  fprintf(stderr, "SIMU: Led on\n");
}
void HAL_led_off(){
  led_on = false;
  fprintf(stderr, "SIMU: Led off\n");
}
void HAL_led_toggle(){
  led_on = !led_on;
  fprintf(stderr, "SIMU: Led %s (via toggle)\n", led_on ? "on" : "off");
}



static struct timespec start_clock;
void HAL_set_timer() {
  clock_gettime(CLOCK_MONOTONIC, &start_clock);
}
uint32_t HAL_get_timer() {
  struct timespec end;

  clock_gettime(CLOCK_MONOTONIC, &end);
  uint32_t seconds = end.tv_sec - start_clock.tv_sec;
  uint32_t nanoseconds = end.tv_nsec - start_clock.tv_nsec;
  
  return (seconds * 1000000) + (nanoseconds / 1000);
}

void HAL_usleep(uint32_t micro) {
  usleep(micro);
}





static inline UartSimulator* uart_from(UartInterface i) {
  return i == UART_control ? &control_sim : &transfer_sim;
}


bool HAL_write_uart_checked(UartInterface interface, uint8_t byte) {
  ssize_t written =  US_write(uart_from(interface), &byte, 1);
  if (-1 == written) {
    if (errno == EAGAIN) {
      errno = 0;
      return 0;
    }

    fprintf(stderr, "SIMU: Failed to read from UART %s; %s\n", 
            interface == UART_control ? "control" : "transfer", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return 1 == written;
}
bool HAL_read_uart_checked(UartInterface interface, uint8_t* out_byte){
  ssize_t size = US_read(uart_from(interface), out_byte, 1);
  if (size == -1) {
    if (errno == EAGAIN) {
      errno = 0;
     return 0;
    }

    fprintf(stderr, "SIMU: Failed to read from UART %s; %s\n", 
            interface == UART_control ? "control" : "transfer", strerror(errno));
    exit(EXIT_FAILURE);
  }

  return 1 == size;
}

size_t HAL_write_uart(UartInterface interface, uint8_t* buffer, size_t len){
  return US_write(uart_from(interface), buffer, len);
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

  this -> fd = fd;
  return 0;
}

char* US_get_name(UartSimulator* this) {
  return ptsname(this -> fd);
}

ssize_t US_read(UartSimulator *this, uint8_t *buff, size_t max_size) {
  return read(this -> fd, buff, max_size);
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
