
#include "test_example.h"
#include <stdbool.h>
#include <stdlib.h>

// clang-format off

// gets run before every test function
void setUp(void) {
    
}

// gets run after every test function
void tearDown(void) {

}

// test function
void test_exmaple(void) {
    TEST_ASSERT_EQUAL_INT(9 + 10, 19); 
}

// run tests in main function
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_exmaple);
    return UNITY_END();
}

// clang-format on