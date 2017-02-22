%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Unit Test for QMF analysis-synthesis bank
%
%  + Takes input wav file and filters through PR filter bank and writes out
%    final synthesis output
%  + Writes the testvectors: data_in, filters, expected synth. output
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
fprintf(f2,'analysis synthesis');
fclose(f2);
fprintf('#################################################################\n');
printtime();

fprintf('     Unit Testcase for QMF analysis-synthesis filter bank                 \n\n');



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

fprintf('Starting Full blockwise SB Analysis filtering\n');
fprintf('Block : [%d]',P);
for i=1:P
   
   fprintf('%8d',i);%fflush(1); 

[bank0l(start_indx(2):end_indx(2),1), bank1l(start_indx(2):end_indx(2),1), history1l] = QMF_analysis(yl(start_indx(1):end_indx(1),1), h0, history1l,mode);

[bank0r(start_indx(2):end_indx(2),1), bank1r(start_indx(2):end_indx(2),1), history1r] = QMF_analysis(yr(start_indx(1):end_indx(1),1), h0, history1r,mode);


[bank0_0l(start_indx(3):end_indx(3),1), bank0_1l(start_indx(3):end_indx(3),1), history2l] = QMF_analysis(bank0l(start_indx(2):end_indx(2),1), h0, history2l,mode);

[bank0_0r(start_indx(3):end_indx(3),1), bank0_1r(start_indx(3):end_indx(3),1), history2r] = QMF_analysis(bank0r(start_indx(2):end_indx(2),1), h0, history2r,mode);



[bank1_0l(start_indx(3):end_indx(3),1), bank1_1l(start_indx(3):end_indx(3),1), history3l] = QMF_analysis(bank1l(start_indx(2):end_indx(2),1), h0, history3l,mode);

[bank1_0r(start_indx(3):end_indx(3),1), bank1_1r(start_indx(3):end_indx(3),1), history3r] = QMF_analysis(bank1r(start_indx(2):end_indx(2),1), h0, history3r,mode);


[bank0_0_0l(start_indx(4):end_indx(4),1), bank0_0_1l(start_indx(4):end_indx(4),1), history4l] = QMF_analysis(bank0_0l(start_indx(3):end_indx(3),1), h0, history4l,mode);

[bank0_0_0r(start_indx(4):end_indx(4),1), bank0_0_1r(start_indx(4):end_indx(4),1), history4r] = QMF_analysis(bank0_0r(start_indx(3):end_indx(3),1), h0, history4r,mode);

   fprintf('\b\b\b\b\b\b\b\b')
start_indx = start_indx + incr; end_indx = end_indx + incr;
end
bank0_1l = [zeros(M-2,1);bank0_1l(1:N/4-M+2)];
bank0_1r = [zeros(M-2,1);bank0_1r(1:N/4-M+2)];
bank1_0l = [zeros(M-2,1);bank1_0l(1:N/4-M+2)];
bank1_0r = [zeros(M-2,1);bank1_0r(1:N/4-M+2)];
bank1_1l = [zeros(M-2,1);bank1_1l(1:N/4-M+2)];
bank1_1r = [zeros(M-2,1);bank1_1r(1:N/4-M+2)];



fprintf('\nStarting Full blockwise SB Synthesis filtering\n');
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
fprintf('Block : [%d]',P);
for i=1:P
   fprintf('%8d',i);%fflush(1); 

[syn_bank0_0l(start_indx(3):end_indx(3),1) , history4l] = QMF_synthesis(bank0_0_0l(start_indx(4):end_indx(4),1), bank0_0_1l(start_indx(4):end_indx(4),1), f0, history4l,mode);

[syn_bank0_0r(start_indx(3):end_indx(3),1) , history4r] = QMF_synthesis(bank0_0_0r(start_indx(4):end_indx(4),1), bank0_0_1r(start_indx(4):end_indx(4),1), f0, history4r,mode);


[syn_bank0l(start_indx(2):end_indx(2),1), history3l] = QMF_synthesis(syn_bank0_0l(start_indx(3):end_indx(3),1), bank0_1l(start_indx(3):end_indx(3),1), f0, history3l,mode);

[syn_bank0r(start_indx(2):end_indx(2),1), history3r] = QMF_synthesis(syn_bank0_0r(start_indx(3):end_indx(3),1), bank0_1r(start_indx(3):end_indx(3),1), f0, history3r,mode);



[syn_bank1l(start_indx(2):end_indx(2),1), history2l] = QMF_synthesis(bank1_0l(start_indx(3):end_indx(3),1), bank1_1l(start_indx(3):end_indx(3),1), f0, history2l,mode);

[syn_bank1r(start_indx(2):end_indx(2),1), history2r] = QMF_synthesis(bank1_0r(start_indx(3):end_indx(3),1), bank1_1r(start_indx(3):end_indx(3),1), f0, history2r,mode);



[synth_outl(start_indx(1):end_indx(1),1), history1l] = QMF_synthesis(syn_bank0l(start_indx(2):end_indx(2),1), syn_bank1l(start_indx(2):end_indx(2),1), f0, history1l,mode);

[synth_outr(start_indx(1):end_indx(1),1), history1r] = QMF_synthesis(syn_bank0r(start_indx(2):end_indx(2),1), syn_bank1r(start_indx(2):end_indx(2),1), f0, history1r,mode);


fprintf('\b\b\b\b\b\b\b\b');
start_indx = start_indx + incr; end_indx = end_indx + incr;
end
fprintf('\n');

snrsegval = snrseg(synth_outl(7*(M-2)+1:N), yl(1:N-7*(M-2)), fs);
fprintf('SNRSEG L= %2d\n',snrsegval);
snrsegval = snrseg(synth_outl(7*(M-2)+1:N), yl(1:N-7*(M-2)), fs);
fprintf('SNRSEG R= %2d\n',snrsegval);
fprintf('Writing Testvectors\n');



fl0 = fopen('./test_vectors/Afilter.txt','w');
for i=1:M
  fprintf(fl0,'%d\n',h0(i));
end
fclose(fl0);

fl0 = fopen('./test_vectors/Sfilter.txt','w');
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

f1 = fopen('./test_vectors/data_size.txt','w');
fprintf(f1,'%d\n',N);
fclose(f1);



