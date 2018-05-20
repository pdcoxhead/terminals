#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <llist.h>
#include "main.h"


#define PORT 8888
#define MAXANSWERSIZE 512
#define MAXNAMESIZE 512
#define POSTBUFFERSIZE 512
#define GET 1
#define POST 2
#define ALREADY_CREATED -1

const char *terminals  = "<html><body>What terminals would you like?</body></html>";
const char *errorpage =
  "<html><body>This doesn't seem to be right.</body></html>";


char error_response_message[512];

struct connection_info_struct
{
  int connectiontype;
  char *endpoint;
  char *answerstring;
  llist* dataList;
  struct MHD_PostProcessor *postprocessor; 
};

// The idea here is that you should be able to easily extend the web server with new
// endpoints. Just add a new endpoint string and a handler and it should work
const MethodEndpointMap get_endpoint_maps[] = {
  {TERMINAL, &get_terminal},
  {"",       &error_get_notfound}, //Keep the error page as the last in the array
};

const MethodEndpointMap post_endpoint_maps[] = {
  {TERMINAL, &post_terminal},
  {"",       &error_post_notfound},
};

/************************************
*  Daemon IX
************************************/

static int
answer_to_connection (void *cls, struct MHD_Connection *connection,
                      const char *url, const char *method,
                      const char *version, const char *upload_data,
                      size_t *upload_data_size, void **con_cls)
{
  int isNewConnection = NULL == *con_cls;
  printf("answer_to_connection \n");
  if (isNewConnection)
  {
    printf("answer_to_connection isNewConnection\n");
    struct connection_info_struct *con_info;

    con_info = malloc(sizeof(struct connection_info_struct));
    if (NULL == con_info)
    {
      return MHD_NO;
    }
    con_info->answerstring = NULL;

    if (0 == strcmp (method, S_POST))
    {
      printf("answer_to_connection POST\n");

      // NOTE: iterate_post will not be called for raw json data but will for each variable if form data is passed
      con_info->postprocessor =
        MHD_create_post_processor(connection, POSTBUFFERSIZE,
                                   iterate_post, (void *) con_info);

      if (NULL == con_info->postprocessor)
      {
        printf("answer_to_connection MHD_create_post_processor failed\n");
        free (con_info);
        return MHD_NO;
      }

      con_info->connectiontype = POST;
    }
    else 
    {
      printf("answer_to_connection con_cls is NULL for GET\n");
      con_info->connectiontype = GET;
    }

    printf("answer_to_connection con_cls assigned\n");
    *con_cls = (void *) con_info;

    return MHD_YES;
  }

  if (0 == strcmp (method, S_GET))
  {
    printf("answer_to_connection GET\n");
    return handleGets(connection, url, upload_data, upload_data_size, con_cls);
  }

  if (0 == strcmp (method, S_POST))
  {
    int result = handlePosts(connection, url, upload_data, upload_data_size, con_cls);
    printf("handePosts = '%d'\n", result);
    return result;
  }

  return send_page (connection, errorpage);
}

int handlePosts(struct MHD_Connection *connection, const char *url, 
                const char *upload_data, size_t *upload_data_size,void **con_cls)
{
  printf("handlePosts data '%s' size = %zu\n", upload_data, *upload_data_size);
  struct connection_info_struct *con_info = *con_cls;
  if (*upload_data_size != 0) //Data has arrived
  {

printf("handlePosts upload_data_size != 0\n");
    MHD_post_process (con_info->postprocessor, upload_data,
                      *upload_data_size);

    int ep_map_size = sizeof(post_endpoint_maps) / sizeof(MethodEndpointMap);
    // Search for the handler for the given endpoint
    int i;
    for (i = 0; i < ep_map_size - 1; i++) { // size - 1 because last handler is an error page

      const MethodEndpointMap* map = &post_endpoint_maps[i];

      // Could use tolower to make sure it's case insenstive for the url and method
      if (strlen(url) == strlen(map->endpoint) &&
          strncmp((const char*)map->endpoint, (const char*)url, sizeof(map->endpoint)) == 0) {
        
        assert(map->handler != NULL);

        printf("handlePosts map->endpoint '%s'\n", map->endpoint);
        int result = map->handler(connection, url, upload_data, upload_data_size, con_cls);
        *upload_data_size = 0;
        return result;
      }
    }
    //no handler for endpoint - Error Page by default
    printf("No handler for post '%s' found\n", url);
    *upload_data_size = 0;
    return post_endpoint_maps[i].handler(connection, url, upload_data, upload_data_size, con_cls);
  }
  else if (NULL == con_info->answerstring)
  {
    // This is left over tutorial stuff that needs rethinking
    printf("handlePosts NULL == con_info->answerstring\n");
    // return send_page (connection, con_info->answerstring);
    return send_page (connection, "<html><body><h1>Response</h1></body></html>");
  } else {
    return send_page (connection, errorpage);
  }
}

int handleGets(struct MHD_Connection *connection, const char* url, 
              const char *upload_data, size_t *upload_data_size, 
              void **con_cls)
{
  printf ("New GET request for %s\n", url);
  int ret;
  int i;
  int ep_map_size = sizeof(get_endpoint_maps) / sizeof(MethodEndpointMap);
  // Search for the handler for the given endpoint
  for (i = 0; i < ep_map_size - 1; i++) { // size - 1 because last handler is an error page

    const MethodEndpointMap* map = &get_endpoint_maps[i];

    // Could use tolower to make sure it's case insenstive for the url and method
    if (strlen(url) == strlen(map->endpoint) &&
        strncmp((const char*)map->endpoint, (const char*)url, sizeof(map->endpoint)) == 0) {
      printf("found get handler %s == %s\n", url, map->endpoint);
      assert(map->handler != NULL);
      return ret = map->handler(connection, url, NULL, NULL, con_cls);
    }
  }
  //no handler for endpoint - Error Page by default
  return get_endpoint_maps[i].handler(connection, url, upload_data, upload_data_size, con_cls);
}

static int
iterate_post (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
              const char *filename, const char *content_type,
              const char *transfer_encoding, const char *data, uint64_t off,
              size_t size)
{
  printf("Processing Posted form data is not implemented\n");
  return MHD_NO;
}


static void request_completed (void *cls, struct MHD_Connection *connection,
                   void **con_cls, enum MHD_RequestTerminationCode toe)
{
  struct connection_info_struct *con_info = *con_cls;
printf("request_completed\n");
  if (NULL == con_info)
  {
    return;
  }

  if (con_info->connectiontype == POST)
  {
    MHD_destroy_post_processor (con_info->postprocessor);
    if (con_info->answerstring)
      free (con_info->answerstring);
  }

  free (con_info);
  *con_cls = NULL;
}

static int send_page (struct MHD_Connection *connection, const char *page)
{
  int ret;
  struct MHD_Response *response;


  response =
    MHD_create_response_from_buffer (strlen (page), (void *) page,
             MHD_RESPMEM_PERSISTENT);
  if (!response)
  {
    return MHD_NO;
  }

  ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response);

  return ret;
}
/************************************
*  Request Handlers
************************************/

int error_get_notfound(struct MHD_Connection *connection,
                   const char *url, const char *upload_data, 
                   size_t *upload_data_size, void **con_cls)
{
  printf("error_post_notfound\n");
  return error_notfound(connection, url, upload_data, upload_data_size, con_cls, S_GET);
}

int error_post_notfound(struct MHD_Connection *connection,
                   const char *url, const char *upload_data, 
                   size_t *upload_data_size, void **con_cls)
{
  printf("error_post_notfound\n");
  return error_notfound(connection, url, upload_data, upload_data_size, con_cls, S_POST);
}

int error_notfound(struct MHD_Connection *connection,
                   const char *url, const char *upload_data, 
                   size_t *upload_data_size, void **con_cls, char* method)
{
  memset(error_response_message, sizeof(error_response_message), 0x0);
  snprintf(error_response_message, sizeof(error_response_message) - 1, 
    "<html><body><h1>ERROR</h1><p>Unexpected method and endpoint combination '%s' '%s'</p></body></html>",
    method, url);
  // const char* response_message = "<html><body><h1>ERROR</h1><p>Unexpected method and endpoint combination </p></body></html>";
  struct MHD_Response *response;
  response = MHD_create_response_from_buffer (strlen (error_response_message),
                                            (void*) error_response_message, MHD_RESPMEM_PERSISTENT);
  int ret = MHD_queue_response (connection, MHD_HTTP_NOT_FOUND, response);
  MHD_destroy_response (response); //clean up
  return ret;
}

int get_terminal(struct MHD_Connection *connection,
                   const char *url, const char *upload_data, 
                   size_t *upload_data_size, void **con_cls)
{
  struct MHD_Response *response;
  response = MHD_create_response_from_buffer (strlen (terminals),
                                            (void*) terminals, MHD_RESPMEM_PERSISTENT);
  int ret = MHD_queue_response (connection, MHD_HTTP_OK, response);
  MHD_destroy_response (response); //clean up
  return ret;
}

int post_terminal(struct MHD_Connection *connection,
                   const char *url, const char *upload_data, 
                   size_t *upload_data_size, void **con_cls)
{
  struct MHD_Response *response;
  printf("received new terminal '%s'\n", upload_data);
  // MHD_HTTP_CREATED when successully created
  response = MHD_create_response_from_buffer (0,
                                            (void*) "", MHD_RESPMEM_PERSISTENT);
  int ret = MHD_queue_response (connection, MHD_HTTP_CREATED, response);

  printf("post_terminal ret '%d'\n", ret);
  MHD_destroy_response (response); //clean up
  return ret;
}

/************************************
*  Main
************************************/

#ifndef TEST
int main ()
{
  struct MHD_Daemon *daemon;
  daemon = MHD_start_daemon (MHD_USE_SELECT_INTERNALLY,
                             PORT, NULL, NULL,
                             &answer_to_connection, NULL,
                             MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL,
                             MHD_OPTION_END);
  if (NULL == daemon) return 1;
  getchar ();

  MHD_stop_daemon (daemon);
  return 0;
}
#endif
