#include "common.h"
#include "adb.h"
test_info_t test_info = {0};
process_info_t process_info = {0};
#define TEST_INFO_MAX_LEN   4096

int test_info_fprintf(char *msg,...)
{
    va_list argp;
    // char* msg ;
    // strcpy(msg,_msg.c_str());
    va_start( argp, msg );
    char *p_start = NULL;
    char tmp_val[TEST_INFO_MAX_LEN] = {0};  
    p_start = tmp_val + strlen(tmp_val);
    vsnprintf(p_start,TEST_INFO_MAX_LEN, msg,argp);
    printf("%s",tmp_val);
    va_end(argp);

    FILE *s_debug_fp = fopen("/home/cx/auto_test_test/auto_test_test.log","at+");
    fprintf(s_debug_fp,"%s",tmp_val );
    fclose(s_debug_fp);

  return 0;
}
void log_clear()
{
    FILE *fp = fopen("/home/cx/auto_test_test/auto_test_test.log","w");
    fclose(fp);
}