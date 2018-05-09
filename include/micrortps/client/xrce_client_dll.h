#ifndef XRCE_CLIENT_DLL_H
#define XRCE_CLIENT_DLL_H

#if defined(_WIN32)
#if defined(micrortps_client_SHARED)
#if defined(micrortps_client_EXPORTS)
#define client_DllAPI __declspec( dllexport )
#else
#define client_DllAPI __declspec( dllimport )
#endif // client_EXPORTS
#else
#define client_DllAPI
#endif // BUILDING_SHARED_LIBS
#else
#define client_DllAPI
#endif // _WIN32

#endif // XRCE_CLIENT_DLL_H
