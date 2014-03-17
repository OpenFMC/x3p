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

Finally, create the X3P file.
>> pinfo = writeX3P('test.x3p','SUR','m',x,y,z,meta);