// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

/**
 * @brief Interface declaration for bulding an HTTP Request.
 */

#ifndef _az_HTTP_REQUEST_BUILDER_H
#define _az_HTTP_REQUEST_BUILDER_H

#include <az_mut_span.h>
#include <az_pair.h>
#include <az_result.h>
#include <az_span.h>
#include <az_span_builder.h>

#include <stdbool.h>
#include <stdint.h>

#include <_az_cfg_prefix.h>

typedef struct {
  int16_t capacity;
  az_pair * headers_start;
  int16_t size;
  az_pair * retry_headers_start;
  int16_t headers_end;
} az_http_request_headers_info;

typedef struct {
  int16_t capacity;
  int16_t size;
} az_http_request_url_info;

typedef struct {
  az_mut_span buffer;
  az_span method_verb;
  az_span_builder url_builder;
  uint16_t max_headers;
  uint16_t retry_headers_start;
  uint16_t headers_end;
  az_span body;
  uint16_t query_start;
} az_http_request_builder;

extern az_span const AZ_HTTP_METHOD_VERB_GET;
extern az_span const AZ_HTTP_METHOD_VERB_HEAD;
extern az_span const AZ_HTTP_METHOD_VERB_POST;
extern az_span const AZ_HTTP_METHOD_VERB_PUT;
extern az_span const AZ_HTTP_METHOD_VERB_DELETE;
extern az_span const AZ_HTTP_METHOD_VERB_TRACE;
extern az_span const AZ_HTTP_METHOD_VERB_OPTIONS;
extern az_span const AZ_HTTP_METHOD_VERB_CONNECT;
extern az_span const AZ_HTTP_METHOD_VERB_PATCH;

/**
 * @brief Format buffer as a http request containing URL and header spans.
 *
 * @param p_hrb HTTP request builder to initialize.
 * @param buffer Buffer to store URL and header spans in.
 * @param max_url_size Maximum URL length (see @ref az_http_request_builder_set_query_parameter).
 * @param method_verb HTTP verb: `"GET"`, `"POST"`, etc.
 * @param initial_url URL.
 *
 * @return
 *   - *`AZ_OK`* success.
 *   - *`AZ_ERROR_BUFFER_OVERFLOW`* `buffer` does not have enough space to fit the `max_url_size`.
 *   - *`AZ_ERROR_ARG`*
 *     - `p_hrb` is _NULL_.
 *     - `buffer`, `method_verb`, or `initial_url` are invalid spans (see @ref az_span_is_valid).
 *     - `max_url_size` is less than `initial_url.size`.
 */
AZ_NODISCARD az_result az_http_request_builder_init(
    az_http_request_builder * const p_hrb,
    az_mut_span const buffer,
    uint16_t const max_url_size,
    az_span const method_verb,
    az_span const initial_url,
    az_span const body);

/**
 * @brief Set query parameter.
 *
 * @param p_hrb HTTP request builder that holds the URL to set the query parameter to.
 * @param name URL parameter name.
 * @param value URL parameter value.
 *
 * @return
 *   - *`AZ_OK`* success.
 *   - *`AZ_ERROR_BUFFER_OVERFLOW`* the `URL` would grow past the `max_url_size`, should the
 * parameter gets set.
 *   - *`AZ_ERROR_ARG`*
 *     - `p_hrb` is _NULL_.
 *     - `name` or `value` are invalid spans (see @ref az_span_is_valid).
 *     - `name` or `value` are empty.
 *     - `name`'s or `value`'s buffer overlap resulting `url`'s buffer.
 */
AZ_NODISCARD az_result az_http_request_builder_set_query_parameter(
    az_http_request_builder * const p_hrb,
    az_span const name,
    az_span const value);

/**
 * @brief Add a new HTTP header for the request.
 *
 * @param p_hrb HTTP request builder that holds the URL to set the query parameter to.
 * @param key Header name (e.g. `"Content-Type"`).
 * @param value Header value (e.g. `"application/x-www-form-urlencoded"`).
 *
 * @return
 *   - *`AZ_OK`* success.
 *   - *`AZ_ERROR_BUFFER_OVERFLOW`* there isn't enough space in the `p_hrb->buffer` to add a header.
 *   - *`AZ_ERROR_ARG`*
 *     - `p_hrb` is _NULL_.
 *     - `key` or `value` are invalid spans (see @ref az_span_is_valid).
 *     - `key` or `value` are empty.
 *     - `name`'s or `value`'s buffer overlap resulting `url`'s buffer.
 */
AZ_NODISCARD az_result az_http_request_builder_append_header(
    az_http_request_builder * const p_hrb,
    az_span const key,
    az_span const value);

/**
 * @brief Adds path to url request.
 * For instance, if url in request is `http://example.net?qp=1` and this function is called with
 * path equals to `test`, then request url will be updated to `http://example.net/test?qp=1`.
 *
 *
 * @param p_hrb http request builder reference
 * @param path span to a path to be appended into url
 * @return AZ_NODISCARD az_http_request_builder_append_path
 */
AZ_NODISCARD az_result
az_http_request_builder_append_path(az_http_request_builder * const p_hrb, az_span const path);

/**
 * @brief Mark that the HTTP headers that are gong to be added via
 * `az_http_request_builder_append_header` are going to be considered as retry headers.
 *
 * @param p_hrb HTTP request builder.
 *
 * @return
 *   - *`AZ_OK`* success.
 *   - *`AZ_ERROR_ARG`* `p_hrb` is _NULL_.
 */
AZ_NODISCARD az_result
az_http_request_builder_mark_retry_headers_start(az_http_request_builder * const p_hrb);

/**
 * @brief Drop all the HTTP headers that were marked as retry headers. No-op if none were marked.
 *
 * @param p_hrb HTTP request builder.
 *
 * @return
 *   - *`AZ_OK`* success.
 *   - *`AZ_ERROR_ARG`* `p_hrb` is _NULL_.
 */
AZ_NODISCARD az_result
az_http_request_builder_remove_retry_headers(az_http_request_builder * const p_hrb);

/**
 * @brief Get the HTTP header by index.
 *
 * @param p_hrb HTTP request builder.
 * @param index Index of the HTTP header to get from the builder.
 * @param out_result Pointer to write the result to.
 *
 * @return
 *   - *`AZ_OK`* success.
 *   - *`AZ_ERROR_ARG`*
 *     - `p_hrb` or `out_result` are _NULL_.
 *     - `index` is out of range.
 */
AZ_NODISCARD az_result az_http_request_builder_get_header(
    az_http_request_builder const * const p_hrb,
    uint16_t const index,
    az_pair * const out_result);

/**
 * @brief utility function for checking if there is at least one header in the request
 *
 */
AZ_NODISCARD AZ_INLINE bool az_http_request_builder_has_headers(
    az_http_request_builder const * const p_hrb) {
  return p_hrb->headers_end > 0;
}

#include <_az_cfg_suffix.h>

#endif
