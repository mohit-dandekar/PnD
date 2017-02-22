/*#########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding 
#  
#  Mohit Dandekar
#  John O'Callaghan
#########################################################################*/

#include "SBC.h"


void SBC_AFB_init( SBC_AFB_t *SBC_AFB_obj,
                   aud_data_t *out_buffer0_0_0,
                   aud_data_t *out_buffer0_0_1,
                   aud_data_t *out_buffer0_1,
                   aud_data_t *out_buffer1_0,
                   aud_data_t *out_buffer1_1,
                   sint16 *filter,
                   sint32 inp_block_len)
{

  SBC_AFB_obj->inp_blk_len = inp_block_len;

  QMFA_bank_init( &(SBC_AFB_obj->QMF_bank000),
                  out_buffer0_0_0 /* OUT 0 - 500 Hz*/,
                  out_buffer0_0_1 /* OUT 500 - 1000 Hz*/,
                  filter,
                  inp_block_len/4);

  QMFA_bank_init( &(SBC_AFB_obj->QMF_bank00),
                  SBC_AFB_obj->QMF_bank000.data_in_buffer,
                  out_buffer0_1 /* OUT 1000 - 2000 Hz*/,
                  filter,
                  inp_block_len/2);

  QMFA_bank_init( &(SBC_AFB_obj->QMF_bank01),
                  out_buffer1_0 /* OUT 3000 - 4000 Hz*/,
                  out_buffer1_1 /* OUT 2000 - 3000 Hz*/,
                  filter,
                  inp_block_len/2);


  /*Connect outputs of first QMF to next 2 QMF inputs*/
  QMFA_bank_init( &(SBC_AFB_obj->QMF_bank0),
                  SBC_AFB_obj->QMF_bank00.data_in_buffer,
                  SBC_AFB_obj->QMF_bank01.data_in_buffer,
                  filter,
                  inp_block_len);

  /*Connect input to first QMF to input*/
  SBC_AFB_obj->inp_buffer = SBC_AFB_obj->QMF_bank0.data_in_buffer;


  
  return;
}

void SBC_AFB_process( SBC_AFB_t *SBC_AFB_obj)
{

  /* Stage 1 QMF bank*/
  QMFA_process( &(SBC_AFB_obj->QMF_bank0)); 
 
  /* Stage 2 QMF bank*/
  QMFA_process( &(SBC_AFB_obj->QMF_bank00)); 
  QMFA_process( &(SBC_AFB_obj->QMF_bank01)); 

  /* Stage 3 QMF bank*/
  QMFA_process( &(SBC_AFB_obj->QMF_bank000)); 

  return;
}



void SBC_SFB_init( SBC_SFB_t *SBC_SFB_obj,
                   aud_data_t *data_out,
                   sint16 *filter,
                   sint32 out_block_len)
{

  QMFS_bank_init( &(SBC_SFB_obj->QMF_bank0),
                  data_out,
                  filter,
                  out_block_len/2); 

 
  QMFS_bank_init( &(SBC_SFB_obj->QMF_bank00),
                  SBC_SFB_obj->QMF_bank0.data_in_buffer_LO, /*points to input buffer0 of bank0*/
                  filter,
                  out_block_len/4);

  SBC_SFB_obj->in_buffer0_1 = SBC_SFB_obj->QMF_bank00.data_in_buffer_HI;


  QMFS_bank_init( &(SBC_SFB_obj->QMF_bank01),
                  SBC_SFB_obj->QMF_bank0.data_in_buffer_HI, /*points to input buffer1 of bank0*/ 
                  filter,
                  out_block_len/4);

  SBC_SFB_obj->in_buffer1_0 = SBC_SFB_obj->QMF_bank01.data_in_buffer_LO;
  SBC_SFB_obj->in_buffer1_1 = SBC_SFB_obj->QMF_bank01.data_in_buffer_HI;


  QMFS_bank_init( &(SBC_SFB_obj->QMF_bank000),
                  SBC_SFB_obj->QMF_bank00.data_in_buffer_LO, /*points to input buffer0 of bank00*/ 
                  filter,
                  out_block_len/8);

  SBC_SFB_obj->in_buffer0_0_0 = SBC_SFB_obj->QMF_bank000.data_in_buffer_LO;
  SBC_SFB_obj->in_buffer0_0_1 = SBC_SFB_obj->QMF_bank000.data_in_buffer_HI;
 

  return;
}
                 
void SBC_SFB_process( SBC_SFB_t *SBC_SFB_obj)
{

  /* Processing first leaf stage*/
  QMFS_process( &(SBC_SFB_obj->QMF_bank000));

  /* Processing second stage*/
  QMFS_process( &(SBC_SFB_obj->QMF_bank00));
  QMFS_process( &(SBC_SFB_obj->QMF_bank01));

  /*Processing Root stage*/
  QMFS_process( &(SBC_SFB_obj->QMF_bank0));

  return;
}
