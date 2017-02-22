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

aud_data_t data_in[P*BLOCK_SIZE];
aud_data_t data_out_bank0_ref[P*BLOCK_SIZE/2];
aud_data_t data_out_bank0_dut[P*BLOCK_SIZE/2];
aud_data_t data_out_bank1_ref[P*BLOCK_SIZE/2];
aud_data_t data_out_bank1_dut[P*BLOCK_SIZE/2];

sint16 filter[FILT_LEN];

aud_data_t temp_buffer0[BLOCK_SIZE];
aud_data_t temp_buffer1[BLOCK_SIZE];

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

  fp = fopen("./test_vectors/filter.txt","r");
  for(i=0;i<40;++i)
  {
    int temp;
    fscanf(fp,"%d",&temp);
    filter[i] = temp;
  }
  fclose(fp);

  fp = fopen("./test_vectors/bank0_expected.txt","r");
  for(i=0;i<BLOCK_SIZE*P/2;++i)
  {
    int templ,tempr;
    fscanf(fp,"%d,%d",&templ,&tempr);
    data_out_bank0_ref[i].l = templ;
    data_out_bank0_dut[i].l = 0;
    data_out_bank0_ref[i].r = tempr;
    data_out_bank0_dut[i].r = 0;
  }
  fclose(fp);

  fp = fopen("./test_vectors/bank1_expected.txt","r");
  for(i=0;i<BLOCK_SIZE*P/2;++i)
  {
    int templ,tempr;
    fscanf(fp,"%d,%d",&templ,&tempr);
    data_out_bank1_ref[i].l = templ;
    data_out_bank1_dut[i].l = 0;
    data_out_bank1_ref[i].r = tempr;
    data_out_bank1_dut[i].r = 0;
  }
  fclose(fp);


  /*Initialization*/
  printf("Initialize: C kernel under Test\n");
  QMFA_bank_init( &QMFA_bank,
                  temp_buffer0,
                  temp_buffer1,
                  filter,
                  BLOCK_SIZE);

  /*Process Call*/
  printf("Process Call: C kernel under Test\n");
  for(i=0;i<P;++i)
  {
    /*Write  Input Buffer of the QMF*/
    for(j=0;j<BLOCK_SIZE;++j)
    {
      QMFA_bank.data_in_buffer[j] = data_in[i*BLOCK_SIZE + j];
    }

    /*QMF process call*/
    QMFA_process( &QMFA_bank);

    /*Read from temporary out buffers to main memory*/
    for(j=0;j<BLOCK_SIZE/2;++j)
    {
       data_out_bank0_dut[i*BLOCK_SIZE/2 + j] = temp_buffer0[j];
       data_out_bank1_dut[i*BLOCK_SIZE/2 + j] = temp_buffer1[j];
    }
  }


  /* Compare Output vs Ref*/
  ret = 0;
  printf("Checking Bank0 output\n");
  for(i=0;i<P*BLOCK_SIZE/2;++i)
  {
    /*Allowing 2 bit errors*/
    if(abs(data_out_bank0_ref[i].l - data_out_bank0_dut[i].l)>1 || abs(data_out_bank0_ref[i].l - data_out_bank0_dut[i].l)>1)
    {
      printf("ERROR : indx = %8d expected = %8d,%8d  output %8d %8d\n",i,data_out_bank0_ref[i].l, 
                                                                          data_out_bank0_ref[i].r,  
                                                                          data_out_bank0_dut[i].l,
                                                                          data_out_bank0_dut[i].r);
      ret += 1;
    }
  }


  printf("Checking Bank1 output\n");
  for(i=0;i<P*BLOCK_SIZE/2;++i)
  {
    /*Allowing 2 bit errors*/
    if(abs(data_out_bank1_ref[i].l - data_out_bank1_dut[i].l)>0 || abs(data_out_bank1_ref[i].l - data_out_bank1_dut[i].l)>0)
    {
      printf("ERROR : indx = %8d expected = %8d,%8d  output %8d %8d\n",i,data_out_bank1_ref[i].l, 
                                                                          data_out_bank1_ref[i].r,  
                                                                          data_out_bank1_dut[i].l,
                                                                          data_out_bank1_dut[i].r);
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


