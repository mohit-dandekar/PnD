function printtime()

time = clock();

if(time(4)>12)
  hour = time(4)-12;
  part = 'PM';
elseif(time(4)<12)
  hour = time(4);
  part = 'AM';
elseif(time(4) == 0)
 hour = 12;
 part = 'AM';
elseif(time(4) == 12)
  hour = 12;
  part = 'PM';
end
fprintf('%d/%d/%d  %d:%d:%02d %s\n',time(3),time(2),time(1),hour,time(5),round(time(6)),part);

end