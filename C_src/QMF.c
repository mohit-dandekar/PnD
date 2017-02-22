/*#########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding 
#  
#  Mohit Dandekar
#  John O'Callaghan
#########################################################################*/

#include "QMF.h"

void QMFA_bank_init( QMFA_bank_t *QMFA_bank_obj, 
                     aud_data_t *data_out_LO,
                     aud_data_t *data_out_HI,
                     sint16 *filter,
                     sint32 block_len )
{
  sint32 i,N;
  aud_data_t *dptr, *dptr0, *dptr1;

  /* Connecting the output pointers to out buffers*/
  QMFA_bank_obj->data_out_LO_ptr = data_out_LO;
  QMFA_bank_obj->data_out_HI_ptr = data_out_HI;

  /* Connecting the filter pointer to the filter array*/
  QMFA_bank_obj->flt_ptr = filter;

  /*Copying Block length*/
  QMFA_bank_obj->blk_len = block_len;


  N = BLOCK_SIZE ; 
  /*Initializing the input data buffer array to 0*/
  dptr = QMFA_bank_obj->data_in_buffer;
  for(i=0;i<N;++i)
  {
    dptr[i].l = 0; 
    dptr[i].r = 0; 
  }

  N = BLOCK_SIZE/2 + FILT_LEN/2-1;
  /*Initializing histrory arrays to 0*/ 
  dptr0 = QMFA_bank_obj->history[0];
  dptr1 = QMFA_bank_obj->history[1];
  for(i=0;i<N;++i)
  {
    dptr0[i].l = 0;
    dptr0[i].r = 0;
    dptr1[i].l = 0;
    dptr1[i].r = 0;
  }

  return;
}

void QMFA_process(QMFA_bank_t *QMFA_bank_obj)
{
  sint32 i,j;  

  sint16 *filter = QMFA_bank_obj->flt_ptr;
  aud_data_t *data_ptr0 = QMFA_bank_obj->data_in_buffer + 0; /*input at location 0 (for even sample access)*/
  aud_data_t *data_ptr1 = QMFA_bank_obj->data_in_buffer + 1; /*input at location 1 (for odd sample access)*/

  sint32 L = QMFA_bank_obj->blk_len/2;
  sint32 M = FILT_LEN/2;
  aud_data_t *hist_rd_ptr0 = QMFA_bank_obj->history[0]; 
  aud_data_t *hist_rd_ptr1 = QMFA_bank_obj->history[1];
  aud_data_t *hist_wr_ptr0 = QMFA_bank_obj->history[0];
  aud_data_t *hist_wr_ptr1 = QMFA_bank_obj->history[1];

  aud_data_t *data_out_lo_ptr = QMFA_bank_obj->data_out_LO_ptr;
  aud_data_t *data_out_hi_ptr = QMFA_bank_obj->data_out_HI_ptr;

  for(i=0;i<L;++i)
  {
    /*Iterations 0 to (L-1) produce the current block of output*/

    aud_data_t o0,o1,out;
    aud_data_w_t accum0,accum1; /*accumulators for polyphase filter commponent 0 and 1*/
    accum0.l = 0; accum0.r = 0;
    accum1.l = 0; accum1.r = 0;

    for(j=0; ((j<=i)&&(j<M)); j++) 
    {
     /* Limit for j is beginning at the rightmost overlap 
        position to the leftmost overlap position*/
      aud_data_t d0,d1;

      sint16 filt_coef0 = filter[2*j+1];  /*Loads the decimated filter coeficients*/
      sint16 filt_coef1 = filter[2*j+0];  /*in the correct order i.e. with reversal*/
  
      d0 = data_ptr0[2*(i-j)]; /*load the data right to left with 1:2 split*/ 
      d1 = data_ptr1[2*(i-j)]; 

      DSMULA(accum0, d0 ,filt_coef0); /*MAC for polyphase filter 0*/   
      DSMULA(accum1, d1 ,filt_coef1); /*MAC for polyphase filter 1*/  
    }
    DPACKV(o0,accum0,15,15);
    DPACKV(o1,accum1,15,15);
    DADD(o0, o0, hist_rd_ptr0[i]); /*add history sample filter 0 overlap add*/
    DADD(o1, o1, hist_rd_ptr1[i]); /*add history sample filter 1 overlap add*/
    DADD(out,o0,o1); /*out bank0 is out0 + out1*/
    data_out_lo_ptr[i] = out;
    DSUB(out,o0,o1); /*out bank1 is out0 - out1*/
    data_out_hi_ptr[i] = out;
  }

  for(i=L;i<L+M-1;++i)
  {

    /*Iterations L to (L + (M-1) -1) produce the 
      history for overlap add for next block*/

    aud_data_t o0,o1;
    aud_data_w_t accum0,accum1;
    accum0.l = 0; accum0.r = 0;
    accum1.l = 0; accum1.r = 0;
    for(j=(i-L+1); ((j<=i)&&(j<M)); j++) 
    {
     /*same logic for j starting at the 
       rightmost point of overlap to leftmost*/

      aud_data_t d0,d1;

      sint16 filt_coef0 = filter[2*j+1];  
      sint16 filt_coef1 = filter[2*j+0];
  
      d0 = data_ptr0[2*(i-j)]; 
      d1 = data_ptr1[2*(i-j)]; 

      DSMULA(accum0, d0 ,filt_coef0);    
      DSMULA(accum1, d1 ,filt_coef1);    
    }
    DPACKV(o0,accum0,15,15);
    DPACKV(o1,accum1,15,15);
    DADD(o0, o0, hist_rd_ptr0[i]);
    DADD(o1, o1, hist_rd_ptr1[i]);
    hist_wr_ptr0[i-L] = o0; /* write out overlap add history history filter 0*/
    hist_wr_ptr1[i-L] = o1; /* write out overlap add history history filter 1*/
  }

  return;
}



void QMFS_bank_init( QMFS_bank_t *QMFS_bank_obj, 
                     aud_data_t *data_out,
                     sint16 *filter,
                     sint32 block_len)
{
  sint32 i;
  aud_data_t *dptr0, *dptr1;

  /*Connecting output pointers to output buffers*/
  QMFS_bank_obj->data_out_ptr = data_out;
 
  /*Connecting filter pointer to filter*/
  QMFS_bank_obj->flt_ptr = filter;

  QMFS_bank_obj->blk_len = block_len;

  /*Initializing input buffer arrays to 0*/
  dptr0 = QMFS_bank_obj->data_in_buffer_LO;
  dptr1 = QMFS_bank_obj->data_in_buffer_HI;
  for(i=0;i<BLOCK_SIZE/2;++i)
  {
    dptr0[i].l = 0;
    dptr0[i].r = 0;
    dptr1[i].l = 0;
    dptr1[i].r = 0;
  } 

  /*Initializing overlap add history arrays to 0*/
  dptr0 = QMFS_bank_obj->history[0];
  dptr1 = QMFS_bank_obj->history[1];
  for(i=0;i<BLOCK_SIZE/2+FILT_LEN/2-1;++i)
  {
    dptr0[i].l = 0;
    dptr0[i].r = 0;
    dptr1[i].l = 0;
    dptr1[i].r = 0;
  }

  return;
}

void QMFS_process(QMFS_bank_t *QMFS_bank_obj)
{
  sint32 i,j;  

  sint16 *filter = QMFS_bank_obj->flt_ptr;
  aud_data_t *data_ptr0 = QMFS_bank_obj->data_in_buffer_LO; /*input buffer bank0*/
  aud_data_t *data_ptr1 = QMFS_bank_obj->data_in_buffer_HI; /*input buffer bank0*/

  sint32 L = QMFS_bank_obj->blk_len;
  sint32 M = FILT_LEN/2;
  aud_data_t *hist_rd_ptr0 = QMFS_bank_obj->history[0];
  aud_data_t *hist_rd_ptr1 = QMFS_bank_obj->history[1];
  aud_data_t *hist_wr_ptr0 = QMFS_bank_obj->history[0];
  aud_data_t *hist_wr_ptr1 = QMFS_bank_obj->history[1];

  aud_data_t *dataout_ptr = QMFS_bank_obj->data_out_ptr;
 
  for(i=0;i<L;++i)
  {
    /* iteration 0 to (L-1) produces the L outputs of the current block*/
    aud_data_t t0,t1,out;
    aud_data_w_t accum0,accum1;
    accum0.l = 0; accum0.r = 0;
    accum1.l = 0; accum1.r = 0;
    for(j=0; ((j<=i)&&(j<M)); j++)
    {
     /*j indexes from the point of rightmost overlap to the leftmost overlap position */
      aud_data_t d0,d1;

      sint16 filt_coef0 = filter[2*j+0];  /*Loads the decimated filter coeficients*/
      sint16 filt_coef1 = filter[2*j+1];  /*in the correct order i.e. with reversal*/
  
      d0 = data_ptr0[(i-j)]; /*load the data right to left with 1:2 split*/ 
      d1 = data_ptr1[(i-j)]; 

      DADD(t0,d0,d1);  /*input to filter 0 is insample0-insample1*/
      DSUB(t1,d0,d1);  /*input to filter 1 is insample0+insample1*/

      DSMULA(accum0, t0 ,filt_coef0);    
      DSMULA(accum1, t1 ,filt_coef1);    
    }
    DPACKV(t0,accum0,15,15);
    DPACKV(t1,accum1,15,15);
    DADD(out, t0, hist_rd_ptr0[i]);
    dataout_ptr[2*i + 0] = out;      /*filter 0 produces the odd output*/
    DADD(out, t1, hist_rd_ptr1[i]);  
    dataout_ptr[2*i + 1] = out;      /*filter 1 produces the even output*/
  }
 
  for(i=L;i<L+M-1;++i)
  {
    aud_data_t t0,t1,out;
    aud_data_w_t accum0,accum1;
    accum0.l = 0; accum0.r = 0;
    accum1.l = 0; accum1.r = 0;
    for(j=(i-L+1); ((j<=i)&&(j<M)); j++)
    {

      /*same logic for j starting at the 
       rightmost point of overlap to leftmost*/
      aud_data_t d0,d1;

      sint16 filt_coef0 = filter[2*j+0];  
      sint16 filt_coef1 = filter[2*j+1];
  
      d0 = data_ptr0[(i-j)]; 
      d1 = data_ptr1[(i-j)]; 

      DADD(t0,d0,d1);
      DSUB(t1,d0,d1);

      DSMULA(accum0, t0 ,filt_coef0);    
      DSMULA(accum1, t1 ,filt_coef1);    
    }
    DPACKV(t0,accum0,15,15);
    DPACKV(t1,accum1,15,15);
    DADD(out, t0, hist_rd_ptr0[i]);
    hist_wr_ptr0[i-L] = out;         /* write out overlap add history history filter 0*/
    DADD(out, t1, hist_rd_ptr1[i]);  
    hist_wr_ptr1[i-L] = out;         /* write out overlap add history history filter 1*/
  }

  return;
}

