#include "hal.h"
#include "impl.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>



typedef enum {
    OP_LIST = 'L',         // 'L' - 0x4c
    OP_READ = 'R',         // 'R' - 0x52
    OP_WRITE = 'W',        // 'W' - 0x57
    OP_RECEIVE = 'C',      // 'C' - 0x43
    OP_INTERROGATE = 'I',  // 'I' - 0x49
    OP_LISTEN = 'N',       // 'N' - 0x4e
    OP_ACK = 'A',          // 'A' - 0x41
    OP_DEBUG = 'D',        // 'D' - 0x44
    OP_ERROR = 'E',        // 'E' - 0x45
} HostOp;


// Im sure this will never fail me /s
extern uint8_t temp_buffer[MAX_READ_SIZE];


void write_response_body(uint8_t* data, size_t len);
void write_packet_header(HostOp op, uint16_t length);
void read_packet_header(HostOp* read_op, uint16_t* read_length);

void packet_send(HostOp op, uint8_t* data, size_t len);

void read_packet_segment(size_t size);


static inline bool _handle_packet_segment(size_t len, uint8_t** buff, size_t* curr_len, size_t* seg_len) {
  if (len <= *curr_len) return false;

  *seg_len = len - *curr_len;
  if (*seg_len > 256) *seg_len = 256;

  read_packet_segment(*seg_len);

  *curr_len += *seg_len;
  *buff = temp_buffer;

  return true;
}


#define STREAM_PACKET_BODY(len, VAR_BUFF, VAR_LEN)  \
    uint8_t* VAR_BUFF; \
    size_t VAR_LEN; \
    for (size_t _curr_len=0; _handle_packet_segment(len, &(VAR_BUFF), &_curr_len, &(VAR_LEN)); )



#define HOST_HEADER_SIZE (1 + 1 + 2)
