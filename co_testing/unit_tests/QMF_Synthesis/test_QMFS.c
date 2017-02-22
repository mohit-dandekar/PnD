/*#########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding 
#  
#  Mohit Dandekar
#  John O'Callaghan
#########################################################################
#
#  Test Wrapper for unit test QMF synthesis filter bank
#
#  + Reads testvectors: bank 0,1 , filter, expected synth. outputs
#  + Calls C-kernel 'QMFS_process' 
#  + compares DUT output with REF (2 bit error allowed)
#######################################################################*/
#include <stdio.h>
#include <stdlib.h>

#include "QMF.h"

#define P 738
#define N P*BLOCK_SIZE 

QMFS_bank_t QMFS_bank;

aud_data_t data_in_bank0[P*BLOCK_SIZE/2];
aud_data_t data_in_bank1[P*BLOCK_SIZE/2];
aud_data_t data_out_ref[P*BLOCK_SIZE];
aud_data_t data_out_dut[P*BLOCK_SIZE];

sint16 filter[FILT_LEN];

aud_data_t temp_buffer[BLOCK_SIZE];

int main(void)
{
  sint32 ret = 0;
  sint32 i,j;

  /*Read Testvector data*/
  printf("Reading Testvectors\n");
  FILE *fp;
  fp = fopen("./test_vectors/data_expected.txt","r");
  for(i=0;i<BLOCK_SIZE*P;++i)
  {
    int templ,tempr;
    fscanf(fp,"%d,%d",&templ,&tempr);
    data_out_ref[i].l = templ;
    data_out_ref[i].r = tempr;
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

  fp = fopen("./test_vectors/bank0.txt","r");
  for(i=0;i<BLOCK_SIZE*P/2;++i)
  {
    int templ,tempr;
    fscanf(fp,"%d,%d",&templ,&tempr);
    data_in_bank0[i].l = templ;
    data_in_bank0[i].r = tempr;
  }
  fclose(fp);

  fp = fopen("./test_vectors/bank1.txt","r");
  for(i=0;i<BLOCK_SIZE*P/2;++i)
  {
    int templ,tempr;
    fscanf(fp,"%d,%d",&templ,&tempr);
    data_in_bank1[i].l = templ;
    data_in_bank1[i].r = tempr;
  }
  fclose(fp);





  /*Initialization*/
  printf("Initialize: C kernel under Test\n");
  QMFS_bank_init( &QMFS_bank,
                  temp_buffer,
                  filter,
                  BLOCK_SIZE/2);

  /*Process Call*/
  printf("Process Call: C kernel under Test\n");
  for(i=0;i<P;++i)
  {
    /*Write  Input Buffer of the QMF*/
    for(j=0;j<BLOCK_SIZE/2;++j)
    {
      QMFS_bank.data_in_buffer_LO[j] = data_in_bank0[i*BLOCK_SIZE/2 + j];
      QMFS_bank.data_in_buffer_HI[j] = data_in_bank1[i*BLOCK_SIZE/2 + j];
    }

    /*QMF process call*/
    QMFS_process( &QMFS_bank);


    /*Read from temporary out buffers to main memory*/
    for(j=0;j<BLOCK_SIZE;++j)
    {
       data_out_dut[i*BLOCK_SIZE + j] = temp_buffer[j];
    }
  }

  /* Compare Output vs Ref*/
  ret = 0;
  printf("Checking synthesis output\n");
  for(i=0;i<P*BLOCK_SIZE/2;++i)
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


