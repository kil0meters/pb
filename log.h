#ifndef LOG_H_
#define LOG_H_

#define CYAN "\x1b[36m"
#define RED "\x1b[31m"
#define YELLOW "\x1b[33m"
#define BOLD "\x1b[1m"
#define RESET "\x1b[0m"

#ifdef debug
#define log_debug(fmt, ...) fprintf(stderr, BOLD CYAN  "debug (%s:%d): " RESET, __FILE__, __LINE__); fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#define log_error(fmt, ...) fprintf(stderr, BOLD RED   "error (%s:%d): " RESET, __FILE__, __LINE__); fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#define log_warning(fmt, ...) fprintf(stderr, BOLD YELLOW "warning (%s:%d): " RESET, __FILE__, __LINE__); fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#else
#define log_debug(fmt, ...)
#define log_error(fmt, ...) fprintf(stderr, BOLD RED   "error: " RESET); fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#define log_warning(fmt, ...) fprintf(stderr, BOLD YELLOW "warning: " RESET); fprintf(stderr, fmt "\n", ##__VA_ARGS__)
#endif

#endif // LOG_H_
