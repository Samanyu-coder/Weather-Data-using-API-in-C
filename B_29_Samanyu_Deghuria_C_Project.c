#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <cJSON.h>

// Function to handle CURL write callback
size_t write_callback(char *data, size_t size, size_t nmemb, char *buffer) {
    size_t total_size = size * nmemb;
    buffer = realloc(buffer, total_size + 1);
    if(buffer == NULL) {
        printf("Error: Unable to allocate memory.\n");
        return 0;
    }
    strncat(buffer, data, total_size);
    return total_size;
}

int main() {
    CURL *curl;
    CURLcode res;

    char *weather_api_url = "http://api.weatherstack.com/current";
    char *api_key = "a10fbaa052fad38c63078ffc17dca0d6";
    char *location = "New York";

    char *buffer = malloc(4096 * sizeof(char));
    if(buffer == NULL) {
        printf("Error: Unable to allocate memory.\n");
        return 1;
    }
    buffer[0] = '\0';

    curl = curl_easy_init();
    if(curl) {
        char request_url[512];
        sprintf(request_url, "%s?access_key=%s&query=%s", weather_api_url, api_key, location);

        curl_easy_setopt(curl, CURLOPT_URL, request_url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK) {
            printf("Error: %s\n", curl_easy_strerror(res));
            return 1;
        }

        curl_easy_cleanup(curl);

        // Parse JSON response
        cJSON *json = cJSON_Parse(buffer);
        if(json == NULL) {
            printf("Error: Failed to parse JSON.\n");
            free(buffer);
            return 1;
        }

        // Extract desired weather data from JSON
        cJSON *current = cJSON_GetObjectItem(json, "current");
        cJSON *temperature = cJSON_GetObjectItem(current, "temperature");
        cJSON *humidity = cJSON_GetObjectItem(current, "humidity");
        cJSON *description = cJSON_GetObjectItem(current, "weather_descriptions");

        // Display weather data
        printf("Temperature: %.2f°C\n", temperature->valuedouble);
        printf("Humidity: %.2f%%\n", humidity->valuedouble);
        printf("Description: %s\n", description->child->valuestring);

        // Clean up
        cJSON_Delete(json);
        free(buffer);
    }

    return 0;
}
