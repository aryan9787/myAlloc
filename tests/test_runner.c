#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void test_alignment(void);
void test_basic_malloc(void);
void test_bigger_than_available_malloc(void);
void test_free(void);
void test_best_fit(void);
void test_zero_and_invalid_free(void);
void complex_set_of_malloc_and_free_calls(void);

/*
 * Runs one test in a separate child process.
 *
 * If the test crashes, the parent process survives
 * and reports the signal.
 */
void call_test(void (*test_func)(void), const char *msg)
{
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return;
    }

    /*
     * Child process
     */
    if (pid == 0) {
        test_func();

        /*
         * Test completed successfully.
         */
        _exit(0);
    }


    int status;

    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return;
    }

    if (WIFSIGNALED(status)) {

        printf("%s crashed with signal %d\n",
               msg,
               WTERMSIG(status));

    } else if (WIFEXITED(status) &&
               WEXITSTATUS(status) == 0) {

        printf("%s passed\n", msg);

    /*
     * Test failed for another reason.
     */
    } else {

        printf("%s failed\n", msg);
    }
}

int main(void)
{
    call_test(
        test_alignment,
        "Memory Alignment"
    );

    call_test(
        test_basic_malloc,
        "Basic Malloc"
    );

    call_test(
        test_bigger_than_available_malloc,
        "Request more memory Malloc"
    );

    call_test(
        test_free,
        "Basic Free"
    );

    call_test(
        test_best_fit,
        "Best Fit"
    );

    call_test(
        test_zero_and_invalid_free,
        "Invalid Requests"
    );

    call_test(
        complex_set_of_malloc_and_free_calls,
        "Complex"
    );

    return 0;
}