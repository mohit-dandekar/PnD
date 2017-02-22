/*#########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding 
#  
#  Mohit Dandekar
#  John O'Callaghan
#########################################################################*/

#ifndef __QMF_H__
#define __QMF_H__

#include "common.h"

/* Template of QMF bank object*/
typedef struct QMFA_bank_t
{
  /*scratch/history buffer*/
  aud_data_t history[2][BLOCK_SIZE/2 + FILT_LEN/2 -1]; 

  /*Input Buffer to be written into by the previous stage*/
  aud_data_t data_in_buffer[BLOCK_SIZE]; 
  /*TODO : QMF at increasing depth in the tree have smaller block size, but the
           memory allocation is fixed to worse case
  */ 

  aud_data_t *data_out_LO_ptr; /* pointer to data out LO branch*/
  aud_data_t *data_out_HI_ptr; /* pointer to data out HI branch*/

  sint16 *flt_ptr; /*pointer to filter*/

  sint32 blk_len;
}QMFA_bank_t;


void QMFA_bank_init( QMFA_bank_t *QMFA_bank_obj, 
                     aud_data_t *data_out_LO, 
                     aud_data_t *data_out_HI,
                     sint16 *filter,
                     sint32 block_len );


void QMFA_process(QMFA_bank_t *QMFA_bank_obj);



/* Template of QMF bank object*/
typedef struct QMFS_bank_t
{
  /*scratch/history buffer*/
  aud_data_t history[2][BLOCK_SIZE/2 + FILT_LEN/2 -1]; 

  /*Input Buffer to be written into by the previous stage*/
  aud_data_t data_in_buffer_LO[BLOCK_SIZE/2]; 

  aud_data_t data_in_buffer_HI[BLOCK_SIZE/2]; 
  /*TODO : QMF at increasing depth in the tree have smaller block size, but the
           memory allocation is fixed to worse case
  */ 

  aud_data_t *data_out_ptr; /* pointer to data out */

  sint16 *flt_ptr; /*pointer to filter*/

  sint32 blk_len;
}QMFS_bank_t;


void QMFS_bank_init( QMFS_bank_t *QMFS_bank_obj, 
                     aud_data_t *data_out,
                     sint16 *filter,
                     sint32 block_len);

void QMFS_process(QMFS_bank_t *QMFS_bank_obj);

#endif /*__QMF_H__*/
