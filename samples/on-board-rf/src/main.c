#include <zephyr.h>
#include <sys/printk.h>

extern int rf_classify(void);

#define NUM_TESTS 10

void classify(void)
{
    int64_t start = k_uptime_get();
    int i;
    int dummy = 0;
    for (i = 0; i < NUM_TESTS; ++i) {
        dummy += rf_classify();
    }
    int64_t delta = k_uptime_delta(&start);

    printk("for %u tests: %lld ms (result=%d)\n", NUM_TESTS, delta, dummy);
}

void main(void)
{
    printk("on board RF speed test running on %s\n", CONFIG_BOARD);

    classify();
}

