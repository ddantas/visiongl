/*********************************************************************
***                                                                ***
***  Header file vglTest.h                                         ***
***                                                                ***
***  ddantas 13/1/2019                                             ***
***                                                                ***
*********************************************************************/

#ifndef __VGLTEST_H__
#define __VGLTEST_H__


#define C_NRM  "\x1B[0m"
#define C_RED  "\x1B[31m"
#define C_GRN  "\x1B[32m"
#define C_YEL  "\x1B[33m"
#define C_BLU  "\x1B[34m"
#define C_MAG  "\x1B[35m"
#define C_CYN  "\x1B[36m"
#define C_WHT  "\x1B[37m"

int NUM_OK = 0;
int NUM_ERR = 0;
char* ASSERT_SECTION_MSG;

#define ASSERT_EQ_I(a, b, msg)   \
  if ((a) == (b))                \
  {                              \
    NUM_OK++;                    \
    printf("%s[      OK  ]%s %s: expected = %d = actual\n", C_GRN, C_NRM, msg, a); \
  }                              \
  else                           \
  {                              \
    NUM_ERR++;                   \
    printf("%s[  FAILED  ]%s %s: expected = %d != %d = actual\n", C_RED, C_NRM, msg, a, b); \
  }                              \

#define ASSERT_NEQ_I(a, b, msg)  \
  if ((a) != (b))                \
  {                              \
    NUM_OK++;                    \
    printf("%s[      OK  ]%s %s: expected = %d != %d actual\n", C_GRN, C_NRM, msg, a, b); \
  }                              \
  else                           \
  {                              \
    NUM_ERR++;                   \
    printf("%s[  FAILED  ]%s %s: expected = %d = actual\n", C_RED, C_NRM, msg, a); \
  }                              \

#define ASSERT_PRINT_MSG(msg)   \
  printf("%s[----------]%s %s\n", C_GRN, C_NRM, msg); \

#define ASSERT_SECTION_START(msg)   \
  ASSERT_SECTION_MSG = msg;         \
  printf("%s[==========]%s Start %s\n", C_GRN, C_NRM, ASSERT_SECTION_MSG); \
  NUM_OK = 0;  \
  NUM_ERR = 0; \

#define ASSERT_SECTION_END()   \
  printf("%s[==========]%s %d tests from %s\n", C_GRN, C_NRM, NUM_OK + NUM_ERR, ASSERT_SECTION_MSG); \
  printf("%s[  PASSED  ]%s %d tests\n", C_GRN, C_NRM, NUM_OK); \
  printf("%s[  FAILED  ]%s %d tests\n", C_RED, C_NRM, NUM_ERR); \
  NUM_OK = 0;  \
  NUM_ERR = 0; \


#endif
