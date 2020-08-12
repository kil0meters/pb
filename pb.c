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
  char *file;
  char *filetype;
  bool use_editor;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;
  switch (key) {
    case 'e': arguments->use_editor = true; break;
    case 'f': arguments->filetype = "python"; break;
    case ARGP_KEY_ARG: arguments->file = arg; break;
    default: return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

static bool upload_file(FILE *stream, char *filetype) {
  bool success = true;
  CURL *curl = curl_easy_init();
  if (!curl) {
    log_error("Could not initalize cURL.");
    return false;
  }

  // Get file size
  fseek(stream, 0L, SEEK_END);
  unsigned long buffer_size = ftell(stream);
  rewind(stream);

  // Another approach would be to allocate 3*buffer_size+77 which would be the
  // theoretical maximum size a buffer could be to save a realloc.
  char *buffer = malloc(sizeof(char) * buffer_size);

  // read whole file into buffer
  fgets(buffer, buffer_size, stream);

  char *encoded_buffer = curl_easy_escape(curl, buffer, buffer_size);
  char *post_data = realloc(encoded_buffer, strlen(encoded_buffer) + 512); // (char*) malloc(77 + strlen(url_encoded_data));

  strcat("api_dev_key=" API_DEV_KEY "&api_option=paste&api_paste_code=", encoded_buffer);

  curl_easy_setopt(curl, CURLOPT_URL, "https://pastebin.com/api/api_post.php");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);

  CURLcode res = curl_easy_perform(curl);

  if (res != CURLE_OK) {
    log_error("Request to Pastebin's servers failed: %s\n", curl_easy_strerror(res));
    success = false;
  }

  free(buffer);
  free(encoded_buffer);
  curl_easy_cleanup(curl);

  return success;
}

static char *guess_filetype(char *filename) {
  char c;

  bool in_filetype;
  char *filetype_buffer = malloc(sizeof(char) * strlen(filename));
  unsigned int i = 0;

  while ((c = *filename++) != '\0') {
    if (c == '.') {
      if (in_filetype) {
        i = 0;
      }
      else {
        in_filetype = true;
      }
    } else if (in_filetype) {
      filetype_buffer[i] = c;
      i++;
    }
  }

  filetype_buffer[i+1] = '\0';

  if (in_filetype == false) {
    return NULL;
  }

  return filetype_buffer;
}

int main(int argc, char **argv) {
  struct arguments arguments;

  arguments.file = "";
  arguments.filetype = "";
  arguments.use_editor = false;

  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  if (strlen(arguments.file) == 0) {
    log_error("A file is required.");
    return 1;
  }

  char *filetype = guess_filetype(arguments.file);

  if (filetype == NULL) {
    log_warning("Could not guess filetype from filename, defaulting to \"txt\"");
    filetype = "txt";
  }

  FILE *fp = fopen(arguments.file, "r");

  if (fp == NULL) {
    log_error("Could not find file \"%s\"", arguments.file);
    return 1;
  }

  bool res = upload_file(fp, filetype);

  return 0;
}

