###########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding 
#  
#  Mohit Dandekar
#  John O'Callaghan
###########################################################################
TOPDIR = ..

DIRLIST =\
	./unit_tests/ADPCM_Enc\
        ./unit_tests/ADPCM_Dec\
	./unit_tests/BitPacker\
	./unit_tests/QMF_Analysis\
	./unit_tests/QMF_Synthesis\
	./unit_tests/QMF_2bank_FB\
	./unit_tests/QMF_full_FB\

unittest:
	$(foreach dir, $(DIRLIST), make  --no-print-directory -C $(dir) clean run;)


fullcodec:
	make --no-print-directory -C ./CODEC clean runfull 

cleanall:
	@echo '################ Cleaning All Tests #################'
	$(foreach dir, $(DIRLIST), make --no-print-directory -C $(dir) clean;)
	make  --no-print-directory -C ./CODEC clean
	rm -f RESULTS.txt RESULTS.txt.bak
	@echo '################       Done!!!      #################'
	
