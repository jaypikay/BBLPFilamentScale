#ifndef _HELPERS
#define _HELPERS 1

#ifdef ENABLE_DEBUG
#define debug_print(A) Serial.print(A)
#define debug_println(A) Serial.println(A)

#else
#define debug_print(A)
#define debug_println(A)
#endif

#endif /* _HELPERS */