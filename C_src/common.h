/*#########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding 
#  
#  Mohit Dandekar
#  John O'Callaghan
#########################################################################*/

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

/*Integer Data types to be used*/
typedef  uint8_t     uint8;
typedef   int8_t     sint8;
typedef uint16_t    uint16;
typedef  int16_t    sint16;
typedef uint32_t    uint32;
typedef  int32_t    sint32;
typedef uint64_t    uint64;
typedef  int64_t    sint64;

typedef uint8_t bitstream ;

#include "globals.h"

#ifdef BUFFERSIZE
  #define BLOCK_SIZE BUFFERSIZE/2
#else
  #define BLOCK_SIZE 24
#endif
#define FILT_LEN 40



/*Custom Data types*/
/*16 bit PCM audio data type*/
typedef struct aud_data_t
{
  sint16 l; /*Left channel sample*/
  sint16 r; /*Right channel sample*/
}aud_data_t;

/*Wide accumulator type 40 bit */
typedef struct aud_data_w_t
{
  sint64 l; /*Left channel wide type*/
  sint64 r; /*Right channel wide type*/
}aud_data_w_t; 

/*8 bit container for 5/4/3/2 bit ADPCM audio data type*/
typedef struct adpcm_data_t
{
  sint8 l; /*Left channel sample*/
  sint8 r; /*Right channel sample*/
}adpcm_data_t;



/*Use macto to align variable address to a 16 byte boundary*/
#ifndef ALIGN16
  #define ALIGN16 __attribute__((aligned(16))) 
#endif

/* Operations */
#define DADD(_z, _x, _y){      \
                               \
            _z.l = _x.l + _y.l;\
            _z.r = _x.r + _y.r;\
        }
 

#define DSUB(_z, _x, _y){      \
                               \
            _z.l = _x.l - _y.l;\
            _z.r = _x.r - _y.r;\
        }
 

#define DMULA(_accum, _x, _y){                          \
             aud_data_w_t _temp;                        \
             _temp.l = _accum.l + (sint64)(_x.l * _y.l);\
             _temp.r = _accum.r + (sint64)(_x.r * _y.r);\
             _accum = _temp;                            \
        }

#define DMUL(_accum, _x, _y){                \
             aud_data_w_t _temp;             \
             _temp.l = (sint64)(_x.l * _y.l);\
             _temp.r = (sint64)(_x.r * _y.r);\
             _accum = _temp;                 \
        }



#define DSMULA(_accum, _x, _y){                                        \
                 aud_data_w_t _temp;                                   \
             _temp.l = _accum.l +  (sint64)((sint32)_x.l * (sint32)_y);\
             _temp.r = _accum.r +  (sint64)((sint32)_x.r * (sint32)_y);\
             _accum = _temp;                                           \
        }

#define DSMUL(_accum, _x, _y){                             \
                 aud_data_w_t _temp;                       \
             _temp.l = (sint64)((sint32)_x.l * (sint32)_y);\
             _temp.r = (sint64)((sint32)_x.r * (sint32)_y);\
             _accum = _temp;                               \
        }

#define DPACKV(_x, _wx, _sl, _sr){                                  \
             aud_data_t _temp;                                      \
             _temp.l = (sint16)((_wx.l + (1<<(_sl-1))) >> _sl);     \
             _temp.r = (sint16)((_wx.r + (1<<(_sr-1))) >> _sl);     \
             _x = _temp;                                            \
        }

#define DPACKL(_x, _wx){                    \
             aud_data_t _temp;              \
             _temp.l = (sint16)(_wx.l);     \
             _temp.r = (sint16)(_wx.r);     \
             _x = _temp;                    \
        }


#define SATN(_t,_x,_liml,_limh){\
          if(_x >= _liml)\
          {\
            if(_x <= _limh)\
            {\
              _t = _x;\
            }else{ _t = _limh;}\
          }else{ _t = _liml;}\
        }                           
         

#endif /*__COMMON_H__*/
