#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <time.h>

// Heuristically compare memory
int memcmp_heur(const void* lhs, const void* rhs, size_t length, float confidence)
{
    const char* lhs_p = lhs;
    const char* rhs_p = rhs;
    assert(confidence < 1.f);
    // Transformed from "confidence = 1 - (1 - 1/len) ^ tries"
    int tries = ceil(log(1.f - confidence) / log(1.f - 1.f/length));

    for (int count = 0; count < tries; count++) {
        int to_test = rand() % length;
        if (lhs_p[to_test] != rhs_p[to_test]) {
            return lhs_p[to_test] - rhs_p[to_test];
        }
    }

    return 0;
}

// Heuristically compare strings
int strcmp_heur(const char* lhs, const char* rhs, float confidence)
{
    size_t lhs_len = strlen(lhs);
    size_t rhs_len = strlen(rhs);
    if (lhs_len != rhs_len) {
        return 1;
    }
    return memcmp_heur(lhs, rhs, lhs_len, confidence);
}

int main(void)
{
    const char str1[] = "Never gonna give you up";
    const char str2[] = "Never gonna geve you ip";

    srand(time(NULL));
    printf("Result same: %d\n", strcmp_heur(str1, str1, 0.5f));
    printf("Result diff: %d\n", strcmp_heur(str1, str2, 0.5f));

    return 0;
}
