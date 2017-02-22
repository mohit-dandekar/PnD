%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Unit Test for QMF synthesis bank
%
%  + Takes input wav file and filters through PR filter bank and writes out
%    intermediate bank outputs and final synthesis output
%  + Writes the testvectors: bank_data, filter, expected synth. output
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
fprintf(f2,'QMF synthesis');
fclose(f2);
fprintf('#################################################################\n');
printtime();
fprintf('     Unit Testcase for QMF synthesis filter bank                 \n\n');





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

a_history1l = zeros(N/2/P+M-1,2);
a_history1r = zeros(N/2/P+M-1,2);
s_history1l = zeros(N/2/P+M-1,2);
s_history1r = zeros(N/2/P+M-1,2);
start_indx = [1 1 1 1]; end_indx = [N/P N/2/P N/4/P N/8/P];
incr = end_indx;
fprintf('Synthesis Filter Bank\n'); fprintf('Block : [%d]',P);
for i=1:P

fprintf('%8d',i);%fflush(1);

[bank0l(start_indx(2):end_indx(2),1), bank1l(start_indx(2):end_indx(2),1), a_history1l] = QMF_analysis(yl(start_indx(1):end_indx(1),1), h0, a_history1l,mode);

[bank0r(start_indx(2):end_indx(2),1), bank1r(start_indx(2):end_indx(2),1), a_history1r] = QMF_analysis(yr(start_indx(1):end_indx(1),1), h0, a_history1r,mode);


[synth_outl(start_indx(1):end_indx(1),1), s_history1l] = QMF_synthesis(bank0l(start_indx(2):end_indx(2),1), bank1l(start_indx(2):end_indx(2),1), f0, s_history1l,mode);

[synth_outr(start_indx(1):end_indx(1),1), s_history1r] = QMF_synthesis(bank0r(start_indx(2):end_indx(2),1), bank1r(start_indx(2):end_indx(2),1), f0, s_history1r,mode);

fprintf('\b\b\b\b\b\b\b\b');
start_indx = start_indx + incr;
end_indx = end_indx + incr;
end
fprintf('\n');

snrsegval = snrseg(synth_outl(1*(M-2)+1:N), yl(1:N-1*(M-2)), fs);
fprintf('SNRSEG L= %2d\n',snrsegval);
snrsegval = snrseg(synth_outl(1*(M-2)+1:N), yl(1:N-1*(M-2)), fs);
fprintf('SNRSEG R= %2d\n',snrsegval);
fprintf('Writing Testvectors\n');

fl0 = fopen('./test_vectors/filter.txt','w');
for i=1:M
  fprintf(fl0,'%d\n',f0(i));
end
fclose(fl0);


f1 = fopen('./test_vectors/data_expected.txt','w');
for i=1:N
  fprintf(f1,'%d,%d\n',synth_outl(i),synth_outr(i));
end
fclose(f1);

f1 = fopen('./test_vectors/data_in.txt','w');
for i=1:N
  fprintf(f1,'%d,%d\n',yl(i),yr(i));
end
fclose(f1);

f2 = fopen('./test_vectors/bank0.txt','w');
for i=1:N/2
  fprintf(f2,'%d,%d\n',bank0l(i),bank0r(i));
end
fclose(f2);

f3 = fopen('./test_vectors/bank1.txt','w');
for i=1:N/2
  fprintf(f3,'%d,%d\n',bank1l(i),bank1r(i));
end
fclose(f3);




