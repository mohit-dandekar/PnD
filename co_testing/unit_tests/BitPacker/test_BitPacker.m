%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Unit Test for bitpacker bitdepacker
%
%  + Takes input wav file and filters through PR filter bank and writes out
%    final synthesis output
%  + Writes the testvectors: 5 streams 
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
fprintf(f2,'Bitpacker');
fclose(f2);

fprintf('#################################################################\n');
printtime();
fprintf('     Unit Testcase for BitPacker/Depacker                        \n\n');



load('filters_40tap.mat');
load('../../../training_data/words_m')
h0 = convert_to_FXPT(h0,16,15);
f0 = convert_to_FXPT(f0,16,15);

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

M = 40;
L = 24;
N = length(yl);
P = N/24;
fprintf('Number of Input blocks = %d  Block Size = %d\n',P,L);

fs = fs/2;



mode = 0;

history1l = zeros(N/2/P+M-1,2);
history1r = zeros(N/2/P+M-1,2);
history2l = zeros(N/4/P+M-1,2);
history2r = zeros(N/4/P+M-1,2);
history3l = zeros(N/4/P+M-1,2);
history3r = zeros(N/4/P+M-1,2);
history4l = zeros(N/8/P+M-1,2);
history4r = zeros(N/8/P+M-1,2);
start_indx = [1 1 1 1]; end_indx = [N/P N/2/P N/4/P N/8/P];
incr = end_indx;


mu1 = 0.4035;
mu2 = 0.4259;
mu3 = 0.4004;
mu4 = 0.4016;
mu5 = 0.4993;
if(mode == 0)
   mu1 = convert_to_FXPT(mu1,16,15); 
   mu2 = convert_to_FXPT(mu2,16,15); 
   mu3 = convert_to_FXPT(mu3,16,15); 
   mu4 = convert_to_FXPT(mu4,16,15); 
   mu5 = convert_to_FXPT(mu5,16,15); 
end

% Bit allocation is 5 5 4 3 0 in actual use case
nbit_1 = 5;
nbit_2 = 5;
nbit_3 = 4;
nbit_4 = 3;
nbit_5 = 0;

coder_historyl = [[16384 0 0]; [16384 0 0]; [16384 0 0]; [16384 0 0]; [16384 0 0]];
coder_historyr = [[16384 0 0]; [16384 0 0]; [16384 0 0]; [16384 0 0]; [16384 0 0]];

start_indx1 = [1 1 1 1 1]; end_indx1 = floor([N/8 N/8 N/4 N/4 N/4]/P);
incr1 = end_indx1;

fprintf('Block : [%d]',P);
for i=1:P
   
   fprintf('%8d',i);
%fflush(1); 

[bank0l(start_indx(2):end_indx(2),1), bank1l(start_indx(2):end_indx(2),1), history1l] = QMF_analysis(yl(start_indx(1):end_indx(1),1), h0, history1l,mode);

[bank0r(start_indx(2):end_indx(2),1), bank1r(start_indx(2):end_indx(2),1), history1r] = QMF_analysis(yr(start_indx(1):end_indx(1),1), h0, history1r,mode);


[bank0_0l(start_indx(3):end_indx(3),1), bank0_1l(start_indx(3):end_indx(3),1), history2l] = QMF_analysis(bank0l(start_indx(2):end_indx(2),1), h0, history2l,mode);

[bank0_0r(start_indx(3):end_indx(3),1), bank0_1r(start_indx(3):end_indx(3),1), history2r] = QMF_analysis(bank0r(start_indx(2):end_indx(2),1), h0, history2r,mode);



[bank1_0l(start_indx(3):end_indx(3),1), bank1_1l(start_indx(3):end_indx(3),1), history3l] = QMF_analysis(bank1l(start_indx(2):end_indx(2),1), h0, history3l,mode);

[bank1_0r(start_indx(3):end_indx(3),1), bank1_1r(start_indx(3):end_indx(3),1), history3r] = QMF_analysis(bank1r(start_indx(2):end_indx(2),1), h0, history3r,mode);


[bank0_0_0l(start_indx(4):end_indx(4),1), bank0_0_1l(start_indx(4):end_indx(4),1), history4l] = QMF_analysis(bank0_0l(start_indx(3):end_indx(3),1), h0, history4l,mode);

[bank0_0_0r(start_indx(4):end_indx(4),1), bank0_0_1r(start_indx(4):end_indx(4),1), history4r] = QMF_analysis(bank0_0r(start_indx(3):end_indx(3),1), h0, history4r,mode);


  [z000l(start_indx1(1):end_indx1(1)), coder_historyl(1,:)]= ADPCM_coder(bank0_0_0l(start_indx1(1):end_indx1(1)), mu1, nbit_1, coder_historyl(1,:), mode);
  [z000r(start_indx1(1):end_indx1(1)), coder_historyr(1,:)]= ADPCM_coder(bank0_0_0r(start_indx1(1):end_indx1(1)), mu1, nbit_1, coder_historyr(1,:), mode);
 
  
  [z001l(start_indx1(2):end_indx1(2)), coder_historyl(2,:)]= ADPCM_coder(bank0_0_1l(start_indx1(2):end_indx1(2)), mu2, nbit_2, coder_historyl(2,:), mode);
  [z001r(start_indx1(2):end_indx1(2)), coder_historyr(2,:)]= ADPCM_coder(bank0_0_1r(start_indx1(2):end_indx1(2)), mu2, nbit_2, coder_historyr(2,:), mode);
  
  
  [z01l(start_indx1(3):end_indx1(3)), coder_historyl(3,:)]= ADPCM_coder(bank0_1l(start_indx1(3):end_indx1(3)), mu3, nbit_3, coder_historyl(3,:), mode);
  [z01r(start_indx1(3):end_indx1(3)), coder_historyr(3,:)]= ADPCM_coder(bank0_1r(start_indx1(3):end_indx1(3)), mu3, nbit_3, coder_historyr(3,:), mode);
  
  
  [z10l(start_indx1(5):end_indx1(5)), coder_historyl(5,:)]= ADPCM_coder(bank1_0l(start_indx1(5):end_indx1(5)), mu5, nbit_5, coder_historyl(5,:) , mode);
  [z10r(start_indx1(5):end_indx1(5)), coder_historyr(5,:)]= ADPCM_coder(bank1_0r(start_indx1(5):end_indx1(5)), mu5, nbit_5, coder_historyr(5,:) , mode);

  
  [z11l(start_indx1(4):end_indx1(4)), coder_historyl(4,:)] = ADPCM_coder(bank1_1l(start_indx1(4):end_indx1(4)), mu4, nbit_4, coder_historyl(4,:) , mode);
  [z11r(start_indx1(4):end_indx1(4)), coder_historyr(4,:)] = ADPCM_coder(bank1_1r(start_indx1(4):end_indx1(4)), mu4, nbit_4, coder_historyr(4,:) , mode);


   fprintf('\b\b\b\b\b\b\b\b')
start_indx = start_indx + incr; end_indx = end_indx + incr;
start_indx1 = start_indx1 + incr1; end_indx1 = end_indx1 + incr1;
end



f1 = fopen('./test_vectors/band0.txt','w');
for i=1:N/8
  fprintf(f1,'%d,%d\n',z000l(i),z000r(i));
end
fclose(f1);
f1 = fopen('./test_vectors/band1.txt','w');
for i=1:N/8
  fprintf(f1,'%d,%d\n',z001l(i),z001r(i));
end
fclose(f1);
f1 = fopen('./test_vectors/band2.txt','w');
for i=1:N/4
  fprintf(f1,'%d,%d\n',z01l(i),z01r(i));
end
fclose(f1);
f1 = fopen('./test_vectors/band3.txt','w');
for i=1:N/4
  fprintf(f1,'%d,%d\n',z11l(i),z11r(i));
end
f1 = fopen('./test_vectors/band4.txt','w');
for i=1:N/4
  fprintf(f1,'%d,%d\n',z10l(i),z10r(i));
end
fclose(f1);


f1 = fopen('./test_vectors/data_size.txt','w');
fprintf(f1,'%d\n',N);
fclose(f1);



