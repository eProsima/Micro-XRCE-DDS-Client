#include <uxr/client/util/time.h>
#include <time.h>

#ifdef WIN32
#include <Windows.h>
#endif

#ifdef __INTIME__
#include <rtdef.h>
#endif

//==================================================================
//                             PUBLIC
//==================================================================
int64_t uxr_millis(void)
{
    return uxr_nanos() / 1000000;
}

int64_t uxr_nanos(void)
{
#ifdef WIN32
    SYSTEMTIME epoch_tm = {1970, 1, 4, 1, 0, 0, 0, 0};
    FILETIME epoch_ft;
    SystemTimeToFileTime(&epoch_tm, &epoch_ft);
    uint64_t epoch_time = (((uint64_t) epoch_ft.dwHighDateTime) << 32) + epoch_ft.dwLowDateTime;

    SYSTEMTIME tm;
    FILETIME ft;
    GetSystemTime(&tm);
    SystemTimeToFileTime(&tm, &ft);
    uint64_t current_time = (((uint64_t) ft.dwHighDateTime) << 32) + ft.dwLowDateTime;

    return (current_time - epoch_time) * 100;
#elif defined(__INTIME__)
    TIMEVALUE  ts = { 0 };
    GetRtSystemTimeAsTimeValue(&ts);
    int64_t intime_epoch = (((int64_t)ts.qwSeconds) * 1000000000) + ((int64_t)ts.dwMicroseconds * 1000);

    // Adding offset from INTime epoch (January 1st, 1978) and UNIX epoch (January 1st, 1970)
    return intime_epoch + (int64_t)252460800;
#else
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (((int64_t)ts.tv_sec) * 1000000000) + ts.tv_nsec;
#endif
}
