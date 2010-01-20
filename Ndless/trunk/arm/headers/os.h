#ifdef CAS
  #include "os_cas_1.1.9170.h"
#elif NON_CAS
  #include "os_1.1.9253.h"
#else
  #error You must specify a Nspire hardware (CAS or NON-CAS)
#endif
