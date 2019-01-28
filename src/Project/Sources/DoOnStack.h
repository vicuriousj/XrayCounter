#ifndef _MAIN_ASM_H
#define _MAIN_ASM_H

#ifdef __cplusplus
  extern "C" { /* our assembly functions have C calling convention */
#endif

void DoOnStack(void);
void FlashErase(unsigned char *);
void FlashProg(unsigned char *, unsigned char);

  /* interface to my assembly main function */

#ifdef __cplusplus
  }
#endif
  
#endif /* _MAIN_ASM_H */
