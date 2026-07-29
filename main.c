#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/typecheck-gcc.h>
#include <stddef.h>

#define GEMINI_API "https://generativelanguage.googleapis.com/v1beta/interactions"
#define GEMINI_API_KEY ""

struct ResponseData {
	char *data;
	size_t bytesSize;
};

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
	return 0;
}

int main() {
	CURL *curl;
	CURLcode response;
	struct curl_slist *headers = NULL;
	struct ResponseData responseData;

	static char *agentMessage = "Estou testando uma integração, responda com 'Olá! Tudo bem?'";

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if (curl) {
		headers = curl_slist_append(headers, "x-goog-api-key: "GEMINI_API_KEY);
		headers = curl_slist_append(headers, "Content-Type: application/json");

		curl_easy_setopt(curl, CURLOPT_URL, GEMINI_API);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, agentMessage);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&responseData);

		response = curl_easy_perform(curl);

		// print error
		if (response != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(response));
		} else {
			printf("Success!");
		}

		curl_slist_free_all(headers);
	}

	curl_global_cleanup();
	return 0;
}
