%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Function : RECIP
%
%  Inputs 
%        x : Input number
%     mode : 0 => Fixed Point (default) 1 => Full Precision
%
%  Outputs
%        y : 1/x in the appropriate format. 
%        S : scale factor for output
function [y,S] = RECIP(x,  mode)

 if((nargin < 4) || isempty(mode))
    mode = 0; 
 end

 if(mode == 0)
     %y = max(min(round(power(2,nfrac)./round(x)),power(2,nbit)-1),-power(2,nbit)); 
     %TODO replace with taylor series approximation
     x0 = power(2,14) + [0:1:31]*power(2,9) + power(2,8);
     one_by_x0 = round((32768./x0)*power(2,13));
     one_by_x0sq = round(((32768./x0).^2)*power(2,13));
     xin = x;
     S = 0;
     while(xin*power(2,S)<16384)
       S = S+1;
     end
     xin = xin*power(2,S);
     indx =  floor((xin - 16384)/power(2,9));
     delta = bitand(xin,power(2,9)-1) - power(2,8);
     y = round(one_by_x0(indx+1) - one_by_x0sq(indx+1)*delta/32768);
  else
     y = (1.0)./x;
     S = 0;
 end
end