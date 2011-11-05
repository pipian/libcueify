/**
 * \file cdtext_crc.h
 * Functions and types for CD-Text CRC checks.
 *
 * Generated on Fri Nov  4 13:39:45 2011,
 * by pycrc v0.7.8, http://www.tty1.net/pycrc/
 * using the configuration:
 *    Width        = 16
 *    Poly         = 0x1021
 *    XorIn        = 0x0000
 *    ReflectIn    = False
 *    XorOut       = 0xffff
 *    ReflectOut   = False
 *    Algorithm    = table-driven
 *****************************************************************************/
#ifndef _LIBCUEIFY_CDTEXT_CRC_H
#define _LIBCUEIFY_CDTEXT_CRC_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * The definition of the used algorithm.
 *****************************************************************************/
#define CRC_ALGO_TABLE_DRIVEN 1


/**
 * The type of the CD-Text CRC values.
 *
 * This type must be big enough to contain at least 16 bits.
 *****************************************************************************/
typedef uint16_t cdtext_crc_t;


/**
 * Calculate the initial CD-Text crc value.
 *
 * \return     The initial crc value.
 *****************************************************************************/
static inline cdtext_crc_t cdtext_crc_init(void)
{
    return 0x0000;
}


/**
 * Update the CD-Text crc value with new data.
 *
 * \param crc      The current crc value.
 * \param data     Pointer to a buffer of \a data_len bytes.
 * \param data_len Number of bytes in the \a data buffer.
 * \return         The updated crc value.
 *****************************************************************************/
cdtext_crc_t cdtext_crc_update(cdtext_crc_t crc, const unsigned char *data,
			       size_t data_len);


/**
 * Calculate the final CD-Text crc value.
 *
 * \param crc  The current crc value.
 * \return     The final crc value.
 *****************************************************************************/
static inline cdtext_crc_t cdtext_crc_finalize(cdtext_crc_t crc)
{
    return crc ^ 0xffff;
}


#ifdef __cplusplus
}           /* closing brace for extern "C" */
#endif

#endif      /* _LIBCUEIFY_CDTEXT_CRC_H */
