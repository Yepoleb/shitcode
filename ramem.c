/*
 * Experiment to use a very big array as a fixed size heap and store
 * values by indexing it with the hash of their name
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define RAMEM_SIZE ((size_t)1048576 * 1024)
#define RAMEM_BITS 30
#define RAMEM_MASK ((1 << RAMEM_BITS) - 1)

char ramem[RAMEM_SIZE];

// 32-bit hash function
uint32_t prospector32(uint32_t x)
{
    x ^= x >> 15;
    x *= 0x2c1b3c6d;
    x ^= x >> 12;
    x *= 0x297a2d39;
    x ^= x >> 15;
    return x;
}

// Hash a string and index into a ramem index
uint32_t ramem_idx(const char* name, uint32_t index)
{
    uint32_t var_hash = index;
    for (size_t i = 0; i < strlen(name); i++) {
        var_hash += name[i];
        var_hash = prospector32(var_hash);
    }
    return var_hash & RAMEM_MASK;
}

void ramem_get(const char *name, uint32_t index, void *data, size_t size)
{
    memcpy(data, ramem + ramem_idx(name, index), size);
}

void ramem_set(const char *name, uint32_t index, const void *data, size_t size)
{
    memcpy(ramem + ramem_idx(name, index), data, size);
}

int32_t ramem_get_int32_i(const char *name, uint32_t index)
{
    int32_t val;
    ramem_get(name, index, &val, sizeof(int32_t));
    return val;
}

int32_t ramem_get_int32(const char *name)
{
    return ramem_get_int32_i(name, 0);
}

void ramem_set_int32_i(const char *name, uint32_t index, int32_t val)
{
    return ramem_set(name, index, &val, sizeof(int32_t));
}

void ramem_set_int32(const char *name, int32_t val)
{
    return ramem_set_int32_i(name, 0, val);
}

// Counts the factors of a number
void count_factors(const char* number_var, const char* factors_var)
{
    ramem_set_int32(factors_var, 0);
    for (
        ramem_set_int32("test_num", 2);
        ramem_get_int32("test_num") < ramem_get_int32(number_var);
        ramem_set_int32("test_num", ramem_get_int32("test_num") + 1)
    ) {
        if (
            (ramem_get_int32(number_var) % ramem_get_int32("test_num"))
            == 0
        ) {
            ramem_set_int32(
                factors_var, ramem_get_int32(factors_var) + 1);
        }
    }
}

int main(void)
{
    ramem_set_int32("NUM_LIMIT", 100);
    // Fill the factors array with the number of factors
    for (
        ramem_set_int32("num", 0);
        ramem_get_int32("num") < ramem_get_int32("NUM_LIMIT");
        ramem_set_int32("num", ramem_get_int32("num") + 1)
    ) {
        count_factors("num", "factors_num");
        ramem_set_int32_i(
            "factors", ramem_get_int32("num"),
            ramem_get_int32("factors_num"));
    }
    // Print the values in the factors array
    for (
        ramem_set_int32("num", 0);
        ramem_get_int32("num") < ramem_get_int32("NUM_LIMIT");
        ramem_set_int32("num", ramem_get_int32("num") + 1)
    ) {
        printf(
            "%d %d\n",
            ramem_get_int32("num"),
            ramem_get_int32_i("factors", ramem_get_int32("num")));
    }

    puts("\nEnter pls");
    getchar();
    return 0;
}
