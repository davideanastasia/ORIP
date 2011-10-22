function [mat] = read_mat(image_file,offset,type,rows,cols)

% syntax: [mat] = read_mat(image_file,offset,type,rows,cols)
% Reads in binary a <rows> x <cols> matrix of type <type> from file <image_file> and 
% returns the result in matrix <mat>. <offset> is the offset in the file in bytes.
% <type> is a string as it is defined in fread() for the type of the input data.


fin=fopen(image_file,'rb');

if fseek(fin,offset,'bof') == -1
   fprintf('\n Error locating the file pointer in the file.\n');
end

[mat items]=fread(fin,[cols rows],type);
if items ~= rows*cols
   fprintf('\n Reading of file %s was incorrect.\n',image_file);
end

fclose(fin);

mat=mat';
%mat=flipud(mat);

imsize=size(mat);
%fprintf('\n image of size %d x %d \n read successfully\n',imsize(1),imsize(2));

