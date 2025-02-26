#ifndef SIMPLE_UTEST_REGISTER_H
#define SIMPLE_UTEST_REGISTER_H

/* this inclusion is mainly to have my LSP not scream at me */
#include <stdint.h>

#include ".deps.h"
/*
 * The heart of this utility. Controls everything about a test; from test
 * execution, option handling to log info provision
 * */

/* DO NOT call this function unless you know exactly what you want to do */
extern void reglist_add(void (*testname)(void), const char *test_name,
                        const char *filename, uint16_t line);

#define register_test(testname, OPTS)                      \
  void testname(void);                                     \
  __attribute__((constructor)) void reg_##testname(void) { \
    reglist_add(testname, #testname, __FILE__, __LINE__);  \
    reglist_config_newest(OPTS);                           \
  }                                                        \
  /* define your test */                                   \
  void testname(void)

/* options to be passed into register_test() */

/* configure the newly registered test with option */
extern void reglist_config_newest(uint16_t);

enum reg_opts {
  /* run test with no success or fail log */
  DEFAULT = 0,
  /* does not run this test */
  EXCLUDE = 1,
  /* prints out success and fail log */
  VERBOSE = 6,
  /* prints out success log */
  VERBOSE_FAIL_LOG = 2,
  /* prints out fail log */
  VERBOSE_SUCCESS_LOG = 4,
};

/* Dealing with parameterized tests */

/* argument list and parameterized list must be wrapped inside these functions
 */
#define ARGS_LIST(...) __VA_ARGS__

#define PARAM_LIST(...) (__VA_ARGS__)

/*
 * note, Clang with pedantic may say something of the line "must specify at
 * least 1 argument". It's just yappin'.
 * */
#define register_param_test(testname, arglist, OPTS, ...)                      \
  void simple_wrapper_for_##testname();                                        \
  void testname(arglist);                                                      \
  __attribute__((constructor)) void reg_simple_wrapper_for_##testname(void) {  \
    reglist_add(simple_wrapper_for_##testname, #testname, __FILE__, __LINE__); \
    reglist_config_newest(OPTS);                                               \
  }                                                                            \
  void simple_wrapper_for_##testname(void) {                                   \
    PASS_PARAM_LIST(testname, __VA_ARGS__);                                    \
  }                                                                            \
  void testname(arglist)

/* All of the below can be called by the user, but I heavily recommend
 * against doing so. All of the below are utilized inside register_param_test
 * already
 * */

#define PASS_PARAM_LIST(testname, ...) \
  __VA_OPT__(EXPAND(PASS_HELPER(testname, __VA_ARGS__)))

#define PAREN ()

#define PASS_HELPER(testname, paramlist, ...) \
  testname paramlist;                         \
  __VA_OPT__(PASS_HELPER_ALIAS PAREN(testname, __VA_ARGS__))

#define PASS_HELPER_ALIAS() PASS_HELPER

/* Parameter list expansion, maximum 64 (4^3) list for each parameterized test
 */
/* DO NOT CALL. */
#define EXPAND(arg) EXPAND1(EXPAND1(EXPAND1(EXPAND1(arg))))
#define EXPAND1(arg) EXPAND2(EXPAND2(EXPAND2(EXPAND2(arg))))
#define EXPAND2(arg) EXPAND3(EXPAND3(EXPAND3(EXPAND3(arg))))
#define EXPAND3(arg) arg

#endif  // !SIMPLE_UTEST_REGISTER_H
