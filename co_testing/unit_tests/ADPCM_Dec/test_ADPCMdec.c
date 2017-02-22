/*#########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding
#
#  Mohit Dandekar
#  John O'Callaghan
#########################################################################
#
#  Test Wrapper for unit test ADPCM Decoder
#
#  + Reads testvectors: data_in,  expected output
#  + Calls C-kernel 'ADPCM_dec_process'
#  + compares DUT output with REF
#######################################################################*/

#include <stdio.h>
#include <stdlib.h>

#include "ADPCM.h"

#define P 738*4
#define N P*BLOCK_SIZE/4

ADPCM_dec_t ADPCM_dec;

adpcm_data_t data_in[P*BLOCK_SIZE];
aud_data_t data_out_ref[P*BLOCK_SIZE];
aud_data_t data_out_dut[P*BLOCK_SIZE];

sint16 mu;
sint8 nbit;
sint16 blk_len;

aud_data_t temp_buffer[BLOCK_SIZE];

int main(void)
{
  sint32 ret = 0;
  sint32 i,j;

  /*Read Testvector data*/
  printf("Reading Testvectors\n");
  FILE *fp;
  fp = fopen("./test_vectors/data_in.txt","r");
  
  for(i=0;i<BLOCK_SIZE*P;++i)
  {
    int templ,tempr;
    fscanf(fp,"%d,%d",&templ,&tempr);
    data_in[i].l = templ;
    data_in[i].r = tempr;
  }
  fclose(fp);

  fp = fopen("./test_vectors/adpcm_expected.txt","r");
  for(i=0;i<BLOCK_SIZE*P;++i)
  {
    int templ,tempr;
    fscanf(fp,"%d,%d",&templ,&tempr);
    data_out_ref[i].l = templ;
    data_out_dut[i].l = 0;
    data_out_ref[i].r = tempr;
    data_out_dut[i].r = 0;
  }
  fclose(fp);

  mu = 13222; // 0.4035
  nbit = 5;
  blk_len = BLOCK_SIZE/4;

  /*Initialization*/
  printf("Initialize: C kernel under Test\n");
  ADPCM_dec_init( &ADPCM_dec,
                  temp_buffer,
                  mu,
                  nbit,
                  blk_len);

  /*Process Call*/
  printf("Process Call: C kernel under Test\n");
  for(i=0;i<P;++i)
  {

    /*Write  Input Buffer of the encoder*/
   for(j=0;j<blk_len;++j)
    {
      ADPCM_dec.data_in_buffer[j] = data_in[i*blk_len + j];
    }

    /*ADPCM dec process call*/
    ADPCM_dec_process(&ADPCM_dec);

    /*Read from temporary out buffer to main memory*/
    for(j=0;j<blk_len;++j)
    {
       data_out_dut[i*blk_len + j] = temp_buffer[j];
    }
  }

  /* Compare Output vs Ref*/
  ret = 0;
  printf("Checking output\n");
  fp = fopen("./test_vectors/output.txt","w");
  for(i=0;i<P*blk_len;++i)
  {
    fprintf(fp,"%d,%d\n",data_out_dut[i].l,data_out_dut[i].r);
    /*Allowing no errors*/
    if(abs(data_out_ref[i].l - data_out_dut[i].l)>0 || abs(data_out_ref[i].r - data_out_dut[i].r)>0)
    {
      printf("ERROR : indx = %8d expected = %8d,%8d  output %8d %8d\n",i,data_out_ref[i].l,
                                                                          data_out_ref[i].r,
                                                                          data_out_dut[i].l,
                                                                          data_out_dut[i].r);
      ret += 1;
    }
  }
  fclose(fp);
  if(ret)
  {
         printf("                          TEST FAIL                               \n");
         printf("  total errors: %d of %d \n",ret,P*blk_len                           );
         printf("##################################################################\n");

  }else{
         printf("                          TEST PASS                               \n");
         printf("##################################################################\n");
       }

  return 0;
}

