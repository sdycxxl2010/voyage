 /* sha256.c
 *
 * Copyright (C) 2006-2015 wolfSSL Inc.
 *
 * This file is part of wolfSSL. (formerly known as CyaSSL)
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

/* code submitted by raphael.huck@efixo.com */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include "settings.h"
#include "sha256.h"

#if !defined(NO_SHA256)
#ifdef HAVE_FIPS

int wc_InitSha256(Sha256* sha)
{
    return InitSha256_fips(sha);
}


int wc_Sha256Update(Sha256* sha, const byte* data, word32 len)
{
    return Sha256Update_fips(sha, data, len);
}


int wc_Sha256Final(Sha256* sha, byte* out)
{
    return Sha256Final_fips(sha, out);
}


int wc_Sha256Hash(const byte* data, word32 len, byte* out)
{
    return Sha256Hash(data, len, out);
}

#else /* else build without fips */

#if !defined(NO_SHA256) && defined(WOLFSSL_TI_HASH)
    /* #include <wolfcrypt/src/port/ti/ti-hash.c> included by wc_port.c */
#else

#if !defined (ALIGN32)
    #if defined (__GNUC__)
        #define ALIGN32 __attribute__ ( (aligned (32)))
    #elif defined(_MSC_VER)
        /* disable align warning, we want alignment ! */
        #pragma warning(disable: 4324)
        #define ALIGN32 __declspec (align (32))
    #else
        #define ALIGN32
    #endif
#endif

#ifdef WOLFSSL_PIC32MZ_HASH
#define wc_InitSha256   wc_InitSha256_sw
#define wc_Sha256Update wc_Sha256Update_sw
#define wc_Sha256Final  wc_Sha256Final_sw
#endif

#ifdef HAVE_FIPS
    /* set NO_WRAPPERS before headers, use direct internal f()s not wrappers */
    #define FIPS_NO_WRAPPERS
#endif

#if defined(USE_INTEL_SPEEDUP)
#define HAVE_INTEL_AVX1
#define HAVE_INTEL_AVX2

#if defined(DEBUG_XMM)
#include "stdio.h"
#endif

#endif

#if defined(HAVE_INTEL_AVX2)
#define HAVE_INTEL_RORX
#endif
 

/*****
Intel AVX1/AVX2 Macro Control Structure

#define HAVE_INTEL_AVX1
#define HAVE_INTEL_AVX2

#define HAVE_INTEL_RORX


int InitSha256(Sha256* sha256) { 
     Save/Recover XMM, YMM
     ...
}

#if defined(HAVE_INTEL_AVX1)|| defined(HAVE_INTEL_AVX2)
  Transform() ; Function prototype 
#else
  Transform() {   }
  int Sha256Final() { 
     Save/Recover XMM, YMM
     ...
  }
#endif

#if defined(HAVE_INTEL_AVX1)|| defined(HAVE_INTEL_AVX2)
    #if defined(HAVE_INTEL_RORX
         #define RND with rorx instuction
    #else
        #define RND
    #endif
#endif

#if defined(HAVE_INTEL_AVX1)
   
   #define XMM Instructions/inline asm
   
   int Transform() {
       Stitched Message Sched/Round
    } 
   
#elif defined(HAVE_INTEL_AVX2)
  
  #define YMM Instructions/inline asm
  
  int Transform() {
      More granural Stitched Message Sched/Round
  }
  
*/


#if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)

/* Each platform needs to query info type 1 from cpuid to see if aesni is
 * supported. Also, let's setup a macro for proper linkage w/o ABI conflicts
 */

#ifndef _MSC_VER
    #define cpuid(reg, leaf, sub)\
            __asm__ __volatile__ ("cpuid":\
             "=a" (reg[0]), "=b" (reg[1]), "=c" (reg[2]), "=d" (reg[3]) :\
             "a" (leaf), "c"(sub));

    #define XASM_LINK(f) asm(f)
#else

    #include <intrin.h>
    #define cpuid(a,b) __cpuid((int*)a,b)

    #define XASM_LINK(f)

#endif /* _MSC_VER */

#define EAX 0
#define EBX 1
#define ECX 2 
#define EDX 3
    
#define CPUID_AVX1   0x1
#define CPUID_AVX2   0x2
#define CPUID_RDRAND 0x4
#define CPUID_RDSEED 0x8
#define CPUID_BMI2   0x10   /* MULX, RORX */

#define IS_INTEL_AVX1       (cpuid_flags&CPUID_AVX1)
#define IS_INTEL_AVX2       (cpuid_flags&CPUID_AVX2)
#define IS_INTEL_BMI2       (cpuid_flags&CPUID_BMI2)
#define IS_INTEL_RDRAND     (cpuid_flags&CPUID_RDRAND)
#define IS_INTEL_RDSEED     (cpuid_flags&CPUID_RDSEED)

static word32 cpuid_check = 0 ;
static word32 cpuid_flags = 0 ;

static word32 cpuid_flag(word32 leaf, word32 sub, word32 num, word32 bit) {
    int got_intel_cpu=0;
    unsigned int reg[5]; 
    
    reg[4] = '\0' ;
    cpuid(reg, 0, 0);  
    if(memcmp((char *)&(reg[EBX]), "Genu", 4) == 0 &&  
                memcmp((char *)&(reg[EDX]), "ineI", 4) == 0 &&  
                memcmp((char *)&(reg[ECX]), "ntel", 4) == 0) {  
        got_intel_cpu = 1;  
    }    
    if (got_intel_cpu) {
        cpuid(reg, leaf, sub);
        return((reg[num]>>bit)&0x1) ;
    }
    return 0 ;
}

static int set_cpuid_flags(void) {  
    if(cpuid_check==0) {
        if(cpuid_flag(1, 0, ECX, 28)){ cpuid_flags |= CPUID_AVX1 ;}
        if(cpuid_flag(7, 0, EBX, 5)){  cpuid_flags |= CPUID_AVX2 ; }
        if(cpuid_flag(7, 0, EBX, 8)) { cpuid_flags |= CPUID_BMI2 ; }
        if(cpuid_flag(1, 0, ECX, 30)){ cpuid_flags |= CPUID_RDRAND ;  } 
        if(cpuid_flag(7, 0, EBX, 18)){ cpuid_flags |= CPUID_RDSEED ;  }
        cpuid_check = 1 ;
        return 0 ;
    }
    return 1 ;
}


/* #if defined(HAVE_INTEL_AVX1/2) at the tail of sha512 */
static int Transform(Sha256* sha256);

#if defined(HAVE_INTEL_AVX1)
static int Transform_AVX1(Sha256 *sha256) ;
#endif
#if defined(HAVE_INTEL_AVX2)
static int Transform_AVX2(Sha256 *sha256) ; 
static int Transform_AVX1_RORX(Sha256 *sha256) ; 
#endif

static int (*Transform_p)(Sha256* sha256) /* = _Transform */;

#define XTRANSFORM(sha256, B)  (*Transform_p)(sha256)

static void set_Transform(void) {
     if(set_cpuid_flags())return ;

#if defined(HAVE_INTEL_AVX2)
     if(IS_INTEL_AVX2 && IS_INTEL_BMI2){ 
         Transform_p = Transform_AVX1_RORX; return ; 
         Transform_p = Transform_AVX2      ; 
                  /* for avoiding warning,"not used" */
     }
#endif
#if defined(HAVE_INTEL_AVX1)
     Transform_p = ((IS_INTEL_AVX1) ? Transform_AVX1 : Transform) ; return ;
#endif
     Transform_p = Transform ; return ;
}

#else
   #if defined(FREESCALE_MMCAU)
      #define XTRANSFORM(sha256, B) Transform(sha256, B)
   #else
      #define XTRANSFORM(sha256, B) Transform(sha256)
   #endif
#endif

/* Dummy for saving MM_REGs on behalf of Transform */
#if defined(HAVE_INTEL_AVX2)&& !defined(HAVE_INTEL_AVX1)
#define  SAVE_XMM_YMM   __asm__ volatile("or %%r8d, %%r8d":::\
  "%ymm4","%ymm5","%ymm6","%ymm7","%ymm8","%ymm9","%ymm10","%ymm11","%ymm12","%ymm13","%ymm14","%ymm15")
#elif defined(HAVE_INTEL_AVX1)
#define  SAVE_XMM_YMM   __asm__ volatile("or %%r8d, %%r8d":::\
    "xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7","xmm8","xmm9","xmm10",\
    "xmm11","xmm12","xmm13","xmm14","xmm15")
#else
#define  SAVE_XMM_YMM
#endif

#ifdef WOLFSSL_PIC32MZ_HASH
#define InitSha256   InitSha256_sw
#define Sha256Update Sha256Update_sw
#define Sha256Final  Sha256Final_sw
#endif

#include "logging.h"
#include "error-crypt.h"

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #include "misc.inline"
#endif

#ifdef FREESCALE_MMCAU
    #include "cau_api.h"
#endif

#ifndef WOLFSSL_HAVE_MIN
#define WOLFSSL_HAVE_MIN

    static INLINE word32 min(word32 a, word32 b)
    {
        return a > b ? b : a;
    }

#endif /* WOLFSSL_HAVE_MIN */


int wc_InitSha256(Sha256* sha256)
{
    #ifdef FREESCALE_MMCAU
        cau_sha256_initialize_output(sha256->digest);
    #else
        sha256->digest[0] = 0x6A09E667L;
        sha256->digest[1] = 0xBB67AE85L;
        sha256->digest[2] = 0x3C6EF372L;
        sha256->digest[3] = 0xA54FF53AL;
        sha256->digest[4] = 0x510E527FL;
        sha256->digest[5] = 0x9B05688CL;
        sha256->digest[6] = 0x1F83D9ABL;
        sha256->digest[7] = 0x5BE0CD19L;
    #endif

    sha256->buffLen = 0;
    sha256->loLen   = 0;
    sha256->hiLen   = 0;
    
#if defined(HAVE_INTEL_AVX1)|| defined(HAVE_INTEL_AVX2)
    set_Transform() ; /* choose best Transform function under this runtime environment */
#endif

    return 0;
}


#if !defined(FREESCALE_MMCAU)
static const ALIGN32 word32 K[64] = {
    0x428A2F98L, 0x71374491L, 0xB5C0FBCFL, 0xE9B5DBA5L, 0x3956C25BL,
    0x59F111F1L, 0x923F82A4L, 0xAB1C5ED5L, 0xD807AA98L, 0x12835B01L,
    0x243185BEL, 0x550C7DC3L, 0x72BE5D74L, 0x80DEB1FEL, 0x9BDC06A7L,
    0xC19BF174L, 0xE49B69C1L, 0xEFBE4786L, 0x0FC19DC6L, 0x240CA1CCL,
    0x2DE92C6FL, 0x4A7484AAL, 0x5CB0A9DCL, 0x76F988DAL, 0x983E5152L,
    0xA831C66DL, 0xB00327C8L, 0xBF597FC7L, 0xC6E00BF3L, 0xD5A79147L,
    0x06CA6351L, 0x14292967L, 0x27B70A85L, 0x2E1B2138L, 0x4D2C6DFCL,
    0x53380D13L, 0x650A7354L, 0x766A0ABBL, 0x81C2C92EL, 0x92722C85L,
    0xA2BFE8A1L, 0xA81A664BL, 0xC24B8B70L, 0xC76C51A3L, 0xD192E819L,
    0xD6990624L, 0xF40E3585L, 0x106AA070L, 0x19A4C116L, 0x1E376C08L,
    0x2748774CL, 0x34B0BCB5L, 0x391C0CB3L, 0x4ED8AA4AL, 0x5B9CCA4FL,
    0x682E6FF3L, 0x748F82EEL, 0x78A5636FL, 0x84C87814L, 0x8CC70208L,
    0x90BEFFFAL, 0xA4506CEBL, 0xBEF9A3F7L, 0xC67178F2L
};

#endif

#if defined(FREESCALE_MMCAU)

static int Transform(Sha256* sha256, byte* buf)
{
    cau_sha256_hash_n(buf, 1, sha256->digest);

    return 0;
}

#endif /* FREESCALE_MMCAU */

#define Ch(x,y,z)       ((z) ^ ((x) & ((y) ^ (z))))
#define Maj(x,y,z)      ((((x) | (y)) & (z)) | ((x) & (y)))
#define R(x, n)         (((x)&0xFFFFFFFFU)>>(n))

#define S(x, n)         rotrFixed(x, n)
#define Sigma0(x)       (S(x, 2) ^ S(x, 13) ^ S(x, 22))
#define Sigma1(x)       (S(x, 6) ^ S(x, 11) ^ S(x, 25))
#define Gamma0(x)       (S(x, 7) ^ S(x, 18) ^ R(x, 3))
#define Gamma1(x)       (S(x, 17) ^ S(x, 19) ^ R(x, 10))

#define RND(a,b,c,d,e,f,g,h,i) \
     t0 = (h) + Sigma1((e)) + Ch((e), (f), (g)) + K[(i)] + W[(i)]; \
     t1 = Sigma0((a)) + Maj((a), (b), (c)); \
     (d) += t0; \
     (h)  = t0 + t1;

#if !defined(FREESCALE_MMCAU)
static int Transform(Sha256* sha256)
{
    word32 S[8], t0, t1;
    int i;

#ifdef WOLFSSL_SMALL_STACK
    word32* W;

    W = (word32*) XMALLOC(sizeof(word32) * 64, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (W == NULL)
        return MEMORY_E;
#else
    word32 W[64];
#endif

    /* Copy context->state[] to working vars */
    for (i = 0; i < 8; i++)
        S[i] = sha256->digest[i];

    for (i = 0; i < 16; i++)
        W[i] = sha256->buffer[i];

    for (i = 16; i < 64; i++)
        W[i] = Gamma1(W[i-2]) + W[i-7] + Gamma0(W[i-15]) + W[i-16];

    for (i = 0; i < 64; i += 8) {
        RND(S[0],S[1],S[2],S[3],S[4],S[5],S[6],S[7],i+0);
        RND(S[7],S[0],S[1],S[2],S[3],S[4],S[5],S[6],i+1);
        RND(S[6],S[7],S[0],S[1],S[2],S[3],S[4],S[5],i+2);
        RND(S[5],S[6],S[7],S[0],S[1],S[2],S[3],S[4],i+3);
        RND(S[4],S[5],S[6],S[7],S[0],S[1],S[2],S[3],i+4);
        RND(S[3],S[4],S[5],S[6],S[7],S[0],S[1],S[2],i+5);
        RND(S[2],S[3],S[4],S[5],S[6],S[7],S[0],S[1],i+6);
        RND(S[1],S[2],S[3],S[4],S[5],S[6],S[7],S[0],i+7);
    }

    /* Add the working vars back into digest state[] */
    for (i = 0; i < 8; i++) {
        sha256->digest[i] += S[i];
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(W, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return 0;
}

#endif /* #if !defined(FREESCALE_MMCAU) */

static INLINE void AddLength(Sha256* sha256, word32 len)
{
    word32 tmp = sha256->loLen;
    if ( (sha256->loLen += len) < tmp)
        sha256->hiLen++;                       /* carry low to high */
}

int wc_Sha256Update(Sha256* sha256, const byte* data, word32 len)
{

    /* do block size increments */
    byte* local = (byte*)sha256->buffer;

    SAVE_XMM_YMM ; /* for Intel AVX */

    while (len) {
        word32 add = min(len, SHA256_BLOCK_SIZE - sha256->buffLen);
        XMEMCPY(&local[sha256->buffLen], data, add);

        sha256->buffLen += add;
        data            += add;
        len             -= add;

        if (sha256->buffLen == SHA256_BLOCK_SIZE) {
            int ret;

            #if defined(LITTLE_ENDIAN_ORDER) && !defined(FREESCALE_MMCAU)
                #if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
                if(!IS_INTEL_AVX1 && !IS_INTEL_AVX2)
                #endif
                ByteReverseWords(sha256->buffer, sha256->buffer,
                                 SHA256_BLOCK_SIZE);
            #endif
            ret = XTRANSFORM(sha256, local);
            if (ret != 0)
                return ret;

            AddLength(sha256, SHA256_BLOCK_SIZE);
            sha256->buffLen = 0;
        }
    }

    return 0;
}

int wc_Sha256Final(Sha256* sha256, byte* hash)
{
    byte* local = (byte*)sha256->buffer;
    int ret;
    
    SAVE_XMM_YMM ; /* for Intel AVX */

    AddLength(sha256, sha256->buffLen);  /* before adding pads */

    local[sha256->buffLen++] = 0x80;     /* add 1 */

    /* pad with zeros */
    if (sha256->buffLen > SHA256_PAD_SIZE) {
        XMEMSET(&local[sha256->buffLen], 0, SHA256_BLOCK_SIZE - sha256->buffLen);
        sha256->buffLen += SHA256_BLOCK_SIZE - sha256->buffLen;

        #if defined(LITTLE_ENDIAN_ORDER) && !defined(FREESCALE_MMCAU)
            #if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
            if(!IS_INTEL_AVX1 && !IS_INTEL_AVX2)
            #endif
            ByteReverseWords(sha256->buffer, sha256->buffer, SHA256_BLOCK_SIZE);
        #endif

        ret = XTRANSFORM(sha256, local);
        if (ret != 0)
            return ret;

        sha256->buffLen = 0;
    }
    XMEMSET(&local[sha256->buffLen], 0, SHA256_PAD_SIZE - sha256->buffLen);

    /* put lengths in bits */
    sha256->hiLen = (sha256->loLen >> (8*sizeof(sha256->loLen) - 3)) +
                 (sha256->hiLen << 3);
    sha256->loLen = sha256->loLen << 3;

    /* store lengths */
    #if defined(LITTLE_ENDIAN_ORDER) && !defined(FREESCALE_MMCAU)
        #if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
        if(!IS_INTEL_AVX1 && !IS_INTEL_AVX2)
        #endif
            ByteReverseWords(sha256->buffer, sha256->buffer, SHA256_BLOCK_SIZE);
    #endif
    /* ! length ordering dependent on digest endian type ! */
    XMEMCPY(&local[SHA256_PAD_SIZE], &sha256->hiLen, sizeof(word32));
    XMEMCPY(&local[SHA256_PAD_SIZE + sizeof(word32)], &sha256->loLen,
            sizeof(word32));

    #if defined(FREESCALE_MMCAU) || defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
        /* Kinetis requires only these bytes reversed */
        #if defined(HAVE_INTEL_AVX1) || defined(HAVE_INTEL_AVX2)
        if(IS_INTEL_AVX1 || IS_INTEL_AVX2)
        #endif
        ByteReverseWords(&sha256->buffer[SHA256_PAD_SIZE/sizeof(word32)],
                         &sha256->buffer[SHA256_PAD_SIZE/sizeof(word32)],
                         2 * sizeof(word32));
    #endif

    ret = XTRANSFORM(sha256, local);
    if (ret != 0)
        return ret;

    #if defined(LITTLE_ENDIAN_ORDER)
        ByteReverseWords(sha256->digest, sha256->digest, SHA256_DIGEST_SIZE);
    #endif
    XMEMCPY(hash, sha256->digest, SHA256_DIGEST_SIZE);

    return wc_InitSha256(sha256);  /* reset state */
}



int wc_Sha256Hash(const byte* data, word32 len, byte* hash)
{
    int ret = 0;
#ifdef WOLFSSL_SMALL_STACK
    Sha256* sha256;
#else
    Sha256 sha256[1];
#endif

#ifdef WOLFSSL_SMALL_STACK
    sha256 = (Sha256*)XMALLOC(sizeof(Sha256), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (sha256 == NULL)
        return MEMORY_E;
#endif

    if ((ret = wc_InitSha256(sha256)) != 0) {
        WOLFSSL_MSG("InitSha256 failed");
    }
    else if ((ret = wc_Sha256Update(sha256, data, len)) != 0) {
        WOLFSSL_MSG("Sha256Update failed");
    }
    else if ((ret = wc_Sha256Final(sha256, hash)) != 0) {
        WOLFSSL_MSG("Sha256Final failed");
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(sha256, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

#endif   /* HAVE_FIPS */

#endif   /* WOLFSSL_TI_HAHS */

#endif /* NO_SHA256 */

