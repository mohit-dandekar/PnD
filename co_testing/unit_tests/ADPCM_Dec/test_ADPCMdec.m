%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Unit Test for ADPCM Decoder
%
%  + loads input WAV
%  + Processes using ADPCM_coder MATLAB kernel to produce output
%  + Writes the testvectors: input_data, adpcm expected output
%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all;
close all;
%clc;
if(exist('OCTAVE_VERSION', 'builtin') ~= 0)
  pkg load signal;
end
addpath('../../../MATLAB/src');
addpath('../../../MATLAB/utils');

f2 = fopen('./test_vectors/test_name.txt','w');
fprintf(f2,'ADPCM_Decoder');
fclose(f2);
fprintf('#################################################################\n');
printtime();
fprintf('     Unit Testcase for ADPCM Decoder                  \n\n');

load('../../../training_data/words_m')

%yl = convert_to_FXPT((rand(N,1)-0.5),16,15);
%yr = convert_to_FXPT((rand(N,1)-0.5),16,15);
xl = x(1:2:length(x));
yl = resample(xl,8000,16000); % includes anti-alising
yl = yl(1:(floor(length(yl)/10/24))*24);
xr = x(2:2:length(x));
yr = resample(xr,8000,16000); % includes anti-alising
yr = yr(1:(floor(length(yr)/10/24))*24);

yl = convert_to_FXPT(yl,16,15);
yr = convert_to_FXPT(yr,16,15);

L = 24/4;
N = length(yl);
P = N/L;
fprintf('Number of Input blocks = %d  Block Size = %d\n',P,L);

fs = fs/2;

mode = 0;

mu1 = 0.4035;
mu1 = convert_to_FXPT(mu1,16,15); 

nbit_1 = 5;

coder_history_l = [16384 0 0];
coder_history_r = [16384 0 0];
decoder_history_l = [16384 0 0];
decoder_history_r = [16384 0 0];

start_indx = [1]; end_indx = floor([length(yl)]/P);
incr = end_indx;

fprintf('ADPCM Decoder\n');fprintf('Block : [%d]',P);

for i=1:P
  fprintf('%8d',i);
  if(exist('OCTAVE_VERSION', 'builtin') ~= 0)
    fflush(1);
  end    

  [z000l(start_indx(1):end_indx(1)), coder_history_l] = ADPCM_coder(yl(start_indx(1):end_indx(1)), mu1, nbit_1, coder_history_l, mode);
  [z000r(start_indx(1):end_indx(1)), coder_history_r] = ADPCM_coder(yr(start_indx(1):end_indx(1)), mu1, nbit_1, coder_history_r, mode);

  [youtl(start_indx(1):end_indx(1)), decoder_history_l(1,:)] = ADPCM_decoder(z000l(start_indx(1):end_indx(1)), mu1, nbit_1, decoder_history_l(1,:), mode);
  [youtr(start_indx(1):end_indx(1)), decoder_history_r(1,:)] = ADPCM_decoder(z000r(start_indx(1):end_indx(1)), mu1, nbit_1, decoder_history_r(1,:), mode);


  fprintf('\b\b\b\b\b\b\b\b');
  start_indx = start_indx + incr;
  end_indx = end_indx + incr;
end


fprintf('\nWriting Testvectors\n');


f2 = fopen('./test_vectors/data_in.txt','w');
for i=1:N
  fprintf(f2,'%d,%d\n',z000l(i),z000r(i));
end
fclose(f2);

f1 = fopen('./test_vectors/adpcm_expected.txt','w');
for i=1:N
  fprintf(f1,'%d,%d\n',youtl(i),youtr(i));
end
fclose(f1);


