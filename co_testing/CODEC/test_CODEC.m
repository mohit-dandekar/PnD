%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  
%
%  
%  
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all;
close all;
%clc;

arg_list = [];
if(exist('OCTAVE_VERSION', 'builtin') ~= 0)
 pkg load signal;
 arg_list = argv ();
 
end

fp = fopen('filename.txt');
filename = fscanf(fp,'%s');
fclose(fp);

addpath('../../MATLAB/src');
addpath('../../MATLAB/utils');
addpath('../../training_data');

fprintf('#################################################################\n');
printtime();
fprintf('     Unit Testcase for Full CODEC: filename: %s                   \n\n',filename);


f2 = fopen('./test_vectors/test_name.txt','w');
fprintf(f2,'FULL CODEC');
fclose(f2);



[x,fs,nbits,nr_channels] = LoadWav(filename);
xl = x(1:2:length(x));
xr = x(2:2:length(x));


xl = resample(xl,fs/2,fs);
xr = resample(xr,fs/2,fs);
fs = fs/2;

wavwrite(xl, fs, 'input.wav');




