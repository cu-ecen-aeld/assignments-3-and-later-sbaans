#include "unity.h"
#include <stdbool.h>
#include <stdlib.h>
#include "../../examples/autotest-validate/autotest-validate.h"
#include "../../assignment-autotest/test/assignment1/username-from-conf-file.h"


/**
* This function should:
*   1) Call the my_username() function in Test_assignment_validate.c to get your hard coded username.
*   2) Obtain the value returned from function malloc_username_from_conf_file() in username-from-conf-file.h within
*       the assignment autotest submodule at assignment-autotest/test/assignment1/
*   3) Use unity assertion TEST_ASSERT_EQUAL_STRING_MESSAGE the two strings are equal.  See
*       the [unity assertion reference](https://github.com/ThrowTheSwitch/Unity/blob/master/docs/UnityAssertionsReference.md)
*/
void test_validate_my_username()
{
    /**
     * TODO: Replace the line below with your code here as described above to verify your /conf/username.txt 
     * config file and my_username() functions are setup properly
     */

    /* Print the value from my_username */
    const char *hardcoded_username;
    const char *username_from_conf;

    hardcoded_username = my_username();

    printf("\n This the hardcoded value : %s \n",hardcoded_username);

    /* Print the value from /conf/username.txt */
    username_from_conf = malloc_username_from_conf_file();

    printf("\n This the username from conf value : %s \n",username_from_conf);

    /* Verify that they are equals */
    TEST_ASSERT_EQUAL_STRING_MESSAGE(hardcoded_username,username_from_conf,"The TEST is NOT OK, the strings are different");

}
