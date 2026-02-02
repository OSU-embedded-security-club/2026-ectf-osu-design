#include "host_messaging.h"
#include "hal.h"
#include "impl.h"
#include "uart_utils.h"

// #include <endian.h>
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
  // length = htole16(length);
  uint8_t* lptr = (uint8_t*) &length;

  uint8_t data[] = {'%', (uint8_t) op, lptr[0], lptr[1]};

  if (safe_uart_write(UART_control, data, sizeof(data))) {
    // TODO: Figure out a safe way to debug log this
    HAL_on_error();
  }
}

static void parse_packet_header(HostOp* read_op, uint16_t* read_length, uint8_t* src){
  if ('%' != src[0]) {
    print_debug("Not a valid host message, expected '%%', got '%x'", src[0]);
    HAL_on_error();
  }
  
  *read_op = (HostOp) src[1];
  // *read_length = le16toh( *(uint16_t*)&src[2] );
  *read_length = *(uint16_t*)&src[2] ;

  if (!hostop_is_valid(*read_op)) {
    print_debug("Not a valid host opcode");
    HAL_on_error();
  }
}



static void write_ack(){
  write_packet_header(OP_ACK, 0);
}


void read_packet_header(HostOp *read_op, uint16_t *read_length) {
  if (safe_uart_read(UART_control, temp_buffer, HOST_HEADER_SIZE)){
    print_debug("Host protocol error: Incorrect sized header");
    HAL_on_error();
  }
  parse_packet_header(read_op, read_length, temp_buffer);
  write_ack();
}



static void read_ack() {
  if (safe_uart_read(UART_control, temp_buffer, HOST_HEADER_SIZE)){
    print_debug("Host protocol error: Incorrect sized header (on expected ack)");
    HAL_on_error();
  }
 
  // These checks could be removed?
  HostOp op;
  uint16_t len;

  parse_packet_header(&op, &len, temp_buffer);
  if (op != OP_ACK) {
    print_debug("Host protocol error: Expected ack, got %x", op);
    HAL_on_error();
  }
  if (len) {
    print_debug("Host protocol error: An ack MUST have zero length");
    HAL_on_error();
  }
}










//================================
//  Important messaging functions
//================================

void write_response_body(uint8_t* data, size_t _len) {
  ptrdiff_t len = (ptrdiff_t) _len;
  read_ack();
  while (len > 0) {
    size_t amount = len > 256 ? 256 : len;

    if (safe_uart_write(UART_control, data, amount)) {
      HAL_on_error();
    }
    read_ack();

    data += amount;
    len -= (ptrdiff_t) amount;
  }


}

void packet_send(HostOp op, uint8_t* data, size_t len) {
  if (0 == data && len) {
    print_debug("Violation: IMPL_packet_send must have len=0 if data=null");
    HAL_on_error();
  }

   

  write_packet_header(op, len);
  write_response_body(data, len);
}


void read_packet_segment(uint8_t* buffer, size_t seg_size) {
  if (safe_uart_read(UART_control, buffer, seg_size)) {
    print_error("Host Protocol error: Timeout in uart segment read");
    HAL_on_error();
  }

  write_ack();
}


// Simplist since no ACKs are needed
void IMPL_write_debug(char* message) {
  size_t len = strlen(message);
   

  write_packet_header(OP_DEBUG, len);

  if (safe_uart_write(UART_control, (uint8_t*) message, len)) {
    HAL_on_error();
  }
}

void IMPL_write_error(char* message) {
  packet_send(OP_ERROR, (uint8_t*) message, message ? strlen(message) : 0);
}




