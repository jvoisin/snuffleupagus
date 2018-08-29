#ifndef SP_CURL_VERIFY_CERTIFICATES_H
#define SP_CURL_VERIFY_CERTIFICATES_H
#include "php_snuffleupagus.h"

#ifndef CURLOPT_SSL_VERIFYPEER
#define CURLOPT_SSL_VERIFYPEER 64
#endif

#ifndef CURLOPT_SSL_VERIFYHOST
#define CURLOPT_SSL_VERIFYHOST 81
#endif

int hook_curl_verify_certificates();

#endif
