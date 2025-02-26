#include ".deps.h"
#include "assert.module.h"
#include "entry.h"
#include "log.h"

#define MAX_TESTS 128

typedef struct test_node test_node;

typedef union test_type test_type;

#define IS_EXCLUDE(test_pos) ((reglist[test_pos].opt >> 0) & 1)
#define IS_VERBOSE(test_pos) \
  (IS_VERBOSE_FAIL(test_pos) || IS_VERBOSE_SUCCESS(test_pos))
#define IS_VERBOSE_FAIL(test_pos) ((reglist[test_pos].opt >> 1) & 1)
#define IS_VERBOSE_SUCCESS(test_pos) ((reglist[test_pos].opt >> 2) & 1)

struct test_node {
  void (*test)(void);
  const char *filename;
  const char *testname;
  // i hope no maniac writes 65,535 lines of test in a single file
  uint16_t line;
  uint16_t opt;
  int success;
};

/* static variables */

static test_node reglist[MAX_TESTS];

static uint16_t n_test = 0;

static int end_declare = 0;

static uint16_t n_run = 0;

static uint16_t n_success = 0;

static uint16_t iter = 0;
/* Interface definition for entry-point */

void run_all_tests(void) {
  /* foolproof for the user */
  end_declare = 1;

  for (iter = 0; iter < n_test; iter++) {
    switch (IS_EXCLUDE(iter)) {
      case 1:
        if (IS_VERBOSE(iter)) {
          printf(STRIKETHROUGH MAGNETA
                 "\nExcluded test %s at line %d, file %s\n" RESET_ALL,
                 reglist[iter].testname, reglist[iter].line,
                 reglist[iter].filename);
        }
        continue;
      case 0:
        reglist[iter].test();
        ++n_run;
        n_success += reglist[iter].success;
        break;
      default:
        break;
    }
  }
}

/* Interface definition for end-user test registration */

void reglist_add(void (*test)(void), const char *test_name, const char *filename,
                 uint16_t line) {
  if (end_declare) {
    return;
  }
  reglist[n_test].test = test;
  reglist[n_test].filename = filename;
  reglist[n_test].testname = test_name;
  reglist[n_test].line = line;
  reglist[n_test].opt = 1;
  reglist[n_test].success = 1;
  n_test++;
}

void reglist_config_newest(const uint16_t opt) {
  /* n_test is already incremented when this is called,
   * so to access the "current" test, subtract n by 1 */
  reglist[n_test - 1].opt = (1 - end_declare) * opt;
}

/* Interface definition for log information requests */

void get_summary_info(uint16_t *num_test, uint16_t *num_run,
                      uint16_t *num_success) {
  (*num_test) = n_test;
  (*num_run) = n_run;
  (*num_success) = n_success;
}

void get_current_test_info(const char **test_name, uint16_t *line,
                           const char **file) {
  (*test_name) = reglist[iter].testname;
  (*line) = reglist[iter].line;
  (*file) = reglist[iter].filename;
}

uint16_t get_verbosity(void) { return (reglist[iter].opt >> 1) & 3; }

/* Interface definition for assertion results */
void notify_fail(void) { reglist[iter].success = 0; }
