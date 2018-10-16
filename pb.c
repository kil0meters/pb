#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

// 32 byte api key
// can be found at https://pastebin.com/api#1 while logged in 
#define API_DEV_KEY ""

int main() {
    int buffer_size = 16;
    char *data = (char*) malloc(buffer_size);
    int string_size = 0;
    for(char c = getchar(); c != EOF; c = getchar()) {
        data[string_size] = c;
        if (string_size == buffer_size - 1) {
            buffer_size += buffer_size;
            data = (char*) realloc(data, buffer_size);
            if (data == 0) {
                fprintf(stderr, "\033[1m\033[31merror:\033[0m Out of memory\n");
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
            fprintf(stderr, "\033[1m\033[31merror:\033[0m Request to Pastebin's servers failed. %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }
    printf("\n");
    curl_global_cleanup();
    return 0;
}
