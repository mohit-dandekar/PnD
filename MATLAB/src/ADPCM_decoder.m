%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Function : ADPCM_decoder
%
%  Inputs 
%        z  : Input APCM stream block to be decoded (array)
%       mu  : Prediction parameter (scalar)  
%     nbit  : number of output code bits
% history_in: parameters to initialize decoding block
%     mode  : 0 => Fixed Point (default) 1 => Full Precision
%
%  Outputs
%        x    : decoded stream
%  history_out: parameters to initialize subsequent block  
function [x, history_out] = ADPCM_decoder(z, mu, nbit, history_in, mode)

 
  history_out = history_in;
  N = length(z);

  x = zeros(N,1);
  if(nbit ~= 0)
      % TODO: ineffecient !!, figure out a better way to store const
      % tables and only chose at runtime
      if(nbit == 2)
         step_table    =  [0.8 1.6];
      elseif(nbit == 3)
         step_table    =  [0.9 0.9 1.5 2.75];
      elseif(nbit == 4)
         step_table    =  [0.9 0.9 0.9 0.9 1.2 1.6 2.0 2.4];
      elseif(nbit == 5)
         step_table    =  [0.9 0.9 0.9 0.9 .95 .95 .95 .95 1.2 1.5 1.8 2.1 2.4 2.7 3.0 3.3];
      end
  if(mode == 1)
      step = history_in(1);
      d_dash = step*history_in(2);
      x_star = history_in(3);
      indx = min(floor(abs(history_in(2))*16384), power(2,nbit-1)-1) + 1;
      
      for i=1:N
 
          x_star = mu * (x_star + d_dash);
          
          step = step * step_table(indx);
          indx = min(floor(abs(z(i))*16384), power(2,nbit-1)-1) + 1;
          
          d_dash = z(i)*step;
          
          x(i) = x_star + d_dash;
      end
      
  else
           step_table = floor(step_table*power(2,13));
      
      step = history_in(1);
      d_dash = (MULA(0,step,history_in(2)));
      x_star = history_in(3);
      indx = min(floor(abs(history_in(2))), power(2,nbit-1)-1) + 1;
      
      for i=1:N
 
          x_star = PACKH(MULA(0,mu , (x_star + d_dash)));
          
          step = floor((MULA(0,step , step_table(indx)) + power(2,12) )/power(2,13));
          indx = min(floor(abs(z(i))), power(2,nbit-1)-1) + 1;
          
          d_dash = (MULA(0,z(i),step));
          
          x(i) = x_star + d_dash;
      end
      
  end
      history_out(1) = step;
      history_out(2) = z(N);
      history_out(3) = x_star;
  end
end