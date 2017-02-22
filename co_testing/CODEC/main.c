#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wavpcm_io.h"
#include "globals.h"

#include "ADPCM_SBC.h"

/*Instantiate the Coder and Decoder objects*/
ADPCM_SBCoder_t ADPCM_SBCoder;
ADPCM_SBDecoder_t ADPCM_SBDecoder;

/*Temporary buffers to store the encoded bitstream*/
bitstream bit_buffer[2][3*BLOCK_SIZE/8]; // 2 channels 3 bits per sample average for 24kbps


/*Temporary buffers to store the decoded and reconstructed output*/
aud_data_t out_buffer[BLOCK_SIZE];



/* This is the function that is called when the program starts. */
int main (int argc, char *argv[])
{
  printf("BLOCK SIZE = %d\n",BLOCK_SIZE*2);
  if(argc>1)
  {
    printf("%s",argv[0]);  
  }

  printf("structure size : %d  %d\n",sizeof(ADPCM_SBCoder),sizeof(ADPCM_SBDecoder));

  /* Variable declarations. */
  struct wavpcm_input input;
  struct wavpcm_output output;
  short buffer[BUFFERSIZE]; 
  short reconstructedBuffer[BUFFERSIZE];
  int bufPos, bufIndex, read;

  memset(&input, 0, sizeof(struct wavpcm_input));
  input.resource=INPUTWAVFILE;
  memset(&output, 0, sizeof(struct wavpcm_output));
  output.resource=OUTPUTWAVFILE;

  /* First open input file and parse header, */
  wavpcm_input_open (&input);
  /* and then use this same header configuration for the output file */
  wavpcm_output_copy_settings(&input, &output);
  wavpcm_output_open(&output);


/*-------------------------------------------------------------------*/
 
                 /*Initialize Encoder and Decoder*/


                 ADPCM_SBCoder_init( &ADPCM_SBCoder,
                                      bit_buffer[0], 
                                      bit_buffer[1],
                                      BLOCK_SIZE);
               
               
                 ADPCM_SBDecoder_init( &ADPCM_SBDecoder,
                                       out_buffer,
                                       BLOCK_SIZE);
               
               
/*-------------------------------------------------------------------*/



 
  /*bufPos expressed in temporal samples*/
  FILE *fpbtstrm = fopen("./test_vectors/bitsout.txt","w");
  FILE *fpindata = fopen("./test_vectors/datain.txt","w");
  FILE *fpoutdata = fopen("./test_vectors/dataout.txt","w");
  for (bufPos=0; bufPos<input.samplesAvailable; bufPos+=(BUFFERSIZE/2)) {
    /* Try to read BUFFERSIZE samples (16 bits, pairwise identical if input is mono, interleaved if input is stereo)  */
    /* into buffer, with read the actual amount read (expressed in bytes! =  (2*read)/(channels * bitDepth/8) array elements)*/
    read = wavpcm_input_read (&input, buffer);
    
    /*Dumping Block data*/
    for(bufIndex=0;bufIndex<BLOCK_SIZE;++bufIndex){
      fprintf(fpindata,"%d,%d\n",buffer[2*bufIndex + 0],buffer[2*bufIndex + 1]);
    }
    
/*----------------------------- SUBBAND ENCODER  ----------------------------*/
             /*Writing into the subband coder input buffer*/
             for(bufIndex=0;bufIndex<BLOCK_SIZE;++bufIndex) 
             {
               ADPCM_SBCoder.inp_buffer[bufIndex].l = buffer[2*bufIndex + 0];
               ADPCM_SBCoder.inp_buffer[bufIndex].r = buffer[2*bufIndex + 1];
             }  
         
             ADPCM_SBCoder_process( &ADPCM_SBCoder);
/*---------------------------------------------------------------------------*/
          
         
             /*Dumping Bitstream*/
    for(bufIndex=0;bufIndex<3*BLOCK_SIZE/8;++bufIndex){
      fprintf(fpbtstrm,"%d,%d\n",bit_buffer[0][bufIndex],bit_buffer[1][bufIndex]);
    }



/*---------------------------- SUBBAND DECODER  -----------------------------*/
              /*Writing into the subband decoder input buffer*/
              for(bufIndex=0;bufIndex<3*BLOCK_SIZE/8;++bufIndex)
              {
                ADPCM_SBDecoder.inp_buffer_L[bufIndex] = bit_buffer[0][bufIndex];
                ADPCM_SBDecoder.inp_buffer_R[bufIndex] = bit_buffer[1][bufIndex];
              }
           
              ADPCM_SBDecoder_process( &ADPCM_SBDecoder);
/*---------------------------------------------------------------------------*/


    
    /*Reading data out to reconstructed buffer*/ 
    for(bufIndex=0;bufIndex<BLOCK_SIZE;++bufIndex) 
    {
      reconstructedBuffer[2*bufIndex + 0] = out_buffer[bufIndex].l;
      reconstructedBuffer[2*bufIndex + 1] = out_buffer[bufIndex].r;
    } 

    /*Dumping Reconstructed Data*/    
    for(bufIndex=0;bufIndex<BLOCK_SIZE;++bufIndex){
      fprintf(fpoutdata,"%d,%d\n",out_buffer[bufIndex].l,out_buffer[bufIndex].r);
    }

    /* dump reconstructed output */
    wavpcm_output_write (&output, reconstructedBuffer, read);
  }

  fclose(fpbtstrm); fclose(fpindata);fclose(fpoutdata);

  /* finalize output (write header) and close */
  wavpcm_output_close (&output);  

  /* Return successful exit code. */
  return 0;
}
