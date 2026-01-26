#include "host_messaging.h"
#include "hal.h"
#include "impl.h"
#include "impl_state.h"

#include <endian.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint8_t temp_buffer[MAX_READ_SIZE];

//=========================
//   HELPER FUNCTIONS
//=========================

static inline bool hostop_is_valid(HostOp op) {
    switch (op) {
        case OP_LIST:
        case OP_READ:
        case OP_WRITE:
        case OP_RECEIVE:
        case OP_INTERROGATE:
        case OP_LISTEN:
        case OP_ACK:
        case OP_DEBUG:
        case OP_ERROR:
            return true;
        default:
            return false;
    }
}

void write_packet_header(HostOp op, uint16_t length) {
  length = htole16(length);
  uint8_t* lptr = (uint8_t*) &length;

  uint8_t data[] = {'%', (uint8_t) op, lptr[0], lptr[1]};

  HAL_write_uart(UART_control, data, sizeof(data));
}

static void _read_packet_header(HostOp* read_op, uint16_t* read_length, uint8_t* src){
  if ('%' != src[0]) {
    print_debug("Not a valid host message, expected '%%', got '%x'", src[0]);
    HAL_on_error();
  }
  
  *read_op = (HostOp) src[1];
  *read_length = le16toh( *(uint16_t*)&src[2] );

  if (!hostop_is_valid(*read_op)) {
    print_debug("Not a valid host opcode");
    HAL_on_error();
  }
}



void write_ack(){
  write_packet_header(OP_ACK, 0);
}


void read_packet_header(HostOp *read_op, uint16_t *read_length) {
  if (HOST_HEADER_SIZE != HAL_read_uart(UART_control, temp_buffer)) {
    print_debug("Host protocol error: Incorrect sized header");
    HAL_on_error();
  }
  _read_packet_header(read_op, read_length, temp_buffer);
  write_ack();
}



static void read_ack() {
  size_t len = HAL_read_uart(UART_control, temp_buffer);

  if (len != HOST_HEADER_SIZE) {
    print_error("Host protocol error: invalid ack packet size");
    HAL_on_error();
  }
}










//================================
//  Important messaging functions
//================================



void write_response_body(uint8_t* data, size_t len) {
  read_ack();
  while (len > 0) {
    size_t amount = len > 256 ? 256 : len;

    HAL_write_uart(UART_control, data, amount);
    read_ack();

    data += amount;
    len -= amount;
  }


}

void packet_send(HostOp op, uint8_t* data, size_t len) {
  if (!data && len) {
    print_debug("Violation: IMPL_packet_send must have len=0 if data=null");
    HAL_on_error();
  }

  START_UART_SECTION

  write_packet_header(op, len);
  write_response_body(data, len);

  END_UART_SECTION
}



size_t read_packet_segment() {
  size_t out = HAL_read_uart(UART_control, temp_buffer);
  write_ack();

  if (out > 256) {
    print_error("Host Protocol error: Illegal segment size: %lu", (unsigned long) out);
    HAL_on_error();
  }
  return out;

}



// Simplist since no ACKs are needed
void IMPL_write_debug(char* message) {
  size_t len = strlen(message);

  START_UART_SECTION

  write_packet_header(OP_DEBUG, len);
  HAL_write_uart(UART_control, (uint8_t*) message, len);

  END_UART_SECTION
}

void IMPL_write_error(char* message) {
  packet_send(OP_ERROR, (uint8_t*) message, message ? strlen(message) : 0);
}




