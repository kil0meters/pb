#include <argp.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <curl/curl.h>

// 32 byte api key
// can be found at https://pastebin.com/api#1 while logged in
#define API_DEV_KEY ""

#include "log.h"

const char *argp_program_version = "pb v0.1";
const char *argp_program_bug_address = "<kilo@meters.sh>";
static char doc[] = "Allows you to quickly post files to pastebin";
static char args_doc[] = "[FILENAME]...";
static struct argp_option options[] = {
    { "edit",     'e', 0, 0, "Open a file with $EDITOR in /tmp/ then upload it"},
    { "filetype", 'f', 0, 0, "Manually set filetype"},
    { 0 }
};

struct arguments {
  char *filetype;
  bool use_editor;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;
  switch (key) {
    case 'e': arguments->use_editor = true; break;
    case 'f': arguments->filetype = "python"; break;
    case ARGP_KEY_ARG: return 0;
    default: return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

int main(int argc, char **argv) {
  struct arguments arguments;

  arguments.filetype = "";
  arguments.use_editor = false;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  int buffer_size = 16;
  char *data = (char*) malloc(buffer_size);
  int string_size = 0;
  for(char c = getchar(); c != EOF; c = getchar()) {
    data[string_size] = c;
    if (string_size == buffer_size - 1) {
      buffer_size += buffer_size;
      data = (char*) realloc(data, buffer_size);
      if (data == 0) {
        log_error("Out of memory");
        return 1;
      }
    }
    string_size++;
  }

  CURL *curl = curl_easy_init();
  if (curl) {
    // url encode data
    char *url_encoded_data = curl_easy_escape(curl, data, string_size);
    char *post_data = (char*) malloc(77 + strlen(url_encoded_data));
    sprintf(post_data, "api_dev_key=" API_DEV_KEY "&api_option=paste&api_paste_code=%s", url_encoded_data);

    curl_easy_setopt(curl, CURLOPT_URL, "https://pastebin.com/api/api_post.php");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      log_error("Request to Pastebin's servers failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
  }
  printf("\n");
  curl_global_cleanup();
  return 0;
}
