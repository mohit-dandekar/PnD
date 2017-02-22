/*#########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding 
#  
#  Mohit Dandekar
#  John O'Callaghan
#########################################################################*/
#ifndef ADPCM_H_INCLUDED
#define ADPCM_H_INCLUDED

#include "common.h"


/* Template for ADPCM encoder object */

typedef struct ADPCM_enc_t {

      /* chunk variables */
      aud_data_t step;
      aud_data_t x_star;
      adpcm_data_t z_N;

      /* input buffer */
      aud_data_t data_in_buffer[BLOCK_SIZE/4];
      /* NOTE: In 5 band implementation, encoder input data
       will be BLOCK_SIZE/8 in 2 bands, and
       BLOCK_SIZE/4 in 3. Setting size here to worst/largest case */

      sint16 mu;
      sint16 nbit;
      sint16 blk_len;

      sint16 *stepTable_ptr; /* pointer to stepTable */

      adpcm_data_t *data_out_ptr; /* pointer to data out */
}ADPCM_enc_t;

void ADPCM_enc_init( ADPCM_enc_t *ADPCM_enc_obj,
                     adpcm_data_t *data_out,
                     sint16 mu,
                     sint8 nbit,
                     sint16 blk_len);

void ADPCM_enc_process( ADPCM_enc_t *ADPCM_enc_obj);

/* Template for ADPCM decoder object */
typedef struct ADPCM_dec_t {

      /* chunk variables */
      aud_data_t step;
      aud_data_t z_N;
      aud_data_t x_star;

      /* ADPCM input buffer */
      adpcm_data_t data_in_buffer[BLOCK_SIZE/4];
      /* NOTE: In 5 band implementation, encoder data in
       will be BLOCK_SIZE/8 in 2 bands, and
       BLOCK_SIZE/4 in 3. Setting size here to worst/largest case */

      sint16 mu;
      sint8 nbit;
      sint16 blk_len;

      sint16 *stepTable_ptr; /*pointer to stepTable*/

      aud_data_t *data_out_ptr; /* pointer to data out */
}ADPCM_dec_t;

void ADPCM_dec_init( ADPCM_dec_t *ADPCM_dec_obj,
                     aud_data_t *data_out,
                     sint16 mu,
                     sint8 nbit,
                     sint16 blk_len);

void ADPCM_dec_process(ADPCM_dec_t *ADPCM_dec_obj);


#endif // ADPCM_H_INCLUDED
