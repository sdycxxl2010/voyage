#define DEVELOPMENT 1
#define INTERNAL_TEST 2
#define PUBLIC_TEST 3
#define PRODUCTION 4


#ifndef ENVIRONMENT
#define ENVIRONMENT  1
#endif

#ifndef USE_BAKCUP_SERVER
#define USE_BAKCUP_SERVER 0
#endif

//忽略服务器状态进行连接
#define FORCE_CONNECT 0

//enable language in development
#define ENABLE_LANGUAGES 1

//test code, to be removed.
#define NEW_SCENE_ARCH 1;