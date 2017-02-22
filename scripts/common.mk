###########################################################################
#  P & D Embedded Systems and Multimedia [H09M0a]
#  Subband-Coding 
#  
#  Mohit Dandekar
#  John O'Callaghan
###########################################################################

VPATH = \
	$(TOPDIR)/C_src\

SEARCH = \
	-I '$(TOPDIR)/C_src'

# Variables
CC = gcc
MAKE = make
DEL = rm -Rf

MCOMMAND = octave

#Compiler and linker flags
#CFLAGS += -O0 -pg -g -r -Wall -c -Wextra -std=gnu99
CFLAGS += -O0 -pg -g -r -ansi -Wall -c -Wextra -std=c99

LFLAGS += 

LIBS += -lm -pg -g


OBJS += $(SRC:.c=.o)

LINK = @$(CC) $(LFLAGS) -o '$@' $(OBJS) $(LIBS)

BUILD = @$(CC) $(CFLAGS) $(SEARCH) -o'$@' $< -DBUFFERSIZE=$(BLOCKSIZE)

#Explicit Rules
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(LINK)

%.o: %.c
	$(BUILD)

# header dependency
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $(SEARCH) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

 -include $(SRC:.c=.d)


RESFILE = RESULTS.txt;

clean:
	-$(DEL) $(OBJS) $(OBJS:.o=.d) $(EXECUTABLE) *.out test_vectors $(PESQ) pesq_results.txt input.wav output.wav $(RESFILE)

TVDIR = test_vectors/test_name.txt

$(TVDIR): 
	@mkdir -p test_vectors
	  

run: $(EXECUTABLE) $(TVDIR)
	@octave --silent --eval "pkg load signal"
	@if [ 0 -eq $? ]; then\
		octave test_$(TNAME).m | tee -a $(TOPDIR)/co_testing/RESULTS.txt; \
	else\
		matlab -nodisplay -nosplash -nodesktop -r "run test_$(TNAME).m ; exit" | tee -a $(TOPDIR)/co_testing/RESULTS.txt;\
	fi  && ./$(EXECUTABLE) | tee -a $(TOPDIR)/co_testing/RESULTS.txt
	@sed -i.bak '/Block :/d' $(TOPDIR)/co_testing/RESULTS.txt


rundbg: $(EXECUTABLE)
	gdb -tui $(EXECUTABLE)

# PESQ Measurement
PESQ = measure_pesq.out

$(PESQ):
	@gcc -w -o $@ $(TOPDIR)/ITU_PESQ/Software/P862_annex_A_2005_CD/source/*.c -lm

runpesq: $(PESQ) 
	@echo $(TOPDIR)/co_testing/CODEC/$(FNAME) >$(TOPDIR)/co_testing/CODEC/filename.txt
	@make --no-print-directory run	
	@echo '                                                                 ' | tee -a $(TOPDIR)/co_testing/RESULTS.txt;
	@echo '                         PESQ SCORE                              ' | tee -a $(TOPDIR)/co_testing/RESULTS.txt;
	@echo '                                                                 ' | tee -a  $(TOPDIR)/co_testing/RESULTS.txt;
	@./$(PESQ) +8000 input.wav output.wav | grep MOS | tee -a  $(TOPDIR)/co_testing/RESULTS.txt;
	@echo '#################################################################' | tee -a  $(TOPDIR)/co_testing/RESULTS.txt;
	@echo '                                                                 ' | tee -a  $(TOPDIR)/co_testing/RESULTS.txt;
	@echo ''
	@echo ''

runfull: $(EXECUTABLE) $(TVDIR)
	$(foreach name, $(FNAMES), echo $(TOPDIR)/co_testing/CODEC/$(name) >$(TOPDIR)/co_testing/CODEC/filename.txt\
	;make --no-print-directory runpesq FNAME=$(name) ; )



