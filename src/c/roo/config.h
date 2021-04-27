#include <stdint.h>

/* Enum of possible splitting / zipping modes*/
typedef enum mode{
    ZERO_FILL,
    NO_FILL
}roo_mode_t;

/* Struct for passing arguments from the main thread to the jack callbacks*/
typedef struct config {
    uint32_t window_size;
    roo_mode_t mode;
}config_t;