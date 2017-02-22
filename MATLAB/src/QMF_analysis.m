%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Function : QMF_analysis
%
%  Inputs 
%        x  : Input Signal
%        h  : Filter Taps (even)
% history_in: History to implement overlap add
%     mode  : 0 => Fixed Point (default), 1 => Full Precision
%
%  Outputs
%        y0    : Output low branch
%        y1    : Output high branch
%  history_out : parameters to initialize subsequent overlap add block 

function [y0, y1, history_out] = QMF_analysis(x, h, history_in, mode)

L = length(x)/2; % block size assumed to be even
M = length(h)/2;


h0 = fliplr(h);
e0 = h0(1:2:2*M); e1 = h0(2:2:2*M);

x0 = [zeros(M-1,1);x(1:2:2*L);zeros(M-1,1)]; 
x1 = [zeros(M-1,1);x(2:2:2*L);zeros(M-1,1)];
if(mode == 1)
  for i=1:L+M-1
   
    d0 = x0(i:i+M-1); d1 = x1(i:i+M-1);
    history_out(i,1) = history_in(i,1) + e0*d0;
    history_out(i,2) = history_in(i,2) + e1*d1;
  end
  y0 = history_out(1:L,1) + history_out(1:L,2);
  y1 = history_out(1:L,1) - history_out(1:L,2);
else
    
  for i=1:L+M-1
   
    d0 = x0(i:i+M-1); d1 = x1(i:i+M-1);
    history_out(i,1) = history_in(i,1) + PACKH(e0*d0);
    history_out(i,2) = history_in(i,2) + PACKH(e1*d1);
  end
  y0 = (history_out(1:L,1) + history_out(1:L,2));
  y1 = (history_out(1:L,1) - history_out(1:L,2));
    
end
  history_out(:,1) = [history_out(L+1:L+M-1,1);zeros(L,1)];
  history_out(:,2) = [history_out(L+1:L+M-1,2);zeros(L,1)];
end