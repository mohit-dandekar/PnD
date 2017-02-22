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

#include "QMF.h"

#define P 738
#define N P*BLOCK_SIZE 

QMFA_bank_t QMFA_bank;
QMFS_bank_t QMFS_bank;

aud_data_t data_in[P*BLOCK_SIZE];
aud_data_t data_out_ref[P*BLOCK_SIZE];
aud_data_t data_out_dut[P*BLOCK_SIZE];

sint16 filter_analysis[FILT_LEN];
sint16 filter_synthesis[FILT_LEN];

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
  printf("Initialize: C kernel under Test\n");
  QMFA_bank_init( &QMFA_bank,
                  QMFS_bank.data_in_buffer_LO,
                  QMFS_bank.data_in_buffer_HI,
                  filter_analysis,
                  BLOCK_SIZE);

  QMFS_bank_init( &QMFS_bank,
                  temp_buffer,
                  filter_synthesis,
                  BLOCK_SIZE/2);


  /*Process Call*/
  printf("Process Call: C kernel under Test\n");
  for(i=0;i<P;++i)
  {
    /*Write  Input Buffer of the QMF*/
    for(j=0;j<BLOCK_SIZE;++j)
    {
      QMFA_bank.data_in_buffer[j] = data_in[i*BLOCK_SIZE + j];
    }

    /*QMF Analysis*/
    QMFA_process( &QMFA_bank);


    /*QMF Synthesis*/
    QMFS_process( &QMFS_bank);



    /*Read from temporary out buffers to main memory*/
    for(j=0;j<BLOCK_SIZE;++j)
    {
       data_out_dut[i*BLOCK_SIZE + j] = temp_buffer[j];
    }
  }


  /* Compare Output vs Ref*/
  ret = 0;
  printf("Checking reconstructed output\n");
  for(i=0;i<P*BLOCK_SIZE;++i)
  {
    /*Allowing no bit errors*/
    if(abs(data_out_ref[i].l - data_out_dut[i].l)>0 || abs(data_out_ref[i].r - data_out_dut[i].r)>0)
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


