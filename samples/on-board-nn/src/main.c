#include <zephyr.h>
#include <sys/printk.h>
#include <string.h>

#include "nn.h"

#define NUM_TESTS 10

void classify(void)
{
    int64_t start = k_uptime_get();
    int i;
    int dummy = 0;
    for (i = 0; i < NUM_TESTS; ++i) {
        dummy += nn_classify();
    }
    int64_t delta = k_uptime_delta(&start);

    printk("for %u tests: %lld ms (result=%d)\n", NUM_TESTS, delta, dummy);
}

void main(void)
{
    printk("on board NN speed test running on %s, stack size=%u\n",
            CONFIG_BOARD, CONFIG_MAIN_STACK_SIZE);

    if (nn_setup() >= 0) {
        classify();
    }
}

