/*#########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding 
#  
#  Mohit Dandekar
#  John O'Callaghan
#########################################################################*/

#include "BitPacker.h"

void BitPacker_init(BitPack_t *BitPack_obj,
                    bitstream *bitsout_L,
                    bitstream *bitsout_R)
{
  sint32 i;
  
  for(i=0;i<BLOCK_SIZE/8;++i)
  {
    BitPack_obj->inp_band0[i].l = 0;
    BitPack_obj->inp_band0[i].r = 0;
    BitPack_obj->inp_band1[i].l = 0;
    BitPack_obj->inp_band1[i].r = 0;
    BitPack_obj->inp_band2[i].l = 0;
    BitPack_obj->inp_band2[i].r = 0;
    BitPack_obj->inp_band3[i].l = 0;
    BitPack_obj->inp_band3[i].r = 0;
    BitPack_obj->inp_band4[i].l = 0;
    BitPack_obj->inp_band4[i].r = 0;
  }
  for(i=BLOCK_SIZE/8;i<BLOCK_SIZE/4;++i)
  {
    BitPack_obj->inp_band2[i].l = 0;
    BitPack_obj->inp_band2[i].r = 0;
    BitPack_obj->inp_band3[i].l = 0;
    BitPack_obj->inp_band3[i].r = 0;
    BitPack_obj->inp_band4[i].l = 0;
    BitPack_obj->inp_band4[i].r = 0;
  }

  for(i=0;i<BLOCK_SIZE/4+FILT_LEN-2;++i)
  {
    BitPack_obj->delay_buffer2[i].l = 0;
    BitPack_obj->delay_buffer2[i].r = 0;
    BitPack_obj->delay_buffer3[i].l = 0;
    BitPack_obj->delay_buffer3[i].r = 0;
  }

  /* Initialise read and write pointers for bank 2 delay buffer */
  BitPack_obj->delay_buffer2_rd = 0;
  BitPack_obj->delay_buffer2_wr = FILT_LEN-2;
  BitPack_obj->delay_buffer2_lim = BLOCK_SIZE/4+FILT_LEN-2;

  /* Initialise read and write pointers for bank 3 delay buffer */
  BitPack_obj->delay_buffer3_rd = 0;
  BitPack_obj->delay_buffer3_wr = FILT_LEN-2;
  BitPack_obj->delay_buffer3_lim = BLOCK_SIZE/4+FILT_LEN-2;

  BitPack_obj->out_ptr_L = bitsout_L;
  BitPack_obj->out_ptr_R = bitsout_R;

  return;
}


void BitPacker_process(BitPack_t *BitPack_obj)
{
  sint32 i; /* Iterator */

  for(i=0;i<BLOCK_SIZE/4;++i)
  {
   /* Copy data from input buffer to delay buffer for bank 2 */
   BitPack_obj->delay_buffer2[BitPack_obj->delay_buffer2_wr] = BitPack_obj->inp_band2[i];
   BitPack_obj->delay_buffer2_wr += 1; 
   if(BitPack_obj->delay_buffer2_wr >= BitPack_obj->delay_buffer2_lim)
    {
      BitPack_obj->delay_buffer2_wr = 0;
    }

   /* Copy data from input buffer to delay buffer for bank 3 */
   BitPack_obj->delay_buffer3[BitPack_obj->delay_buffer3_wr] = BitPack_obj->inp_band3[i];
   BitPack_obj->delay_buffer3_wr += 1; 
   if(BitPack_obj->delay_buffer3_wr >= BitPack_obj->delay_buffer3_lim)
    {
      BitPack_obj->delay_buffer3_wr = 0;
    }
  }
  

  for(i=0;i<3*BLOCK_SIZE/8/3;++i)
  {
    bitstream byte_out0l, byte_out1l, byte_out2l;
    bitstream byte_out0r, byte_out1r, byte_out2r;
    sint8 byte0l, byte0r;
    sint8 byte1l, byte1r;
    sint8 byte2_0l, byte2_0r;
    sint8 byte2_1l, byte2_1r;
    sint8 byte3_0l, byte3_0r;
    sint8 byte3_1l, byte3_1r;

    byte0l = BitPack_obj->inp_band0[i].l;
    byte0r = BitPack_obj->inp_band0[i].r;
    byte1l = BitPack_obj->inp_band1[i].l;
    byte1r = BitPack_obj->inp_band1[i].r;

    /* Load two output values from bank 2 delay buffer to local variables */
    byte2_0l = BitPack_obj->delay_buffer2[BitPack_obj->delay_buffer2_rd + 0].l;
    byte2_0r = BitPack_obj->delay_buffer2[BitPack_obj->delay_buffer2_rd + 0].r;
    byte2_1l = BitPack_obj->delay_buffer2[BitPack_obj->delay_buffer2_rd + 1].l;
    byte2_1r = BitPack_obj->delay_buffer2[BitPack_obj->delay_buffer2_rd + 1].r;
    BitPack_obj->delay_buffer2_rd += 2;

    /* if end of delay buffer 2 reached, set read to 0 */
    if(BitPack_obj->delay_buffer2_rd >= BitPack_obj->delay_buffer2_lim)
    {
      BitPack_obj->delay_buffer2_rd = 0;
    } 

    /* Load two output values from bank 3 delay buffer to local variables */
    byte3_0l = BitPack_obj->delay_buffer3[BitPack_obj->delay_buffer3_rd + 0].l;
    byte3_0r = BitPack_obj->delay_buffer3[BitPack_obj->delay_buffer3_rd + 0].r;
    byte3_1l = BitPack_obj->delay_buffer3[BitPack_obj->delay_buffer3_rd + 1].l;
    byte3_1r = BitPack_obj->delay_buffer3[BitPack_obj->delay_buffer3_rd + 1].r;
    BitPack_obj->delay_buffer3_rd += 2;

    /* if end of delay buffer 3 reached, set read to 0 */
    if(BitPack_obj->delay_buffer3_rd >= BitPack_obj->delay_buffer3_lim)
    {
      BitPack_obj->delay_buffer3_rd = 0;
    } 

    /* Create three output bytes (left channel) */
    byte_out0l = (bitstream)((byte0l & 0x1f) | (byte3_0l << 5));   /* Byte 0 packs bank0 output (5 bit) and bank3 output (3 bit) */
    byte_out1l = (bitstream)((byte1l & 0x1f) | (byte3_1l << 5));   /* Byte 1 packs bank1 output (5 bit) and next bank3 output (3 bit) */
    byte_out2l = (bitstream)((byte2_0l & 0x0f) | (byte2_1l << 4)); /* Byte 2 packs two consecutive bank2 outputs (4 bit each) */

    /* Create three output bytes (right channel) */
    byte_out0r = (bitstream)((byte0r & 0x1f) | (byte3_0r << 5));   /* Byte 0 packs bank0 output (5 bit) and bank3 output (3 bit) */
    byte_out1r = (bitstream)((byte1r & 0x1f) | (byte3_1r << 5));   /* Byte 1 packs bank1 output (5 bit) and next bank3 output (3 bit) */
    byte_out2r = (bitstream)((byte2_0r & 0x0f) | (byte2_1r << 4)); /* Byte 2 packs two consecutive bank2 outputs (4 bit each) */

    /* Write 3 output bytes to packer object (left channel) */
    BitPack_obj->out_ptr_L[3*i + 0] = byte_out0l;
    BitPack_obj->out_ptr_L[3*i + 1] = byte_out1l;
    BitPack_obj->out_ptr_L[3*i + 2] = byte_out2l;

    /* Write 3 output bytes to packer object (right channel) */
    BitPack_obj->out_ptr_R[3*i + 0] = byte_out0r;
    BitPack_obj->out_ptr_R[3*i + 1] = byte_out1r;
    BitPack_obj->out_ptr_R[3*i + 2] = byte_out2r;
  }

  return;
}





void BitDepacker_init( BitDepack_t *BitDepack_obj,
                       adpcm_data_t *band0_out,
                       adpcm_data_t *band1_out,
                       adpcm_data_t *band2_out,
                       adpcm_data_t *band3_out,
                       adpcm_data_t *band4_out)
{
  sint32 i;
  for(i=0;i<3*BLOCK_SIZE/8;++i)
  {
    BitDepack_obj->inp_buffer0[i] = 0;
    BitDepack_obj->inp_buffer1[i] = 0;
  }

    BitDepack_obj->out_band0_ptr = band0_out;
    BitDepack_obj->out_band1_ptr = band1_out;
    BitDepack_obj->out_band2_ptr = band2_out;
    BitDepack_obj->out_band3_ptr = band3_out;
    BitDepack_obj->out_band4_ptr = band4_out;
  return;
}

/* Sign extension function */
static sint8 SGN_EXT(uint8 x, sint8 nb)
{
  sint8 sb = (x >> (nb-1)) & 0x1;
  if(sb)
  {
    return (x | (0xff<<(nb)));
  }else{
    return x;
  }
}

void BitDepacker_process(BitDepack_t *BitDepack_obj)
{
  sint32 i; /* Iterator */
  
  for(i=0;i<3*BLOCK_SIZE/8/3;++i)
  {
    bitstream byte0l,byte0r; 
    bitstream byte1l,byte1r; 
    bitstream byte2l,byte2r; 

    /* Load three byte packed data to local variables (left channel) */
    byte0l = BitDepack_obj->inp_buffer0[3*i + 0];
    byte1l = BitDepack_obj->inp_buffer0[3*i + 1];
    byte2l = BitDepack_obj->inp_buffer0[3*i + 2];

    /* Load three byte packed data to local variables (right channel) */
    byte0r = BitDepack_obj->inp_buffer1[3*i + 0];
    byte1r = BitDepack_obj->inp_buffer1[3*i + 1];
    byte2r = BitDepack_obj->inp_buffer1[3*i + 2];

    /* Split three byte packed data into individual outputs for each sub-band 0-4 */
    BitDepack_obj->out_band0_ptr[i].l = SGN_EXT((byte0l & 0x1f),5);
    BitDepack_obj->out_band0_ptr[i].r = SGN_EXT((byte0r & 0x1f),5);

    BitDepack_obj->out_band1_ptr[i].l = SGN_EXT((byte1l & 0x1f),5);
    BitDepack_obj->out_band1_ptr[i].r = SGN_EXT((byte1r & 0x1f),5);

    BitDepack_obj->out_band3_ptr[2*i+0].l = SGN_EXT((byte0l>>5),3);
    BitDepack_obj->out_band3_ptr[2*i+0].r = SGN_EXT((byte0r>>5),3);

    BitDepack_obj->out_band3_ptr[2*i+1].l = SGN_EXT((byte1l>>5),3);
    BitDepack_obj->out_band3_ptr[2*i+1].r = SGN_EXT((byte1r>>5),3);

    BitDepack_obj->out_band2_ptr[2*i+0].l = SGN_EXT((byte2l & 0x0f),4);
    BitDepack_obj->out_band2_ptr[2*i+0].r = SGN_EXT((byte2r & 0x0f),4);

    BitDepack_obj->out_band2_ptr[2*i+1].l = SGN_EXT((byte2l>>4),4);
    BitDepack_obj->out_band2_ptr[2*i+1].r = SGN_EXT((byte2r>>4),4);

    BitDepack_obj->out_band4_ptr[2*i+0].l = 0;
    BitDepack_obj->out_band4_ptr[2*i+0].r = 0;
    BitDepack_obj->out_band4_ptr[2*i+1].l = 0;
    BitDepack_obj->out_band4_ptr[2*i+1].r = 0;
  }

  return;
}

