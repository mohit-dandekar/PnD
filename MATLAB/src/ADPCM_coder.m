%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Function : ADPCM_coder
%
%  Inputs 
%        x  : Input PCM stream block to be coded (array)
%       mu  : Prediction parameter (scalar)  
%     nbit  : number of output code bits  
% history_in: parameterss to initialize coding block
%     mode  : 0 => Fixed Point (default) 1 => Full Precision
%
%  Outputs
%        z    : coded stream
%  history_out: parameters to initialize subsequent block  

function [z, history_out] = ADPCM_coder(x, mu, nbit, history_in, mode)

  history_out = history_in;
  N = length(x);
  
  z = zeros(N,1);
  if(nbit ~= 0)
      if(nbit > 5 || nbit < 2)
        error('ERROR : number of bits for ADPCM code out of bound');
      end
      
      % TODO: ineffecient!!, figure out a better way to store const
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
      
      d = zeros(N,1);
               
      
  if(mode == 1)
      step = history_in(1);
      d_dash = step*history_in(2);
      indx = min(floor(abs(history_in(2)*16384)), power(2,nbit-1)-1) + 1;

      x_star = history_in(3);

      for i=1:N
           x_star = mu * (x_star + d_dash);
          
           d(i) = x(i) - x_star;
          
           step = step*step_table(indx);
           inv_step = 1.0/step;
  
           z(i) = min(max((d(i)*inv_step), (-power(2,nbit-1))), (power(2,nbit-1)-1));
           
           indx = min(floor(abs(z(i)*16384)), power(2,nbit-1)-1) + 1;
           d_dash = step * z(i);
      end
  else
      
      step_table = floor(step_table*power(2,13));
      
      
      step = history_in(1);
      d_dash = (MULA(0,step,history_in(2)));
      indx = min(floor(abs(history_in(2))), power(2,nbit-1)-1) + 1;

      x_star = history_in(3);

      for i=1:N
           x_star = PACKH(MULA(0,mu, (x_star + d_dash)));
          
           d(i) = x(i) - x_star;
          
           step = floor((MULA(0,step, step_table(indx))+(power(2,12)))/power(2,13));
           [inv_step,S] = RECIP(step,0);%power(2,15)/step; % Replace by Taylor series approx. DONE!!! mmd 29/03/16

           z(i) = min(max(round((MULA(0,d(i),inv_step))/power(2,15+13-S)), (-power(2,nbit-1))), (power(2,nbit-1)-1));
           
           
           indx = min(floor(abs(z(i))), power(2,nbit-1)-1) + 1;
           d_dash = (MULA(0,step , z(i)));
      end
  end
  
      history_out(1) = step;
      history_out(2) = z(N);
      history_out(3) = x_star;
  end
end
