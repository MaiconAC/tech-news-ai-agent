#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/typecheck-gcc.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define GEMINI_API                                                             \
  "https://generativelanguage.googleapis.com/v1beta/interactions"
#define GEMINI_API_KEY ""

struct ResponseData {
  char *chunks;
  size_t size;
};

// A resposta do curl chega em chunks, nao tudo de uma vez, e o curl
// nao faz nenhuma tratativa, cabe ao usuario fazer, a funcao de callback
// roda sempre que chega um chunk da resposta
//
// char *receivedChunk -> aponta pro buffer com o chunk recebido, está em
// binário puro
//
// size_t size -> herança do fwrite(), quase sempre é 1, mas é
// importante sempre multiplicar
//
// size_t nmemb -> tamanho do chunk
//
// void *accChunks -> são os chunks acumulados ao longo dos callbacks
size_t write_callback(char *receivedChunk, size_t size, size_t nmemb,
                      void *accChunks) {
  size_t realSize = size * nmemb;
  struct ResponseData *chunksOnMem = (struct ResponseData *)accChunks;

  // Cresce o buffer de chunks acumulados para adicionar o proximo, sempre
  // mantem um byte extra no fim com 0
  char *newPtr = realloc(chunksOnMem->chunks, chunksOnMem->size + realSize + 1);
  if (!newPtr) {
    printf("Out of memory on realloc");
    return 0;
  }

  chunksOnMem->chunks = newPtr;
  memcpy(&(chunksOnMem->chunks[chunksOnMem->size]), receivedChunk, realSize);
  chunksOnMem->size += realSize;
  chunksOnMem->chunks[chunksOnMem->size] = 0;

  // precisa retornar pro libcurl, mas os chunks são salvos no ponteiro
  return realSize;
}

int main() {
  CURL *curl;
  CURLcode responseCode;
  struct curl_slist *headers = NULL;
  struct ResponseData responseData;

  static char *agentMessage =
      "{ \"model\": \"gemini-3.6-flash\", \"input\": \"Estou testando uma integração, responda com 'Olá! Tudo bem?'\" }";

  responseCode = curl_global_init(CURL_GLOBAL_DEFAULT);

  if (responseCode != CURLE_OK) {
    return (int)responseCode;
  }

  responseData.chunks =
      malloc(1); // aloca o 1o byte e realoca durante a requisicao
  responseData.size = 0;
  curl = curl_easy_init();

  if (curl) {
    headers = curl_slist_append(headers, "x-goog-api-key: " GEMINI_API_KEY);
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, GEMINI_API);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, agentMessage);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&responseData);

    responseCode = curl_easy_perform(curl);

    // print error
    if (responseCode != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(responseCode));
    } else {
      printf("Success!\nData received: %s\n", responseData.chunks);
    }

    curl_easy_cleanup(curl);
  }

  free(responseData.chunks);

  curl_global_cleanup();

  return (int)responseCode;
}
