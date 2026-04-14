#include "../include/common.h"

#ifdef _WIN32
DWORD WINAPI markerThread(LPVOID param) {
#else
void* markerThread(void* param) {
#endif
    // TODO: Implement marker thread logic
    return 0;
}
