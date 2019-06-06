#ifndef dt_ASSERT_h
#define dt_ASSERT_h

#ifdef DEBUG
  #define xassert assert
#else
  #define xassert(s)
#endif


#endif
