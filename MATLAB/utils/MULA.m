%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%  P & D Embedded Systems and Multimedia [H09M0a] 2015-2016
%  Subband-Coding 
%
%  Mohit Dandekar
%  John O'Callaghan
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% Function : MULA
%
%  Inputs 
%     accum : accumulator
%        x  : input0
%        y  : input1
%  
%  Outputs
%        accum_out : accum + x*y
%
function accum_out = MULA(accum, x, y)

  accum_out = min(max(round(accum + x*y),-power(2,40)),power(2,40)-1);

end