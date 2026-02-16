#ifndef ti_dl_dl_log__include
#define ti_dl_dl_log__include

#include <stdio.h>

typedef enum {
  DL_LOG_LEVEL_NONE = 0,
  DL_LOG_LEVEL_ERROR,
  DL_LOG_LEVEL_WARNING,
  DL_LOG_LEVEL_INFO,
  DL_LOG_LEVEL_DEBUG
} DL_Log_Level;

#ifndef DL_LOG_LEVEL
#ifdef DEBUG
#define DL_LOG_LEVEL DL_LOG_LEVEL_DEBUG
#else
#define DL_LOG_LEVEL DL_LOG_LEVEL_INFO
#endif
#endif

#define DL_LOG_ERROR(...)                                                      \
  do {                                                                         \
    if (DL_LOG_LEVEL >= DL_LOG_LEVEL_ERROR) {                                  \
      printf("[ERROR] " __VA_ARGS__);                                          \
      printf("\n");                                                            \
    }                                                                          \
  } while (0)

#define DL_LOG_WARNING(...)                                                    \
  do {                                                                         \
    if (DL_LOG_LEVEL >= DL_LOG_LEVEL_WARNING) {                                \
      printf("[WARNING] " __VA_ARGS__);                                        \
      printf("\n");                                                            \
    }                                                                          \
  } while (0)

#define DL_LOG_INFO(...)                                                       \
  do {                                                                         \
    if (DL_LOG_LEVEL >= DL_LOG_LEVEL_INFO) {                                   \
      printf("[INFO] " __VA_ARGS__);                                           \
      printf("\n");                                                            \
    }                                                                          \
  } while (0)

#define DL_LOG_DEBUG(...)                                                      \
  do {                                                                         \
    if (DL_LOG_LEVEL >= DL_LOG_LEVEL_DEBUG) {                                  \
      printf("[DEBUG] " __VA_ARGS__);                                          \
      printf("\n");                                                            \
    }                                                                          \
  } while (0)

#endif /* ti_dl_dl_log__include */
