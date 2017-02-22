%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Proof of concept for Blockwise Processing for subband decomposition
%  --> coding --> decoding --> synthesis
%
%  Does not handle stereo. 
%
%  Fixed point version uses lot of functions and runs quite slow. 
% 
%


clear all;
close all;
clc;

addpath('../src');
addpath('../utils');
%load('')
load('filters_40tap.mat')



mode = 0;  % Fix point => (0) or Floating point => (1) 

 block_size = 336;

[x,fs,nbits,nr_channels] = LoadWav('../../training_data/words_m');
x = x(1:2:length(x));
y = resample(x,8000,16000); % includes anti-alising
y = y(1:(floor(length(y)/block_size))*block_size);
fs = fs/2;


M = length(h0);
N = length(y);
P= N/block_size; % Number of blocks


fprintf('Starting...\n Number of input blocks = %d\n Block Size = %d\n',P,N/P);


if(mode == 0)
   y = convert_to_FXPT(y,16,15);
   h0 = convert_to_FXPT(h0,16,15);
   h1 = convert_to_FXPT(h1,16,15);
   f0 = convert_to_FXPT(f0,16,15);
   f1 = convert_to_FXPT(f1,16,15);
end




history1 = zeros(N/2/P+M-1,2);
history2 = zeros(N/4/P+M-1,2);
history3 = zeros(N/4/P+M-1,2);
history4 = zeros(N/8/P+M-1,2);
start_indx = [1 1 1 1]; end_indx = [N/P N/2/P N/4/P N/8/P];
incr = end_indx;

for i=1:P

[bank0(start_indx(2):end_indx(2),1), bank1(start_indx(2):end_indx(2),1), history1] = QMF_analysis(y(start_indx(1):end_indx(1),1), h0, history1,mode);

[bank0_0(start_indx(3):end_indx(3),1), bank0_1(start_indx(3):end_indx(3),1), history2] = QMF_analysis(bank0(start_indx(2):end_indx(2),1), h0, history2,mode);

[bank1_0(start_indx(3):end_indx(3),1), bank1_1(start_indx(3):end_indx(3),1), history3] = QMF_analysis(bank1(start_indx(2):end_indx(2),1), h0, history3,mode);

[bank0_0_0(start_indx(4):end_indx(4),1), bank0_0_1(start_indx(4):end_indx(4),1), history4] = QMF_analysis(bank0_0(start_indx(3):end_indx(3),1), h0, history4,mode);

start_indx = start_indx + incr;
end_indx = end_indx + incr;
end

bank0_1 = [zeros(M-2,1);bank0_1(1:N/4-M+2)];
bank1_0 = [zeros(M-2,1);bank1_0(1:N/4-M+2)];
bank1_1 = [zeros(M-2,1);bank1_1(1:N/4-M+2)];

fprintf('Subband Decompostion Done\n');


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

coder_history = [[16384 0 0]; [16384 0 0]; [16384 0 0]; [16384 0 0]; [16384 0 0]];
decoder_history = [[16384 0 0]; [16384 0 0]; [16384 0 0]; [16384 0 0]; [16384 0 0]];
start_indx = [1 1 1 1 1]; end_indx = floor([length(bank0_0_0) length(bank0_0_1) length(bank0_1) length(bank1_1) length(bank1_0)]/P);
incr = end_indx;

for i=1:P
    
  [z000(start_indx(1):end_indx(1)), coder_history(1,:)]= ADPCM_coder(bank0_0_0(start_indx(1):end_indx(1)), mu1, nbit_1, coder_history(1,:), mode);
 
  
  [z001(start_indx(2):end_indx(2)), coder_history(2,:)]= ADPCM_coder(bank0_0_1(start_indx(2):end_indx(2)), mu2, nbit_2, coder_history(2,:), mode);
  
  
  [z01(start_indx(3):end_indx(3)), coder_history(3,:)]= ADPCM_coder(bank0_1(start_indx(3):end_indx(3)), mu3, nbit_3, coder_history(3,:), mode);
  
  
  [z10(start_indx(5):end_indx(5)), coder_history(5,:)]= ADPCM_coder(bank1_0(start_indx(5):end_indx(5)), mu5, nbit_5, coder_history(5,:) , mode);

  
  [z11(start_indx(4):end_indx(4)), coder_history(4,:)] = ADPCM_coder(bank1_1(start_indx(4):end_indx(4)), mu4, nbit_4, coder_history(4,:) , mode);

  


  [bank0_0_0(start_indx(1):end_indx(1)), decoder_history(1,:)] = ADPCM_decoder(z000(start_indx(1):end_indx(1)), mu1, nbit_1, decoder_history(1,:), mode);

  [bank0_0_1(start_indx(2):end_indx(2)), decoder_history(2,:)] = ADPCM_decoder(z001(start_indx(2):end_indx(2)), mu2, nbit_2, decoder_history(2,:), mode);

  [bank0_1(start_indx(3):end_indx(3)), decoder_history(3,:)] = ADPCM_decoder(z01(start_indx(3):end_indx(3)), mu3, nbit_3, decoder_history(3,:), mode);

  [bank1_0(start_indx(5):end_indx(5)), decoder_history(5,:)] = ADPCM_decoder(z10(start_indx(5):end_indx(5)), mu5, nbit_5, decoder_history(5,:), mode);

  [bank1_1(start_indx(4):end_indx(4)), decoder_history(4,:) ] = ADPCM_decoder(z11(start_indx(4):end_indx(4)), mu4, nbit_4, decoder_history(4,:), mode);

  
   start_indx  = start_indx+incr;end_indx  = end_indx+incr;
end

fprintf('Encoding Decoding Done\n');
  
figure;
subplot(5,1,1);plot(z000);title('z000');
subplot(5,1,2);plot(z001);title('z001');
subplot(5,1,3);plot(z01);title('z01');
subplot(5,1,4);plot(z11);title('z11');
subplot(5,1,5);plot(z10);title('z10');



history1 = zeros(N/2/P+M-1,2);
history2 = zeros(N/4/P+M-1,2);
history3 = zeros(N/4/P+M-1,2);
history4 = zeros(N/8/P+M-1,2);

start_indx = [1 1 1 1]; end_indx = [N/P N/2/P N/4/P N/8/P];
incr = end_indx;

for i=1:P

[syn_bank0_0(start_indx(3):end_indx(3),1) , history4] = QMF_synthesis(bank0_0_0(start_indx(4):end_indx(4),1), bank0_0_1(start_indx(4):end_indx(4),1), f0, history4,mode);

[syn_bank0(start_indx(2):end_indx(2),1), history3] = QMF_synthesis(syn_bank0_0(start_indx(3):end_indx(3),1), bank0_1(start_indx(3):end_indx(3),1), f0, history3,mode);

[syn_bank1(start_indx(2):end_indx(2),1), history2] = QMF_synthesis(bank1_0(start_indx(3):end_indx(3),1), bank1_1(start_indx(3):end_indx(3),1), f0, history2,mode);

[synth_out(start_indx(1):end_indx(1),1), history1] = QMF_synthesis(syn_bank0(start_indx(2):end_indx(2),1), syn_bank1(start_indx(2):end_indx(2),1), f0, history1,mode);

start_indx  = start_indx+incr;end_indx  = end_indx+incr;
end

fprintf('Synthesis Done\n');

pesqval = pesq(y(1:N-7*(M-1)), synth_out(7*(M-1)+1:N), fs);
snrsegval = snrseg(synth_out(7*(M-2)+1:N), y(1:N-7*(M-2)), fs);
if(mode == 0)
    fprintf('Fixed Point computation enabled\n');
else
    fprintf('Floating Point computation enabled\n');
end
fprintf('PESQ = %f  , SNRseg = %f\n',pesqval,snrsegval);


figure;
subplot(2,1,1);
plot([0:1/N:1-1/N]*fs,(abs(fft(y))));xlabel('Frequency (Hz)');ylabel('');title('Input Frequency Spectrum');
subplot(2,1,2);
plot([0:1/length(synth_out):1-1/length(synth_out)]*fs,(abs(fft(synth_out))));xlabel('Frequency (Hz)');ylabel('');title('Synthesis Output Frequency Spectrum');
