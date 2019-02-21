#define _GNU_SOURCE 1
#include <ucontext.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void action_handler(int sig, siginfo_t* info, void* context_p)
{
    ucontext_t* ucontext = (ucontext_t*)context_p;
    ucontext->uc_mcontext.gregs[REG_RIP] += 6;
}

int test_pointer(const void* p);
__asm__(
    ".global test_pointer\n"
    "test_pointer:\n"
    "movb (%rdi), %al\n"
    "movl $0, %eax\n"
    "ret\n"
    "movl $1, %eax\n"
    "ret\n"
);

size_t strlen_segv(const char* str)
{
    struct sigaction sa_set = {0};
    sa_set.sa_sigaction = action_handler;
    sa_set.sa_flags = SA_SIGINFO;
    sigemptyset(&sa_set.sa_mask);
    sigaction(SIGSEGV , &sa_set, NULL);

    size_t index = 0;
    while (1) {
        int did_segfault = test_pointer(str + index);
        if (did_segfault) {
            break;
        }
        index++;
    }

    struct sigaction sa_unset = {0};
    sa_unset.sa_handler = SIG_DFL;
    sigemptyset(&sa_unset.sa_mask);
    sigaction(SIGSEGV , &sa_unset, NULL);

    return index;
}

int main(void)
{
    char* str1 = calloc(20, 1);
    const char* str2 = "Hello world!";
    size_t length = strlen_segv(str1);
    printf("len: %zu\n", length);
    printf("addr1: %p\n", str1);
    size_t length2 = strlen_segv(str2);
    printf("len2: %zu\n", length2);
    printf("addr1: %p\n", str2);
    getchar();

    return 0;
}
