%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Function : PACKH
%
%  Inputs 
%        x  : input number assumed q30
%  
%  Outputs
%        y : q15 output
%
function y = PACKH(x)

  y = max(min(floor((x+power(2,14))/power(2,15)),power(2,15)-1),-power(2,15));

end