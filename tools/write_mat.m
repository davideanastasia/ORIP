function [] = write_mat(image_file,mat,type,rows,cols);

% syntax: [] = write_mat(image_file,mat,type,rows,cols);
% Writes in binary form the matrix mat in the file <image_file>.
% The elements of <mat> are written with the precision of <type>


fout=fopen(image_file,'wb');

if fout == -1
   fprintf('\n file did not open succesfully\n');
end


tot_el = fwrite(fout,mat',type);

if tot_el ~= rows*cols
   fprintf('\n The data of the input were written incorrectly.\n');
end

fclose(fout);

