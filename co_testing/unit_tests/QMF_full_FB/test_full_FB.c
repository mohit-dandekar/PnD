/*#########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding 
#  
#  Mohit Dandekar
#  John O'Callaghan
#########################################################################
#
#  Test Wrapper for unit test QMF analysis filter bank
#
#  + Reads testvectors: data_in, filter, expected bank 0,1 outputs
#  + Calls C-kernel 'QMFA_process' 
#  + compares DUT output with REF (1 bit error allowed)
#######################################################################*/
#include <stdio.h>
#include <stdlib.h>

#include "SBC.h"

#define PMAX 10000

SBC_AFB_t SBC_AFB;
SBC_SFB_t SBC_SFB;

aud_data_t data_in[PMAX*BLOCK_SIZE];
aud_data_t data_out_ref[PMAX*BLOCK_SIZE];
aud_data_t data_out_dut[PMAX*BLOCK_SIZE];

sint16 filter_analysis[FILT_LEN];
sint16 filter_synthesis[FILT_LEN];

aud_data_t temp_buffer[5][BLOCK_SIZE];
aud_data_t data_out_buffer[BLOCK_SIZE];

aud_data_t subband_buffer[5][PMAX*BLOCK_SIZE];

int main(void)
{
  sint32 ret = 0;
  sint32 i,j;

  /*Read Testvector data*/
  printf("Reading Testvectors\n");
 

  FILE *fp;

  fp = fopen("./test_vectors/data_size.txt","r");
  int N;
  fscanf(fp,"%d",&N);
  fclose(fp);

  sint32 P = N/BLOCK_SIZE;

  fp = fopen("./test_vectors/data_in.txt","r");
  for(i=0;i<BLOCK_SIZE*P;++i)
  {
    int templ,tempr;
    fscanf(fp,"%d,%d",&templ,&tempr);
    data_in[i].l = templ;
    data_in[i].r = tempr;
  }
  fclose(fp);

  fp = fopen("./test_vectors/data_expected.txt","r");
  for(i=0;i<BLOCK_SIZE*P;++i)
  {
    int templ,tempr;
    fscanf(fp,"%d,%d",&templ,&tempr);
    data_out_ref[i].l = templ;
    data_out_ref[i].r = tempr;
  }
  fclose(fp);


  fp = fopen("./test_vectors/Afilter.txt","r");
  for(i=0;i<40;++i)
  {
    int temp;
    fscanf(fp,"%d",&temp);
    filter_analysis[i] = temp;
  }
  fclose(fp);

  fp = fopen("./test_vectors/Sfilter.txt","r");
  for(i=0;i<40;++i)
  {
    int temp;
    fscanf(fp,"%d",&temp);
    filter_synthesis[i] = temp;
  }
  fclose(fp);


  /*Initialization*/
  for(i=0;i<P*BLOCK_SIZE;++i)
  {
    for(j=0;j<5;++j)
    {
      subband_buffer[j][i].l = 0;
      subband_buffer[j][i].r = 0;
    }
  }

  printf("Initialize: C kernel under Test\n");
  SBC_AFB_init( &SBC_AFB,
                 temp_buffer[0],              
                 temp_buffer[1],              
                 temp_buffer[2],              
                 temp_buffer[3],              
                 temp_buffer[4],
                 filter_analysis,
                 BLOCK_SIZE);

  SBC_SFB_init( &SBC_SFB, 
                 data_out_buffer,
                 filter_synthesis,
                 BLOCK_SIZE);


  /*Process Call*/
  printf("Process Call: C kernel under Test\n");
 
  printf("Analysis FB\n"); 
  for(i=0;i<P;++i)
  {
    /*Write  Input Buffer of the QMF Tree*/
    for(j=0;j<BLOCK_SIZE;++j)
    {
      SBC_AFB.inp_buffer[j] = data_in[i*BLOCK_SIZE + j];
    }

    /*QMF Analysis*/
    SBC_AFB_process( &SBC_AFB);

    for(j=0;j<BLOCK_SIZE/8;++j)
    {
      subband_buffer[0][i*BLOCK_SIZE/8 + j] = temp_buffer[0][j];
      subband_buffer[1][i*BLOCK_SIZE/8 + j] = temp_buffer[1][j];
    } 
    for(j=0;j<BLOCK_SIZE/4;++j)
    {
      subband_buffer[2][i*BLOCK_SIZE/4 + j + FILT_LEN-2] = temp_buffer[2][j];
      subband_buffer[3][i*BLOCK_SIZE/4 + j + FILT_LEN-2] = temp_buffer[3][j];
      subband_buffer[4][i*BLOCK_SIZE/4 + j + FILT_LEN-2] = temp_buffer[4][j];
    }
  }



  
  


  printf("Synthesis FB\n"); 
  for(i=0;i<P;++i)
  {

    for(j=0;j<BLOCK_SIZE/8;++j)
    {
      SBC_SFB.in_buffer0_0_0[j] = subband_buffer[0][i*BLOCK_SIZE/8 + j] ;
      SBC_SFB.in_buffer0_0_1[j] = subband_buffer[1][i*BLOCK_SIZE/8 + j] ;
    } 
    for(j=0;j<BLOCK_SIZE/4;++j)
    {
      SBC_SFB.in_buffer0_1[j] = subband_buffer[2][i*BLOCK_SIZE/4 + j ]; 
      SBC_SFB.in_buffer1_0[j] = subband_buffer[3][i*BLOCK_SIZE/4 + j ];
      SBC_SFB.in_buffer1_1[j] = subband_buffer[4][i*BLOCK_SIZE/4 + j ];
    } 

    /*QMF Synthesis*/
    SBC_SFB_process( &SBC_SFB);


    /*Read from temporary out buffers to main memory*/
    for(j=0;j<BLOCK_SIZE;++j)
    {
       data_out_dut[i*BLOCK_SIZE + j] = data_out_buffer[j];
    }
  }


  /* Compare Output vs Ref*/
  ret = 0;
  printf("Checking reconstructed output\n");
  for(i=0;i<P*BLOCK_SIZE;++i)
  {
    /*Allowing 2 bit errors*/
    if(abs(data_out_ref[i].l - data_out_dut[i].l)>2 || abs(data_out_ref[i].r - data_out_dut[i].r)>2)
    {
      printf("ERROR : indx = %8d expected = %8d,%8d  output %8d %8d\n",i,data_out_ref[i].l, 
                                                                          data_out_ref[i].r,  
                                                                          data_out_dut[i].l,
                                                                          data_out_dut[i].r);
      ret += 1;
    }
  }


  if(ret)
  {
         printf("                          TEST FAIL                               \n");
         printf("  total errors: %d \n",ret                                           );
         printf("##################################################################\n");

  }else{
         printf("                          TEST PASS                               \n");
         printf("##################################################################\n");
       }

  return 0;
}


