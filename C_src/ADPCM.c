/*#########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding
#
#  Mohit Dandekar
#  John O'Callaghan
#########################################################################*/

#include "ADPCM.h"

/* Step multipliers stored in q2.13 format */
sint16 stepTable2[2] = {6553, 13107};
sint16 stepTable3[4] = {7372, 7372, 12288, 22528};
sint16 stepTable4[8] = {7372, 7372, 7372, 7372, 9830, 13107, 16384, 19660};
sint16 stepTable5[16] = {7372, 7372, 7372, 7372, 7782, 7782, 7782, 7782, 9830,
                         12288, 14745, 17203, 19660, 22118, 24576, 27033};

/*(1/x) taylor series approximation terms in range 0.5-1 in q15*/
                       /* q2.13 */
sint16 one_by_x0[32] ={16132, 15650, 15197, 14769, 14364, 13981, 13618, 13273, 
                       12945, 12633, 12336, 12053, 11782, 11523, 11275, 11038,
                       10810, 10592, 10382, 10180, 9986, 9800, 9620, 9447, 
                       9279, 9118, 8962, 8812, 8666, 8525, 8389, 8257};  

                          /* q2.13 */
sint16 one_by_x0sq[32]  = {31768, 29899, 28191, 26625, 25186, 23861, 22637, 21506,
                           20457, 19483, 18577, 17733, 16945, 16208, 15518, 14872, 
                           14265, 13694, 13157, 12651, 12174, 11723, 11297, 10893,
                           10511, 10149, 9805, 9478, 9167, 8872, 8590, 8322};

static void RECIP(sint16 *dout, 
                  sint16 *scale,
                  sint16 din)
{
  sint32 s,indx ;
  sint16 delta;
  sint16 sign = (din > 0)? 0 : 1;
  sint16 xin = abs(din);

  /*Normalize value to 0.5 - 1 in q15*/
  s = 0;
  while((xin << s)<16384)
  {
    s++;
  }
  xin = (xin << s); 
  indx = ((xin - 16384)>>9); /*5 bit index*/
  delta = (xin & ((1<<9)-1)) - (1<<8);/*q15*/ 
 
  *dout = (one_by_x0[indx] - ((one_by_x0sq[indx]*delta + 16384)>>15))*(sign? -1 : 1); /*output in (S+2).(13-S) format*/
  *scale = s;
  return;
}

void ADPCM_enc_init( ADPCM_enc_t *ADPCM_enc_obj,
                     adpcm_data_t *data_out,
                     sint16 mu,
                     sint8 nbit,
                     sint16 blk_len)
{
  /* Connecting output pointer to output buffer */
  ADPCM_enc_obj->data_out_ptr = data_out;

  /* Copying mu, nbit and block length */
  ADPCM_enc_obj->mu = mu;
  ADPCM_enc_obj->nbit = nbit;
  ADPCM_enc_obj->blk_len = blk_len;

  /* Initialising chunk / history variables */
  ADPCM_enc_obj->step.l = 16384;
  ADPCM_enc_obj->step.r = 16384;
  ADPCM_enc_obj->z_N.l = 0;
  ADPCM_enc_obj->z_N.r = 0;
  ADPCM_enc_obj->x_star.l = 0;
  ADPCM_enc_obj->x_star.r = 0;

  /* Connecting stepTable pointer to correct table depending on number of output ADPCM bits */
  if (nbit == 2){
    ADPCM_enc_obj->stepTable_ptr = stepTable2;
  }else if (nbit == 3){
    ADPCM_enc_obj->stepTable_ptr = stepTable3;
  }else if (nbit == 4){
    ADPCM_enc_obj->stepTable_ptr = stepTable4;
  }else if (nbit == 5){
    ADPCM_enc_obj->stepTable_ptr = stepTable5;
  }

return;
}

void ADPCM_enc_process(ADPCM_enc_t *ADPCM_enc_obj)
{
  sint32 bufPos; /* Iterator for position in buffer */

  aud_data_t step, x_star, d, d_dash, inv_step, t0, stepTableVal; /* 16 bit stereo variables */
  aud_data_w_t accum0; /* Accumulator for mul results */
  sint16 indxl,indxr,sl,sr;

  aud_data_t *data_ptr = ADPCM_enc_obj->data_in_buffer; /* Connect pointer for input */
  adpcm_data_t *data_out_ptr = ADPCM_enc_obj->data_out_ptr; /* Connect pointer for adpcm output */

  sint16 *stepTable = ADPCM_enc_obj->stepTable_ptr; /* Connect pointer for stepTable */

  /* Copy required variables from encoder object */
  sint16 nbit = ADPCM_enc_obj->nbit;
  sint16 mu = ADPCM_enc_obj->mu;
  sint16 blk_len = ADPCM_enc_obj->blk_len;

  /* Copy history from chunk variables */
  step = ADPCM_enc_obj->step;
  x_star = ADPCM_enc_obj->x_star;

  /* Calculate initial d' = step * previous quantized output */
  DMUL(accum0,step,ADPCM_enc_obj->z_N);
  DPACKL(d_dash,accum0);

  /* Set initial index for stepTable lookup based on previous quantised output */
  SATN(indxl, abs(ADPCM_enc_obj->z_N.l),(0),((1<<(nbit-1))-1));
  SATN(indxr, abs(ADPCM_enc_obj->z_N.r),(0),((1<<(nbit-1))-1));

  /* If no bits assigned to this bank, return empty data out without further processing */
  if(nbit == 0)
  {
      for(bufPos=0;bufPos<blk_len;++bufPos)
      {
        data_out_ptr[bufPos].l = 0;
        data_out_ptr[bufPos].r = 0;
      }
    return; 
  }

  for(bufPos=0;bufPos<blk_len;++bufPos)
    {
      /* Predictor: x* = mu( x* + d' )    */
      DADD(t0,x_star,d_dash); /* (x* + d') */
      DSMUL(accum0,t0,mu);
      DPACKV(x_star,accum0,15,15); /* Pack result stored in accumulator to Q15 */

      /* Prediction error: d = x - x*     */
      DSUB(d,data_ptr[bufPos],x_star);

      /* Step: step = step * stepTableVal */
      stepTableVal.l = stepTable[indxl];  /* Left Channel step multiplier (Q2.13) */
      stepTableVal.r = stepTable[indxr];  /* Right Channel step multiplier (Q2.13) */ 

      DMUL(accum0,step,stepTableVal); /* step * stepTableVal (Q2.13) */
      DPACKV(step,accum0,13,13);   

      /* Calculate step reciprocal, inverse step */
      RECIP(&(inv_step.l), &sl, step.l); 
      RECIP(&(inv_step.r), &sr, step.r); 

      /* Quantizer: Output = d * inv_step */
      DMUL(accum0,d,inv_step);
      DPACKV(t0,accum0,(15+13-sl),(15+13-sr));

      /* Saturate / limit output depending on nbit */
      SATN(t0.l, t0.l,(-(1<<(nbit-1))),((1<<(nbit-1))-1)); 
      SATN(t0.r, t0.r,(-(1<<(nbit-1))),((1<<(nbit-1))-1));

      /* Write result to output pointer */
      data_out_ptr[bufPos].l = (sint8)t0.l; 
      data_out_ptr[bufPos].r = (sint8)t0.r;

      /* Set index of step multiplier for next iteration based on new quantized output */
      SATN(indxl, abs(t0.l),(0),((1<<(nbit-1))-1));
      SATN(indxr, abs(t0.r),(0),((1<<(nbit-1))-1));

      /* Dequantizer: d' = step * quantized output */
      DMUL(accum0,step,t0);
      DPACKL(d_dash,accum0);
    }

  /* Write history to chunk variables in encoder object */
  ADPCM_enc_obj->step = step;
  ADPCM_enc_obj->z_N.l = (sint8)t0.l;
  ADPCM_enc_obj->z_N.r = (sint8)t0.r;
  ADPCM_enc_obj->x_star = x_star;

  return;
}


void ADPCM_dec_init( ADPCM_dec_t *ADPCM_dec_obj,
                     aud_data_t *data_out,
                     sint16 mu,
                     sint8 nbit,
                     sint16 blk_len)
{
  /* Connecting output pointer to output buffer */
  ADPCM_dec_obj->data_out_ptr = data_out;

  /* Copying mu, nbit and block length */
  ADPCM_dec_obj->mu = mu;
  ADPCM_dec_obj->nbit = nbit;
  ADPCM_dec_obj->blk_len = blk_len;

  /* Initialising chunk / history variables */
  ADPCM_dec_obj->step.l = 16384;
  ADPCM_dec_obj->step.r = 16384;
  ADPCM_dec_obj->z_N.l = 0;
  ADPCM_dec_obj->z_N.r = 0;
  ADPCM_dec_obj->x_star.l = 0;
  ADPCM_dec_obj->x_star.r = 0;

  /* Connecting stepTable pointer to correct table depending on number of output ADPCM bits */
  if (nbit == 2){
    ADPCM_dec_obj->stepTable_ptr = stepTable2;
  }else if (nbit == 3){
    ADPCM_dec_obj->stepTable_ptr = stepTable3;
  }else if (nbit == 4){
    ADPCM_dec_obj->stepTable_ptr = stepTable4;
  }else if (nbit == 5){
    ADPCM_dec_obj->stepTable_ptr = stepTable5;
  }

return;
}


void ADPCM_dec_process(ADPCM_dec_t *ADPCM_dec_obj)
{
  sint32 bufPos; /* Iterator for position in buffer */

  aud_data_t step, x_star, d_dash, t0, stepTableVal, zin, out;  /* 16 bit stereo variables */
  aud_data_w_t accum0; /* Accumulator for mul results */
  sint16 indxl,indxr;

  adpcm_data_t *data_ptr = ADPCM_dec_obj->data_in_buffer; /* Connect pointer for adpcm input */
  aud_data_t *data_out_ptr = ADPCM_dec_obj->data_out_ptr; /* Connect pointer for output */

  sint16 *stepTable = ADPCM_dec_obj->stepTable_ptr; /* Connect pointer for stepTable */

  /* Copy required variables from encoder object */
  sint16 nbit = ADPCM_dec_obj->nbit;
  sint16 mu = ADPCM_dec_obj->mu;
  sint16 blk_len = ADPCM_dec_obj->blk_len;

  /* Copy history from chunk variables */
  step = ADPCM_dec_obj->step;
  x_star = ADPCM_dec_obj->x_star;

  /* Calculate initial d' = step * previous quantized output */
  DMUL(accum0,step,ADPCM_dec_obj->z_N);
  DPACKL(d_dash,accum0);

  /* Set initial index for stepTable lookup based on previous quantised output */
  SATN(indxl, abs(ADPCM_dec_obj->z_N.l),(0),((1<<(nbit-1))-1));
  SATN(indxr, abs(ADPCM_dec_obj->z_N.r),(0),((1<<(nbit-1))-1));

  /* If no bits assigned to this bank, return empty data out without further processing */
  if(nbit == 0)
  {
      for(bufPos=0;bufPos<blk_len;++bufPos)
      {
        data_out_ptr[bufPos].l = 0;
        data_out_ptr[bufPos].r = 0;
      }
    return; 
  }

  for(bufPos=0;bufPos<blk_len;++bufPos)
    {

      /* Predictor: x* = mu( x* + d' )    */
      DADD(t0,x_star,d_dash);
      DSMUL(accum0,t0,mu);
      DPACKV(x_star,accum0,15,15); /* Pack result stored in accumulator to Q15 */

      /* Step: step = step * stepTableVal */
      stepTableVal.l = stepTable[indxl];  /* Left Channel step multiplier (Q2.13) */
      stepTableVal.r = stepTable[indxr];  /* Right Channel step multiplier (Q2.13) */ 

      DMUL(accum0,step,stepTableVal); /* step * stepTableVal (Q2.13) */
      DPACKV(step,accum0,13,13);   

      /* Copy quantised data to variable */
      zin.l = data_ptr[bufPos].l;     
      zin.r = data_ptr[bufPos].r;     

      /* Dequantizer: d' = step * quantized data */
      DMUL(accum0,step,zin);
      DPACKL(d_dash,accum0);

      /* Output = x* + d' */
      DADD(out,x_star,d_dash);
      data_out_ptr[bufPos] = out;

      /* Set index of step multiplier for next iteration based on new quantized output */
      SATN(indxl, abs(zin.l),(0),((1<<(nbit-1))-1));
      SATN(indxr, abs(zin.r),(0),((1<<(nbit-1))-1));
    }

    /* Write history to chunk variables in encoder object */
    ADPCM_dec_obj->step = step;
    ADPCM_dec_obj->z_N.l = zin.l;
    ADPCM_dec_obj->z_N.r = zin.r;
    ADPCM_dec_obj->x_star = x_star;

  return;
}
