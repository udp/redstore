#include <check.h>
#include <unistd.h>
#include <stdlib.h>

#include "http/redhttpd.h"
#include "easycheck.h"

START_TEST (test_http_url_unescape)
{
    char * unescaped = http_url_unescape("hello+world%20");
    fail_unless(strcmp(unescaped, "hello world ")==0, NULL);
    free(unescaped);
}
END_TEST

START_TEST (test_http_url_unescape_invalid)
{
    char * unescaped = http_url_unescape("hello%2G%2");
    fail_unless(strcmp(unescaped, "hello%2G%2")==0, NULL);
    free(unescaped);
}
END_TEST

START_TEST (test_http_url_escape)
{
    char * escaped = http_url_escape("-Hello World_.html");
    fail_unless(strcmp(escaped, "-Hello%20World_.html")==0, NULL);
    free(escaped);
}
END_TEST


EASYCHECK_TESTCASE_START(check_httpd)
EASYCHECK_TESTCASE_ADD(test_http_url_unescape)
EASYCHECK_TESTCASE_ADD(test_http_url_unescape_invalid)
EASYCHECK_TESTCASE_ADD(test_http_url_escape)
EASYCHECK_TESTCASE_END