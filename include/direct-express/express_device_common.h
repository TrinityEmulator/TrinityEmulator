#ifndef EXPRESS_DEVICE_COMMON_H
#define EXPRESS_DEVICE_COMMON_H

#include "direct-express/direct_express_distribute.h"

#define EXPRESS_GPU_FUN_ID ((uint64_t)1)
#define EXPRESS_LOG_FUN_ID ((uint64_t)2)

#define GET_DEVICE_ID(id) ((id) >> 32)
#define GET_FUN_ID(id) ((id)&0xffffff)
#define FUN_NEED_SYNC(id) (((id) >> 24) & 0x1)
#define FUN_HAS_HOST_SYNC(id) (((id) >> 24) & 0x2)

#endif