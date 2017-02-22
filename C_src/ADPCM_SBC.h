/*#########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding 
#  
#  Mohit Dandekar
#  John O'Callaghan
#########################################################################*/

#ifndef __ADPCM_SBC_H__
#define __ADPCM_SBC_H__

#include "common.h"
#include "SBC.h"
#include "ADPCM.h"
#include "BitPacker.h"



typedef struct ADPCM_SBCoder_t 
{
/*                                         ----------
              |------->|coder band0|----->|          |
         -----|------->|coder band1|----->|          |
   ---->| AFB |------->|coder band2|----->| BitPacker|------>          
         -----|------->|coder band4|----->|          |
              |------->|coder band3|----->|          |
                                           ----------
*/

   SBC_AFB_t SBC_AFB;  /*Instantiate the analysis filterbank object*/

   ADPCM_enc_t ADPCM_ENC_band0;/*instantiate the coder band0*/
   ADPCM_enc_t ADPCM_ENC_band1;/*instantiate the coder band1*/
   ADPCM_enc_t ADPCM_ENC_band2;/*instantiate the coder band2*/
   ADPCM_enc_t ADPCM_ENC_band3;/*instantiate the coder band3*/
   ADPCM_enc_t ADPCM_ENC_band4;/*instantiate the coder band4*/
   
   BitPack_t BitPack; /*instantiate the bitpacker*/

   aud_data_t *inp_buffer; 

   sint32 blk_len; 
}ADPCM_SBCoder_t;

void ADPCM_SBCoder_init( ADPCM_SBCoder_t *ADPCM_SBCoder_obj,
                         bitstream *bitsout_L,
                         bitstream *bitsout_R,
                         sint32 inp_block_len);

void ADPCM_SBCoder_process( ADPCM_SBCoder_t *ADPCM_SBCoder_obj);



typedef struct ADPCM_SBDecoder_t 
{

/* 
        -------------                             ----                    
       |             |----->|decoder band0|----->|    \
       |             |----->|decoder band1|----->|     \
  ---->| BitDepacker |----->|decoder band2|----->| SFB  ------>          
       |             |----->|decoder band4|----->|     /
       |             |----->|decoder band3|----->|    /
        -------------                             ----            
*/

   SBC_SFB_t SBC_SFB;

   ADPCM_dec_t ADPCM_DEC_band0;
   ADPCM_dec_t ADPCM_DEC_band1;
   ADPCM_dec_t ADPCM_DEC_band2;
   ADPCM_dec_t ADPCM_DEC_band3;
   ADPCM_dec_t ADPCM_DEC_band4;

   BitDepack_t BitDepack;

   bitstream *inp_buffer_L; 
   bitstream *inp_buffer_R; 

   sint32 blk_len; 
}ADPCM_SBDecoder_t;

void ADPCM_SBDecoder_init(ADPCM_SBDecoder_t *ADPCM_SBDecoder_obj,
                          aud_data_t *data_out_ptr,
                          sint32 out_block_len);

void ADPCM_SBDecoder_process( ADPCM_SBDecoder_t *ADPCM_SBDecoder_obj);




#endif /*__ADPCM_SBC_H__*/
