#include "hello.h"
#include <stdio.h>
#include "config.h"
#include <stdlib.h>

#define NAPI_CALL(env, call)                                      \
  do {                                                            \
    napi_status status = (call);                                  \
    if (status != napi_ok) {                                      \
      const napi_extended_error_info* error_info = NULL;          \
      napi_get_last_error_info((env), &error_info);               \
      bool is_pending;                                            \
      napi_is_exception_pending((env), &is_pending);              \
      if (!is_pending) {                                          \
        const char* message = (error_info->error_message == NULL) \
            ? "empty error message"                               \
            : error_info->error_message;                          \
        napi_throw_error((env), NULL, message);                   \
        return NULL;                                              \
      }                                                           \
    }                                                             \
  } while(0)

static napi_value
DoSomethingUseful(napi_env env, napi_callback_info info) {
  // Do something useful.
  printf("dosomethinguseful called\n");
  config_t *conf = malloc(sizeof(config_t));
  conf->window_size = 2;
  conf->mode = ZERO_FILL;
  napi_value wrapped;
  napi_status status;
  status = napi_create_object(env,&wrapped);
  if (status != napi_ok) return NULL;
  napi_value two;
  napi_create_int32(env,2,&two);
  if (status != napi_ok) return NULL;
  napi_set_named_property(env,wrapped,"window_size",two);
  if (status != napi_ok) return NULL;
  return wrapped;
}

napi_value create_addon(napi_env env) {
  napi_value result;
  NAPI_CALL(env, napi_create_object(env, &result));

  napi_value exported_function;
  NAPI_CALL(env, napi_create_function(env,
                                      "doSomethingUseful",
                                      NAPI_AUTO_LENGTH,
                                      DoSomethingUseful,
                                      NULL,
                                      &exported_function));

  NAPI_CALL(env, napi_set_named_property(env,
                                         result,
                                         "doSomethingUseful",
                                         exported_function));

  return result;
}