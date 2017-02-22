%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Function : convert_to_FLPT
%
%  Inputs 
%        x  : Input fixed point number (signed)(array)
%      nbits: number of total bits
%     nfrac : number of fractional bits
%  Outputs
%        y  : Floating point
% 
function y = convert_to_FLPT(x, nbits, nfrac)

%  input number assumed to be fixed point signed 

  if(nfrac >= nbits)
      fprintf('WARNING :\n (function : convert_to_FLPT)\n number of fractional bits >= number of bits in word.\n Limiting fractional bits to wordsize - 1\n');
  end

  scale = power(2,nfrac);
  y = (x .* 1.0)./scale;
end