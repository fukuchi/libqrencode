#ifndef __RSECC_DECODER_H__
#define __RSECC_DECODER_H__

void RSECC_decoder_init();
int RSECC_decoder_checkSyndrome(int dl, unsigned char *data, int el, unsigned char *ecc);

#endif /* __RSECC_DECODER_H__ */
