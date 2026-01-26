#include "hal.h"
#include "hal_impl.h"
#include "impl.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>



//===========================
//          STATE
//===========================

bool is_master; 
UartSimulator control_sim;
UartSimulator transfer_sim;


//===========================
//  Command Line Interface
//===========================


#define EXIT_ON_ERR(EXPR, ...) if (EXPR) { \
    fprintf(stderr, __VA_ARGS__); \
    exit(EXIT_FAILURE); }

static void setup(int argc, char *argv[]){
  bool user_needs_help = argc != 1 && argc != 2;
  bool user_wants_help = argc >= 2 && (
    0 == strcmp(argv[1], "-h") || 
    0 == strcmp(argv[1], "--help") || 
    0 == strcmp(argv[1], "-?") ||
    0 == strcmp(argv[1], "?") 
  );
  if (user_wants_help || user_needs_help) {
    fprintf(stderr, 
            "Welcome to the simulator! To run in 'master' mode, run with no arguments\n"
            "Otherwise, to run in 'companion' mode: ./out /path/to/trans\n"
            );

    exit(EXIT_FAILURE);
  }

  is_master = argc == 1;

  EXIT_ON_ERR(
    US_init_new(&control_sim), 
    "Could not init Uart simulator ttys\n");

  printf("SIMU: Please use %s in the host tools\n", US_get_name(&control_sim));
  if (is_master) {
    EXIT_ON_ERR(
      US_init_new(&transfer_sim), 
      "Could not init Uart simulator ttys\n");

    printf("SIMU: To create a companion, run: %s %s\n", argv[0], US_get_name(&transfer_sim));
  } else {

    // TODO
  }
}


//===========================
//        Main loop
//===========================


int main(int argc, char *argv[]){
  setup(argc, argv);



  while (1) {
    struct pollfd fds[2];
    US_get_pollfd(&transfer_sim, &fds[0]);
    US_get_pollfd(&control_sim, &fds[1]);


    if (0 > poll(fds, 2, -1)) {
      perror("poll");
      return EXIT_FAILURE;
    }

    if (fds[0].revents & POLLIN)  IMPL_on_uart(UART_transfer);
    if (fds[1].revents & POLLIN)  IMPL_on_uart(UART_control);

  }
}
