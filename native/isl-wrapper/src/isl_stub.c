#include <stddef.h>

#ifdef _WIN32
__declspec(dllexport)
#endif
int isl_wrapper_stub(void) {
  return 0;
}
