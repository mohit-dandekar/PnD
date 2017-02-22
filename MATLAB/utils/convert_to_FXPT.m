%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Function : convert_to_FXPT
%
%  Inputs 
%        x  : Input full precision number (signed)(array)
%      nbits: number of total bits
%     nfrac : number of fractional bits
%  Outputs
%        y : Fixed point number
% 
function y = convert_to_FXPT(x, nbits, nfrac)

%  input number assumed to be full precision signed and fitting the 
% desired range. Inputs outside range will saturate

  maxlim = power(2, nbits-1);
  scale = power(2,min(nfrac,nbits-1));
  
  if(nfrac >= nbits)
      fprintf('WARNING :\n (function : convert_to_FXPT)\n number of fractional bits >= number of bits in word.\n Limiting fractional bits to wordsize - 1\n');
  end
  
  y = max((min(round(x.*scale) ,(maxlim-1))),(-maxlim));
end