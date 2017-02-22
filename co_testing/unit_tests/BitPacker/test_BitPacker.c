/*#########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding
#
#  Mohit Dandekar
#  John O'Callaghan
#########################################################################
#
#  Test Wrapper for unit test BitPacker Encoder
#
#  + Reads testvectors: data_in,  expected output
#  + Calls C-kernel 'ADPCM_enc_process'
#  + compares DUT output with REF
#######################################################################*/

#include <stdio.h>
#include <stdlib.h>

#include "BitPacker.h"
#include <math.h>
#define PMAX 10000

  BitPack_t BitPack;
  BitDepack_t BitDepack;

  adpcm_data_t data_in[5][PMAX*BLOCK_SIZE/4];
  adpcm_data_t data_out_dut[5][PMAX*BLOCK_SIZE/4];

  bitstream bitsout[2][3*BLOCK_SIZE/8];

  adpcm_data_t band0_buffer[BLOCK_SIZE/8];
  adpcm_data_t band1_buffer[BLOCK_SIZE/8];
  adpcm_data_t band2_buffer[BLOCK_SIZE/4];
  adpcm_data_t band3_buffer[BLOCK_SIZE/4];
  adpcm_data_t band4_buffer[BLOCK_SIZE/4];

int main(void)
{
  sint32 ret = 0;
  sint32 i,j;

  FILE *fp;

  fp = fopen("./test_vectors/data_size.txt","r");
  int N;
  fscanf(fp,"%d",&N);
  fclose(fp);

  sint32 P = N/(BLOCK_SIZE);

  fp = fopen("./test_vectors/band0.txt","r");
  for(i=0;i<BLOCK_SIZE*P/8;++i)
  {
    int templ,tempr;
    fscanf(fp,"%d,%d",&templ,&tempr);
    data_in[0][i].l = templ;
    data_in[0][i].r = tempr;
  }
  fclose(fp);
  fp = fopen("./test_vectors/band1.txt","r");
  for(i=0;i<BLOCK_SIZE*P/8;++i)
  {
    int templ,tempr;
    fscanf(fp,"%d,%d",&templ,&tempr);
    data_in[1][i].l = templ;
    data_in[1][i].r = tempr;
  }
  fclose(fp);
  fp = fopen("./test_vectors/band2.txt","r");
  for(i=0;i<BLOCK_SIZE*P/4;++i)
  {
    int templ,tempr;
    fscanf(fp,"%d,%d",&templ,&tempr);
    data_in[2][i].l = templ;
    data_in[2][i].r = tempr;
  }
  fclose(fp);
  fp = fopen("./test_vectors/band3.txt","r");
  for(i=0;i<BLOCK_SIZE*P/4;++i)
  {
    int templ,tempr;
    fscanf(fp,"%d,%d",&templ,&tempr);
    data_in[3][i].l = templ;
    data_in[3][i].r = tempr;
  }
  fclose(fp);
  fp = fopen("./test_vectors/band4.txt","r");
  for(i=0;i<BLOCK_SIZE*P/4;++i)
  {
    int templ,tempr;
    fscanf(fp,"%d,%d",&templ,&tempr);
    data_in[4][i].l = templ;
    data_in[4][i].r = tempr;
  }
  fclose(fp);




  /*Initialization*/
  printf("Initialize: C kernel under Test\n");
  BitPacker_init( &BitPack,
                  bitsout[0],
                  bitsout[1]);
 
  BitDepacker_init( &BitDepack,
                    band0_buffer,
                    band1_buffer,
                    band2_buffer,
                    band3_buffer,
                    band4_buffer); 

  /*Process Call*/
  printf("Process Call: C kernel under Test\n");
  for(i=0;i<P;++i)
 {
    /*Write  Input Buffer of the encoder*/
  for(j=0;j<BLOCK_SIZE/8;++j)
  {
    BitPack.inp_band0[j] = data_in[0][i*BLOCK_SIZE/8 +j] ;
    BitPack.inp_band1[j] = data_in[1][i*BLOCK_SIZE/8 +j] ;
    BitPack.inp_band2[j] = data_in[2][i*BLOCK_SIZE/4 +j] ;
    BitPack.inp_band3[j] = data_in[3][i*BLOCK_SIZE/4 +j] ;
    BitPack.inp_band4[j] = data_in[4][i*BLOCK_SIZE/4 +j] ;
  }
  for(j=BLOCK_SIZE/8;j<BLOCK_SIZE/4;++j)
  {
    BitPack.inp_band2[j] = data_in[2][i*BLOCK_SIZE/4 +j] ;
    BitPack.inp_band3[j] = data_in[3][i*BLOCK_SIZE/4 +j] ;
    BitPack.inp_band4[j] = data_in[4][i*BLOCK_SIZE/4 +j] ;
  }

    /*ADPCM enc process call*/
    BitPacker_process( &BitPack);
    /*Read from temporary out buffer to main memory*/

    for(j=0;j<3*BLOCK_SIZE/8;++j)
    {
      BitDepack.inp_buffer0[j] = bitsout[0][j];
      BitDepack.inp_buffer1[j] = bitsout[1][j];
    } 
   


    BitDepacker_process( &BitDepack);

    for(j=0;j<BLOCK_SIZE/8;++j)
    {
      data_out_dut[0][i*BLOCK_SIZE/8 +j] =  band0_buffer[j] ;
      data_out_dut[1][i*BLOCK_SIZE/8 +j] =  band1_buffer[j];
      data_out_dut[2][i*BLOCK_SIZE/4 +j] =  band2_buffer[j] ;
      data_out_dut[3][i*BLOCK_SIZE/4 +j] =  band3_buffer[j] ;
      data_out_dut[4][i*BLOCK_SIZE/4 +j] =  band4_buffer[j] ;
     }
    for(j=BLOCK_SIZE/8;j<BLOCK_SIZE/4;++j)
    {
      data_out_dut [2][i*BLOCK_SIZE/4 +j] =  band2_buffer[j] ;
      data_out_dut [3][i*BLOCK_SIZE/4 +j] =  band3_buffer[j] ;
      data_out_dut [4][i*BLOCK_SIZE/4 +j] =  band4_buffer[j] ;
    }

  }

  /* Compare Output vs Ref*/
  ret = 0;
  sint32 sc[] = {8,8,4,4}; sint32 delay[] = {0, 0, FILT_LEN-2, FILT_LEN-2};
  printf("Checking output\n");
  for(j=0;j<4;++j)
  {
    for(i=0;i<N/sc[j]-delay[j];++i)
    {
      /*Allowing no errors*/
      if(abs(data_in[j][i].l - data_out_dut[j][i+delay[j]].l)>0 || abs(data_in[j][i].r - data_out_dut[j][i+delay[j]].r)>0)
      {
        printf("ERROR : Band = %d indx = %8d expected = %8d,%8d  output %8d %8d\n",j,i,data_in[j][i].l,
                                                                          data_in[j][i].r,
                                                                          data_out_dut[j][i+delay[j]].l,
                                                                          data_out_dut[j][i+delay[j]].r);
        ret += 1;
      }
    }
  }

  if(ret)
  {
         printf("                          TEST FAIL                               \n");
         printf("##################################################################\n");

  }else{
         printf("                          TEST PASS                               \n");
         printf("##################################################################\n");
       }

  return 0;
}

