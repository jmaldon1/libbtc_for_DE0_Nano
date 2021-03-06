/*

 The MIT License (MIT)

 Copyright (c) 2015 Douglas J. Bakkum

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*/

#include "..\include\btc\random.h"

#define TESTING

#include "libbtc-config.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
#include <windows.h>
#include <wincrypt.h>
#endif

void btc_random_init_internal(void);
btc_bool btc_random_bytes_internal(uint8_t* buf, uint32_t len, const uint8_t update_seed);

static const btc_rnd_mapper default_rnd_mapper = {btc_random_init_internal, btc_random_bytes_internal};
static btc_rnd_mapper current_rnd_mapper = {btc_random_init_internal, btc_random_bytes_internal};

void btc_rnd_set_mapper_default()
{
    current_rnd_mapper = default_rnd_mapper;
}

void btc_rnd_set_mapper(const btc_rnd_mapper mapper)
{
    current_rnd_mapper = mapper;
}

void btc_random_init(void)
{
    current_rnd_mapper.btc_random_init();
}

btc_bool btc_random_bytes(uint8_t* buf, uint32_t len, const uint8_t update_seed)
{
    return current_rnd_mapper.btc_random_bytes(buf, len, update_seed);
}

#ifdef TESTING
void btc_random_init_internal(void)
{
    srand(time(NULL));
}


btc_bool btc_random_bytes_internal(uint8_t* buf, uint32_t len, uint8_t update_seed)
{
    (void)update_seed;
    for (uint32_t i = 0; i < len; i++) {
        buf[i] = rand();
    }

    return true;
}
#else
void btc_random_init_internal(void) {}
btc_bool btc_random_bytes_internal(uint8_t* buf, uint32_t len, const uint8_t update_seed)
{
#ifdef WIN32
    HCRYPTPROV hProvider;
    int ret = CryptAcquireContextW(&hProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
    assert(ret);
    ret = CryptGenRandom(hProvider, len, buf);
    assert(ret);
    CryptReleaseContext(hProvider, 0);
#else
    (void)update_seed; //unused
    FILE* frand = fopen(RANDOM_DEVICE, "r");
    if (!frand) {
        return false;
    }

    size_t len_read = fread(buf, 1, len, frand);
    assert(len_read == len);
    fclose(frand);
    return true;
#endif
//	memset(buf, 1, len);
	return true;
}
#endif
//
//#ifndef RAND_PLATFORM_INDEPENDENT
//
//
//#pragma message("NOT SUITABLE FOR PRODUCTION USE!")
//
// The following code is not supposed to be used in a production environment.
// It's included only to make the library testable.
// The message above tries to prevent any accidental use outside of the test environment.
//
// You are supposed to replace the random32() function with your own secure code.
// There is also a possibility to replace the random_buffer() function as it is defined as a weak symbol.

//#include <stdio.h>
//#include <time.h>
//
//uint32_t random32temp(void)
//{
//	static int initialized = 0;
//	if (!initialized) {
//		srand((unsigned)time(NULL));
//		initialized = 1;
//	}
//	return ((rand() & 0xFF) | ((rand() & 0xFF) << 8) | ((rand() & 0xFF) << 16) | ((uint32_t) (rand() & 0xFF) << 24));
//}
//
//#endif /* RAND_PLATFORM_INDEPENDENT */
//
////
//// The following code is platform independent
////
//
////With __attribute__((weak)) I can declare a function which will be overridden by a
//// function with the same name, but without __attribute__((weak)) parameter.
//void __attribute__((weak)) random_buffer(uint8_t *buf, size_t len)
//{
//	uint32_t r = 0;
//	for (size_t i = 0; i < len; i++) {
//		if (i % 4 == 0) {
//			r = random32temp();
//		}
//		buf[i] = (r >> ((i % 4) * 8)) & 0xFF;
//	}
//}
