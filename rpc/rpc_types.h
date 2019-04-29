#ifndef RPC_TYPES_H
#define RPC_TYPES_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <stdint.h>
#ifndef boolean
typedef uint8_t boolean;
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 1
#endif

typedef enum {
    RPC_E_OK,
    RPC_E_NOT_OK,
    RPC_E_NOTFOUND,
    RPC_E_NOT_IMPLEMENTED,
    RPC_E_REPEATED,
    RPC_E_UNREACHABLE,
    RPC_E_TIMEOUT
} Rpc_ReturnType;

#define RPC_EXPECT_HELPER(boolExp) \
    if ((boolExp) != true) { \
        printf("%s: %d: FAIL\n", __FUNCTION__, __LINE__);

#define RPC_EXPECT(boolExp, err) do { \
    RPC_EXPECT_HELPER(boolExp) \
        return err; \
    } } while (0)

#define RPC_EXPECT_VOID(boolExp) do { \
    RPC_EXPECT_HELPER(boolExp) \
        return; \
    } } while (0)

#endif // RPC_TYPES_H
