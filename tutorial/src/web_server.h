#ifndef __web_server_h__
#define __web_server_h__

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>

// methods
#define S_GET  "GET"
#define S_POST "POST"
#define S_PUT  "PUT"

// endpoints
#define TERMINALS "/terminals"

typedef struct _MethodEndpointMap {
	const char* endpoint;
	int (*handler)(struct MHD_Connection *connection,
                   const char *url, const char *upload_data,
                   size_t *upload_data_size, void **con_cls);
} MethodEndpointMap;

int start_daemon();

static int
iterate_post (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
              const char *filename, const char *content_type,
              const char *transfer_encoding, const char *data, uint64_t off,
              size_t size);

static int send_page (struct MHD_Connection *connection, const char *page);

static int answer_to_connection (void *cls, struct MHD_Connection *connection,
                          const char *url, const char *method, 
                          const char *version, const char *upload_data, 
                          size_t *upload_data_size, void **con_cls);


int handlePosts(struct MHD_Connection *connection, const char *url, 
                const char *upload_data, size_t *upload_data_size,void **con_cls);

int handleGets(struct MHD_Connection *connection, const char* url, 
              const char *upload_data, size_t *upload_data_size, void **con_cls);

int error_get_notfound(struct MHD_Connection *connection,
                   const char *url, const char *upload_data, 
                   size_t *upload_data_size, void **con_cls);

int error_post_notfound(struct MHD_Connection *connection,
                   const char *url, const char *upload_data, 
                   size_t *upload_data_size, void **con_cls);

int error_notfound(struct MHD_Connection *connection,
                          const char *url, const char *upload_data, 
                          size_t *upload_data_size, void **con_cls, char* method);

int get_terminals(struct MHD_Connection *connection,
                          const char *url, const char *upload_data, 
                          size_t *upload_data_size, void **con_cls);
int post_terminals(struct MHD_Connection *connection,
                          const char *url, const char *upload_data, 
                          size_t *upload_data_size, void **con_cls);
#endif