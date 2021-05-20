#include <stdint.h>

/* Enum of possible splitting / zipping modes*/
typedef enum mode{
    ZERO_FILL,
    NO_FILL,
    SINE
}roo_mode_t;

/* Struct for passing arguments from the main thread to the jack callbacks*/
typedef struct config {
    uint32_t window_size;
    roo_mode_t mode;
    uint32_t _roo;
    float *window;
}config_t;