=== INSTALLATION AND TESTING ===

This package contains a MATLAB toolbox for reading and writing X3P files.  It
is compiled for Win64 and has been tested with MATLAB R2013a.  To use it simply
place the contents of the archive in a directory, e.g., c:\X3PToolbox.  Within
MATLAB, add the path to this toolbox to the command path with:

>> addpath c:\X3PToolbox

To test that the toolbox is installed correctly and working, run

>> TestX3P

This will load, write and reload a number of test files to ensure that
everything is functioning properly.  If this function completes without any
errors everything is working as it should be.


=== Usage ===

The toolbox provides two primary functions: readX3P and writeX3P.  Detailed usage
instructions are available by running

>> help readX3P
>> help writeX3P.

Example usage:

To load a file with units of meters run:

>> [x,y,z,pinfo,meta] = readX3P(filename,'m');

x, y and z will contain the data in the requested units (meters in this case),
pinfo will contain additional information about the data and meta will contain
additional general fields.  See the above mentioned help for detailed
description of the contents of this structure.

Other units are available, including 'dm', 'cm', 'mm', 'um' and 'nm'.  Note that
specifying these units will change the scale of all of x, y and z.

To save information about a surface, use the following:

First, create some data:
>> x = 0:0.1:1; y = 0:0.1:2; z = rand(length(x),length(y));

Next, setup the meta information:
>> meta = X3PMetaInfo();
>> meta.Creator = 'X3P Toolbox User';

Note the use of the X3PMetaInfo() to return a default meta data structure. You can
edit the fields of this data structure by editing the fields:
  .Creator
  .Date
  .Instrument_Manufacturer 
  .Instrument_Model
  .Instrument_Serial
  .Instrument_Version
  .CalibrationDate
  .ProbingSystem_Type
  .ProbingSystem_Identification
  .Comment
Note that the .Creator field was edited above.

Finally, create (save) the X3P file.
>> pinfo = writeX3P('test.x3p','SUR','m',x,y,z,meta);


Note that the x, y, z data can exist in two different formats:

The first is a long list of points:
 x(i) = x-position of ith point
 y(i) = y-position of ith point
 z(i) = z-position of ith point
where i = 1 .. n (for n surface points)

The second uses arrays for a grid of n by m points.
 x(i) = the coordinate of the ith sample point in the x dimension (i = 1..n)
 y(j) = the coordinate of the jth sample point in the y dimension (j = 1..m)
 z(i,j) = the height of the point at (x(i),y(j)) (i=1..n, j=1..m)
Where x is a one-dimensional vector with length n, y is a one dimensional
vector with length m, and z is a two-dimensional vector with size n by m.



