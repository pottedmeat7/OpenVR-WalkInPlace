#pragma once

#ifdef NDEBUG
#undef NDEBUG
#define ELPP_THREAD_SAFE
#define ELPP_NO_DEFAULT_LOG_FILE
#define NDEBUG
#else
#define ELPP_THREAD_SAFE
#define ELPP_NO_DEFAULT_LOG_FILE
#endif
