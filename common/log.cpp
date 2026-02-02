#include "log.h"
#include <string.h>
#include <time.h>
#include <unistd.h>
static FILE *s_debug_fp = NULL;

// int _cur_time_string_get( char *p_time )
// {
//   time_t current_time = time(NULL);
//   struct tm *tm       = NULL;
  
//   tm = localtime(&current_time);
//   sprintf(p_time, "%04d-%02d-%02d %02d:%02d:%02d ", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, 
//     (tm->tm_hour+8)%24, tm->tm_min, tm->tm_sec);
//   return 0;
  
// }

// int _out_log(int save, char *msg, ...)
// {
//   va_list argp;
//   // char* msg ;
//   // strcpy(msg, _msg.c_str());
//   va_start( argp, msg );
//   char *p_start = NULL;
//   char tmp_val[LOG_MAX_LEN] = {0};
//   _cur_time_string_get(tmp_val);
  
//   p_start = tmp_val + strlen(tmp_val);
//   vsnprintf(p_start, LOG_MAX_LEN - strlen(tmp_val), msg, argp);
//   printf("%s", tmp_val);
//   va_end(argp);
  
//   if ( save )
//   {
//     s_debug_fp = fopen("./version_base_test.log", "at+");
//     fprintf(s_debug_fp, "%s", tmp_val );
//     fclose(s_debug_fp);
//   }
//   return 0;
// }


static FILE *log_file = NULL;

void log_init(const char *filename) {
    log_file = fopen(filename, "a");
    if (!log_file) {
        perror("Failed to open log file");
        exit(1);
    }
}

void log_close() {
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
}

void log_internal(LogLevel level, const char *format, ...) {
    // time_t now;
    // time(&now);
    // char *time_str = ctime(&now);
    // time_str[strlen(time_str) - 1] = '\0';
//
    char time_str[LOG_MAX_LEN] = {0};

    time_t current_time = time(NULL);
    struct tm *tm       = NULL;
  
    tm = localtime(&current_time);
    sprintf(time_str, "%04d-%02d-%02d %02d:%02d:%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, 
      (tm->tm_hour)%24, tm->tm_min, tm->tm_sec);
//
    const char *level_str;
    switch (level) {
        case LOG_DEBUG: level_str = "DEBUG"; break;
        case LOG_INFO:  level_str = "INFO";  break;
        case LOG_WARN:  level_str = "WARN";  break;
        case LOG_ERROR: level_str = "ERROR"; break;
        default:        level_str = "UNKNOWN";
    }

    printf("[%s] [%s] ", time_str, level_str);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");

    if (log_file) {
        fprintf(log_file, "[%s] [%s] ", time_str, level_str);
        va_start(args, format);
        vfprintf(log_file, format, args);
        va_end(args);
        fprintf(log_file, "\n");
        fflush(log_file); 
    }
}
void wait_in_place(int time)
{
  if (time <= 0)
  {
    return ;
  }
//   log_debug("Wait for %d seconds\n", time);
  sleep(time);
}