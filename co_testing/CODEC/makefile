###########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding 
#  
#  Mohit Dandekar
#  John O'Callaghan
###########################################################################

TOPDIR = ../..

FNAMES = ../../training_data/belasting.wav\
	../../training_data/bir.wav\
	../../training_data/f116.wav\
	../../training_data/f216.wav\
	../../training_data/m116.wav\
	../../training_data/m216.wav\
	../../training_data/words_f.wav\
	../../training_data/words_m.wav\

SRC =               \
	main.c      \
	QMF.c       \
	SBC.c       \
	ADPCM.c     \
	BitPacker.c     \
	ADPCM_SBC.c \
	wavpcm_io.c \

TNAME = CODEC
EXECUTABLE = ex_test_$(TNAME).out

# Can be set by user to multiple of 16
ifndef $(BLOCKSIZE)
  BLOCKSIZE = 48   
endif

-include $(TOPDIR)/scripts/common.mk
