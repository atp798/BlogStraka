#ifndef __WEB_SERVER_H__
#define __WEB_SERVER_H__
#include "c_types.h"
#include "cJSON.h"

#define MAX_HOST 	15
#define MAX_PATH	15
#define MAX_PARAM	15

#define WEBTASK_QUEUE_LEN	4

typedef enum HttpMethod{
	GET,
	POST
}HttpMethod;

typedef enum HttpStatusCode{
	SUCCESS = 200,
	REDIRECTION = 301,
	BAD_REQUEST = 400,
	SERV_ERROR = 500
} HttpStatusCode;

typedef struct URLParam{
    enum HttpMethod eMethod;
    uint8 nPort;
    char szHost[MAX_HOST+1];
    char szPath[MAX_PATH+1];
    char szParam[MAX_PARAM+1];
} URLParam;

typedef enum PostEncode{
	JSON_ENCODE,
	URL_ENCODE
}PostEncode;

typedef struct PostParam{
	PostEncode eEncode;
	cJSON *jsonData;
}PostParam;

void WebServTask(os_event_t *e);

void WebServInit(uint32 port);

#endif
