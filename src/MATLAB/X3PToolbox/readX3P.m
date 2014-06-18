function [x,y,z,pinfo,meta] = readX3P(filename,units,varargin)
% [x,y,z] = readX3P(filename,units,...)
% [x,y,z,pinfo] = readX3P(filename,units,...)
% [x,y,z,pinfo,meta] = readX3P(filename,units,...)
%
% Load an X3P file from disk in the specified units.
%
% filename - Path to filename
% units - Desired units.  Valid units are: m, dm, cm, mm, um, nm, pm 
%
% x,y,z - Coordinate values
% pinfo - Information about the X3P file and loaded points
% meta - Meta information including dates, system info, comments, etc
%
%   Additional key-value-pairs can be specified after the last argument:
%     'VendorSpecificFilename' - The exact (case sensitive) filename
%             (without path portion) of a file that was packed into the
%             x3p file as a vendor specific extension. This argument is
%             mandatory for reading the vendorspecific extension.
%     'VendorSpecificTargetFile' - The path for the destination where the
%             unpacked file should be stored to. Default is the current
%             directory and the original filename.
%     'VendorSpecificURI' - if a specific URI has been used you have to
%             specify it here. The default extension for MATLAB written
%             x3p-files is 'http://www.mathworks.com/x3p/version_1'

    unitScale = get_unit_scale(units);
    setup_x3p_mex();
    [z,x,y,pinfo,meta] = openX3P_mex(filename,varargin{:});
    if unitScale ~= 1
        x = (1.0/unitScale)*x;
        y = (1.0/unitScale)*y;
        z = (1.0/unitScale)*z;
    end
    pinfo.Units = units;
end