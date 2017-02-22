/*#########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding 
#  
#  Mohit Dandekar
#  John O'Callaghan
#########################################################################*/


#ifndef __BITPACKER_H__
#define __BITPACKER_H__

#include "common.h"

/* Template for packer object */
typedef struct BitPack_t
{

/*
                 index(nbit)
                 
                  --------       -------------
  bank0          |  0(5)  |---->|             |
                  --------      |             | 
                  --------      |             |
  bank1          |  1(5)  |---->|             |
                  --------      |             |     <-- MSB -------------  24 bits --------------- LSB -->
         -----------------      |             |     -----------------------------------------------------
  bank2 | 2_1(4) | 2_0(4) |---->|  BitPacker  |--->| 2_1(4) | 2_0(4) |3_1(3)|   1(5)   |3_0(3)|   0(5)   |
         -----------------      |             |     -----------------------------------------------------
         -----------------      |             |
  bank3 | 3_1(3) | 3_0(3) |---->|             |
         -----------------      |             |
         -----------------      |             |
  bank4 |  empty subband  |---->|             |
         -----------------       -------------


*/



  /* Inputs from each sub-band */
  adpcm_data_t inp_band0[BLOCK_SIZE/8];
  adpcm_data_t inp_band1[BLOCK_SIZE/8];

  adpcm_data_t inp_band2[BLOCK_SIZE/4];
  adpcm_data_t inp_band3[BLOCK_SIZE/4];
  adpcm_data_t inp_band4[BLOCK_SIZE/4];

  /* Delay buffers for adpcm data (bank 2) */
  adpcm_data_t delay_buffer2[BLOCK_SIZE/4 + FILT_LEN-2];
  sint16 delay_buffer2_rd, delay_buffer2_wr, delay_buffer2_lim;

  /* Delay buffers for adpcm data (bank 3) */
  adpcm_data_t delay_buffer3[BLOCK_SIZE/4 + FILT_LEN-2];
  sint16 delay_buffer3_rd, delay_buffer3_wr, delay_buffer3_lim;

  bitstream *out_ptr_L;
  bitstream *out_ptr_R;
}BitPack_t;

void BitPacker_init(BitPack_t *BitPack_obj,
                    bitstream *bitsout_L,
                    bitstream *bitsout_R);


void BitPacker_process(BitPack_t *BitPack_obj);

/* Template for depacker object */
typedef struct BitDepack_t
{

/*
                                                                                  index(nbit)
                                                                                  
                                                                 -------------               --------               
                                                                |             |------------>|  0(5)  |----> bank0 
                                                                |             |              --------             
                                                                |             |              --------             
                                                                |             |------------>|  1(5)  |----> bank1 
     <-- MSB -------------  24 bits --------------- LSB -->     |             |              --------             
     -----------------------------------------------------      |             |     -----------------             
--->| 2_1(4) | 2_0(4) |3_1(3)|   1(5)   |3_0(3)|   0(5)   |---->| BitDepacker |--->| 2_1(4) | 2_0(4) |----> bank2 
     -----------------------------------------------------      |             |     -----------------             
                                                                |             |     -----------------             
                                                                |             |--->| 3_1(3) | 3_0(3) |----> bank3 
                                                                |             |     -----------------             
                                                                |             |     -----------------             
                                                                |             |--->|  empty subband  |----> bank4 
                                                                 -------------      -----------------             


*/

  /* Input buffers for packed data */
  bitstream inp_buffer0[3*BLOCK_SIZE/8];
  bitstream inp_buffer1[3*BLOCK_SIZE/8];

  /* Pointers for ADPCM output of each sub-band */
  adpcm_data_t *out_band0_ptr  ;
  adpcm_data_t *out_band1_ptr  ;
  adpcm_data_t *out_band2_ptr  ;
  adpcm_data_t *out_band3_ptr  ;
  adpcm_data_t *out_band4_ptr  ;

}BitDepack_t;

void BitDepacker_init( BitDepack_t *BitDepack_obj,
                       adpcm_data_t *band0_out,
                       adpcm_data_t *band1_out,
                       adpcm_data_t *band2_out,
                       adpcm_data_t *band3_out,
                       adpcm_data_t *band4_out);

void BitDepacker_process(BitDepack_t *BitDepack_obj);


#endif /*__BITPACKER_H__*/
