/* LibTomCrypt, modular cryptographic library -- Tom St Denis
 *
 * LibTomCrypt is a library that provides various cryptographic
 * algorithms in a highly modular and flexible manner.
 *
 * The library is free for all purposes without any express
 * guarantee it works.
 *
 * Tom St Denis, tomstdenis@gmail.com, http://libtom.org
 */

/**
   @file gcm_gf_mult.c
   GCM implementation, do the GF mult, by Tom St Denis
*/
#include "tomcrypt.h"

#if defined(LTC_GCM_TABLES) || defined(LRW_TABLES) || ((defined(LTC_GCM_MODE) || defined(LTC_GCM_MODE)) && defined(LTC_FAST))

/* this is x*2^128 mod p(x) ... the results are 16 bytes each stored in a packed format.  Since only the
 * lower 16 bits are not zero'ed I removed the upper 14 bytes */
const unsigned char gcm_shift_table[256*2] = {
0x00, 0x00, 0x01, 0xc2, 0x03, 0x84, 0x02, 0x46, 0x07, 0x08, 0x06, 0xca, 0x04, 0x8c, 0x05, 0x4e,
0x0e, 0x10, 0x0f, 0xd2, 0x0d, 0x94, 0x0c, 0x56, 0x09, 0x18, 0x08, 0xda, 0x0a, 0x9c, 0x0b, 0x5e,
0x1c, 0x20, 0x1d, 0xe2, 0x1f, 0xa4, 0x1e, 0x66, 0x1b, 0x28, 0x1a, 0xea, 0x18, 0xac, 0x19, 0x6e,
0x12, 0x30, 0x13, 0xf2, 0x11, 0xb4, 0x10, 0x76, 0x15, 0x38, 0x14, 0xfa, 0x16, 0xbc, 0x17, 0x7e,
0x38, 0x40, 0x39, 0x82, 0x3b, 0xc4, 0x3a, 0x06, 0x3f, 0x48, 0x3e, 0x8a, 0x3c, 0xcc, 0x3d, 0x0e,
0x36, 0x50, 0x37, 0x92, 0x35, 0xd4, 0x34, 0x16, 0x31, 0x58, 0x30, 0x9a, 0x32, 0xdc, 0x33, 0x1e,
0x24, 0x60, 0x25, 0xa2, 0x27, 0xe4, 0x26, 0x26, 0x23, 0x68, 0x22, 0xaa, 0x20, 0xec, 0x21, 0x2e,
0x2a, 0x70, 0x2b, 0xb2, 0x29, 0xf4, 0x28, 0x36, 0x2d, 0x78, 0x2c, 0xba, 0x2e, 0xfc, 0x2f, 0x3e,
0x70, 0x80, 0x71, 0x42, 0x73, 0x04, 0x72, 0xc6, 0x77, 0x88, 0x76, 0x4a, 0x74, 0x0c, 0x75, 0xce,
0x7e, 0x90, 0x7f, 0x52, 0x7d, 0x14, 0x7c, 0xd6, 0x79, 0x98, 0x78, 0x5a, 0x7a, 0x1c, 0x7b, 0xde,
0x6c, 0xa0, 0x6d, 0x62, 0x6f, 0x24, 0x6e, 0xe6, 0x6b, 0xa8, 0x6a, 0x6a, 0x68, 0x2c, 0x69, 0xee,
0x62, 0xb0, 0x63, 0x72, 0x61, 0x34, 0x60, 0xf6, 0x65, 0xb8, 0x64, 0x7a, 0x66, 0x3c, 0x67, 0xfe,
0x48, 0xc0, 0x49, 0x02, 0x4b, 0x44, 0x4a, 0x86, 0x4f, 0xc8, 0x4e, 0x0a, 0x4c, 0x4c, 0x4d, 0x8e,
0x46, 0xd0, 0x47, 0x12, 0x45, 0x54, 0x44, 0x96, 0x41, 0xd8, 0x40, 0x1a, 0x42, 0x5c, 0x43, 0x9e,
0x54, 0xe0, 0x55, 0x22, 0x57, 0x64, 0x56, 0xa6, 0x53, 0xe8, 0x52, 0x2a, 0x50, 0x6c, 0x51, 0xae,
0x5a, 0xf0, 0x5b, 0x32, 0x59, 0x74, 0x58, 0xb6, 0x5d, 0xf8, 0x5c, 0x3a, 0x5e, 0x7c, 0x5f, 0xbe,
0xe1, 0x00, 0xe0, 0xc2, 0xe2, 0x84, 0xe3, 0x46, 0xe6, 0x08, 0xe7, 0xca, 0xe5, 0x8c, 0xe4, 0x4e,
0xef, 0x10, 0xee, 0xd2, 0xec, 0x94, 0xed, 0x56, 0xe8, 0x18, 0xe9, 0xda, 0xeb, 0x9c, 0xea, 0x5e,
0xfd, 0x20, 0xfc, 0xe2, 0xfe, 0xa4, 0xff, 0x66, 0xfa, 0x28, 0xfb, 0xea, 0xf9, 0xac, 0xf8, 0x6e,
0xf3, 0x30, 0xf2, 0xf2, 0xf0, 0xb4, 0xf1, 0x76, 0xf4, 0x38, 0xf5, 0xfa, 0xf7, 0xbc, 0xf6, 0x7e,
0xd9, 0x40, 0xd8, 0x82, 0xda, 0xc4, 0xdb, 0x06, 0xde, 0x48, 0xdf, 0x8a, 0xdd, 0xcc, 0xdc, 0x0e,
0xd7, 0x50, 0xd6, 0x92, 0xd4, 0xd4, 0xd5, 0x16, 0xd0, 0x58, 0xd1, 0x9a, 0xd3, 0xdc, 0xd2, 0x1e,
0xc5, 0x60, 0xc4, 0xa2, 0xc6, 0xe4, 0xc7, 0x26, 0xc2, 0x68, 0xc3, 0xaa, 0xc1, 0xec, 0xc0, 0x2e,
0xcb, 0x70, 0xca, 0xb2, 0xc8, 0xf4, 0xc9, 0x36, 0xcc, 0x78, 0xcd, 0xba, 0xcf, 0xfc, 0xce, 0x3e,
0x91, 0x80, 0x90, 0x42, 0x92, 0x04, 0x93, 0xc6, 0x96, 0x88, 0x97, 0x4a, 0x95, 0x0c, 0x94, 0xce,
0x9f, 0x90, 0x9e, 0x52, 0x9c, 0x14, 0x9d, 0xd6, 0x98, 0x98, 0x99, 0x5a, 0x9b, 0x1c, 0x9a, 0xde,
0x8d, 0xa0, 0x8c, 0x62, 0x8e, 0x24, 0x8f, 0xe6, 0x8a, 0xa8, 0x8b, 0x6a, 0x89, 0x2c, 0x88, 0xee,
0x83, 0xb0, 0x82, 0x72, 0x80, 0x34, 0x81, 0xf6, 0x84, 0xb8, 0x85, 0x7a, 0x87, 0x3c, 0x86, 0xfe,
0xa9, 0xc0, 0xa8, 0x02, 0xaa, 0x44, 0xab, 0x86, 0xae, 0xc8, 0xaf, 0x0a, 0xad, 0x4c, 0xac, 0x8e,
0xa7, 0xd0, 0xa6, 0x12, 0xa4, 0x54, 0xa5, 0x96, 0xa0, 0xd8, 0xa1, 0x1a, 0xa3, 0x5c, 0xa2, 0x9e,
0xb5, 0xe0, 0xb4, 0x22, 0xb6, 0x64, 0xb7, 0xa6, 0xb2, 0xe8, 0xb3, 0x2a, 0xb1, 0x6c, 0xb0, 0xae,
0xbb, 0xf0, 0xba, 0x32, 0xb8, 0x74, 0xb9, 0xb6, 0xbc, 0xf8, 0xbd, 0x3a, 0xbf, 0x7c, 0xbe, 0xbe };

#endif


#if defined(LTC_GCM_MODE) || defined(LRW_MODE)

#ifndef LTC_FAST
/* right shift */
static void gcm_rightshift(unsigned char *a)
{
   int x;
   for (x = 15; x > 0; x--) {
       a[x] = (a[x]>>1) | ((a[x-1]<<7)&0x80);
   }
   a[0] >>= 1;
}

/* c = b*a */
static const unsigned char mask[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
static const unsigned char poly[] = { 0x00, 0xE1 };


/**
  GCM GF multiplier (internal use only)  bitserial
  @param a   First value
  @param b   Second value
  @param c   Destination for a * b
 */
void gcm_gf_mult(const unsigned char *a, const unsigned char *b, unsigned char *c)
{
   unsigned char Z[16], V[16];
   unsigned x, y, z;

   zeromem(Z, 16);
   XMEMCPY(V, a, 16);
   for (x = 0; x < 128; x++) {
       if (b[x>>3] & mask[x&7]) {
          for (y = 0; y < 16; y++) {
              Z[y] ^= V[y];
          }
       }
       z     = V[15] & 0x01;
       gcm_rightshift(V);
       V[0] ^= poly[z];
   }
   XMEMCPY(c, Z, 16);
}

#else

/* map normal numbers to "ieee" way ... e.g. bit reversed */
#define M(x) ( ((x&8)>>3) | ((x&4)>>1) | ((x&2)<<1) | ((x&1)<<3) )

#define BPD (sizeof(LTC_FAST_TYPE) * 8)
#define WPV (1 + (16 / sizeof(LTC_FAST_TYPE)))

/**
  GCM GF multiplier (internal use only)  word oriented
  @param a   First value
  @param b   Second value
  @param c   Destination for a * b
 */
void gcm_gf_mult(const unsigned char *a, const unsigned char *b, unsigned char *c)
{
   int i, j, k, u;
   LTC_FAST_TYPE B[16][WPV], tmp[32 / sizeof(LTC_FAST_TYPE)], pB[16 / sizeof(LTC_FAST_TYPE)], zz, z;
   unsigned char pTmp[32];

   /* create simple tables */
   zeromem(B[0],       sizeof(B[0]));
   zeromem(B[M(1)],    sizeof(B[M(1)]));

#ifdef ENDIAN_32BITWORD
   for (i = 0; i < 4; i++) {
       LOAD32H(B[M(1)][i], a + (i<<2));
       LOAD32L(pB[i],      b + (i<<2));
   }
#else
   for (i = 0; i < 2; i++) {
       LOAD64H(B[M(1)][i], a + (i<<3));
       LOAD64L(pB[i],      b + (i<<3));
   }
#endif

   /* now create 2, 4 and 8 */
   B[M(2)][0] = B[M(1)][0] >> 1;
   B[M(4)][0] = B[M(1)][0] >> 2;
   B[M(8)][0] = B[M(1)][0] >> 3;
   for (i = 1; i < (int)WPV; i++) {
      B[M(2)][i] = (B[M(1)][i-1] << (BPD-1)) | (B[M(1)][i] >> 1);
      B[M(4)][i] = (B[M(1)][i-1] << (BPD-2)) | (B[M(1)][i] >> 2);
      B[M(8)][i] = (B[M(1)][i-1] << (BPD-3)) | (B[M(1)][i] >> 3);
   }

   /*  now all values with two bits which are 3, 5, 6, 9, 10, 12 */
   for (i = 0; i < (int)WPV; i++) {
      B[M(3)][i]  = B[M(1)][i] ^ B[M(2)][i];
      B[M(5)][i]  = B[M(1)][i] ^ B[M(4)][i];
      B[M(6)][i]  = B[M(2)][i] ^ B[M(4)][i];
      B[M(9)][i]  = B[M(1)][i] ^ B[M(8)][i];
      B[M(10)][i] = B[M(2)][i] ^ B[M(8)][i];
      B[M(12)][i] = B[M(8)][i] ^ B[M(4)][i];

   /*  now all 3 bit values and the only 4 bit value: 7, 11, 13, 14, 15 */
      B[M(7)][i]  = B[M(3)][i] ^ B[M(4)][i];
      B[M(11)][i] = B[M(3)][i] ^ B[M(8)][i];
      B[M(13)][i] = B[M(1)][i] ^ B[M(12)][i];
      B[M(14)][i] = B[M(6)][i] ^ B[M(8)][i];
      B[M(15)][i] = B[M(7)][i] ^ B[M(8)][i];
   }

   zeromem(tmp, sizeof(tmp));

   /* compute product four bits of each word at a time */
   /* for each nibble */
   for (i = (BPD/4)-1; i >= 0; i--) {
       /* for each word */
       for (j = 0; j < (int)(WPV-1); j++) {
        /* grab the 4 bits recall the nibbles are backwards so it's a shift by (i^1)*4 */
           u = (pB[j] >> ((i^1)<<2)) & 15;

        /* add offset by the word count the table looked up value to the result */
           for (k = 0; k < (int)WPV; k++) {
               tmp[k+j] ^= B[u][k];
           }
       }
     /* shift result up by 4 bits */
       if (i != 0) {
          for (z = j = 0; j < (int)(32 / sizeof(LTC_FAST_TYPE)); j++) {
              zz = tmp[j] << (BPD-4);
              tmp[j] = (tmp[j] >> 4) | z;
              z = zz;
          }
       }
   }

   /* store product */
#ifdef ENDIAN_32BITWORD
   for (i = 0; i < 8; i++) {
       STORE32H(tmp[i], pTmp + (i<<2));
   }
#else
   for (i = 0; i < 4; i++) {
       STORE64H(tmp[i], pTmp + (i<<3));
   }
#endif

   /* reduce by taking most significant byte and adding the appropriate two byte sequence 16 bytes down */
   for (i = 31; i >= 16; i--) {
       pTmp[i-16] ^= gcm_shift_table[((unsigned)pTmp[i]<<1)];
       pTmp[i-15] ^= gcm_shift_table[((unsigned)pTmp[i]<<1)+1];
   }

   for (i = 0; i < 16; i++) {
       c[i] = pTmp[i];
   }

}

#endif

#endif

/* $Source: /cvs/libtom/libtomcrypt/src/encauth/gcm/gcm_gf_mult.c,v $ */
/* $Revision: 1.25 $ */
/* $Date: 2007/05/12 14:32:35 $ */
