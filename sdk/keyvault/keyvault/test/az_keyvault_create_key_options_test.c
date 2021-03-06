// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <az_keyvault_create_key_options.h>
#include <az_span.h>

#include "./az_test.h"

#include <_az_cfg.h>

void az_create_key_options_test() {
  {
    az_keyvault_create_key_options options = { 0 };
    az_span buffer[6];
    options.operations = az_span_span_builder_create((az_mut_span_span)AZ_SPAN_FROM_ARRAY(buffer));

    // Append
    TEST_ASSERT(
        az_keyvault_create_key_options_append_operation(&options, az_keyvault_key_operation_sign())
        == AZ_OK)
    TEST_ASSERT(options.operations.length == 1)
    TEST_ASSERT(
        az_span_is_equal(options.operations.buffer.begin[0], az_keyvault_key_operation_sign()))

    // check we can't add all if not empty
    TEST_ASSERT(
        az_keyvault_create_key_options_append_all_operations(&options) == AZ_ERROR_BUFFER_OVERFLOW)

    // clear
    TEST_ASSERT(az_keyvault_create_key_options_clear(&options) == AZ_OK)
    TEST_ASSERT(options.operations.length == 0)

    // Append all
    TEST_ASSERT(az_keyvault_create_key_options_append_all_operations(&options) == AZ_OK)
    TEST_ASSERT(
        az_span_is_equal(options.operations.buffer.begin[0], az_keyvault_key_operation_decrypt()));
    TEST_ASSERT(
        az_span_is_equal(options.operations.buffer.begin[1], az_keyvault_key_operation_encrypt()));
    TEST_ASSERT(
        az_span_is_equal(options.operations.buffer.begin[2], az_keyvault_key_operation_sign()));
    TEST_ASSERT(az_span_is_equal(
        options.operations.buffer.begin[3], az_keyvault_key_operation_unwrapKey()));
    TEST_ASSERT(
        az_span_is_equal(options.operations.buffer.begin[4], az_keyvault_key_operation_verify()));
    TEST_ASSERT(
        az_span_is_equal(options.operations.buffer.begin[5], az_keyvault_key_operation_wrapKey()));

    // fail next append, full list
    TEST_ASSERT(
        az_keyvault_create_key_options_append_operation(
            &options, az_keyvault_key_operation_verify())
        == AZ_ERROR_BUFFER_OVERFLOW)

    // is full check
    TEST_ASSERT(az_keyvault_create_key_options_is_full(&options))
    TEST_ASSERT(!az_keyvault_create_key_options_is_empty(&options))

    // clear
    TEST_ASSERT(az_keyvault_create_key_options_clear(&options) == AZ_OK)

    // is full check
    TEST_ASSERT(!az_keyvault_create_key_options_is_full(&options))
    TEST_ASSERT(az_keyvault_create_key_options_is_empty(&options))
  }
}
