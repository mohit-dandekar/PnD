%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Function : QMF_synthesis
%
%  Inputs 
%        y0  : Input Signal
%        y1  : Input Signal
%        f  : Filter Taps (even)
% history_in: History to implement overlap add
%     mode  : 0 => Fixed Point (default), 1 => Full Precision
%
%  Outputs
%        x    : Output 
%  history_out : parameters to initialize subsequent overlap add block 

function [x, history_out] = QMF_synthesis(y0, y1, f, history_in, mode)

L = length(y0); % block size assumed to be even
M = length(f)/2;

%r0 = fliplr(f(1:2:2*M)); r1 = fliplr(f(2:2:2*M));
f0 = fliplr(f);
r0 = f0(1:2:2*M); r1 = f0(2:2:2*M);

Y0 = [zeros(M-1,1); y0; zeros(M-1,1)];
Y1 = [zeros(M-1,1); y1; zeros(M-1,1)];
if(mode == 1)
  for i=1:L+M-1
   
    d0 = Y0(i:i+M-1) - Y1(i:i+M-1) ; d1 = Y0(i:i+M-1) + Y1(i:i+M-1);
    history_out(i,1) = history_in(i,1) + r0*d0;
    history_out(i,2) = history_in(i,2) + r1*d1;
  end
  x = reshape([transpose(history_out(1:L,2));transpose(history_out(1:L,1))],2*L,1)    ;
else
  
  for i=1:L+M-1
   
    d0 = Y0(i:i+M-1) + Y1(i:i+M-1) ; d1 = Y0(i:i+M-1) - Y1(i:i+M-1);
    history_out(i,1) = history_in(i,1) + PACKH(r1*d0);
    history_out(i,2) = history_in(i,2) + PACKH(r0*d1);
  end
  x = (reshape([transpose(history_out(1:L,1));transpose(history_out(1:L,2))],2*L,1));
    
end
  history_out(:,1) = [history_out(L+1:L+M-1,1);zeros(L,1)];
  history_out(:,2) = [history_out(L+1:L+M-1,2);zeros(L,1)];
end