function pinfo = writeX3P(filename,featureType,units,x,y,z,meta,varargin)
% pinfo = writeX3P(FileName,FeatureType,Units,x,y,z,meta,...)
%
% Write an X3P file.
%
%     FileName - name of file to write
%     FeatureType - one of PRF for a line profile, SUR for surface or PCL for a point cloud.
%     Units - one of m, dm, cm, mm, um, nm, pm 
%     x     - 1,2 or 3d array of x coordinates in units
%     y     - 1,2 or 3d array of y coordinates in units
%     z     - 1,2 or 3d array of z coordinates in units
%     meta  - Meta data structure of the file with the following elements:
%      .Date    - Data set creation date of the form '2007-04-30T13:58:02.6+02:00'.
%                 Set to empty string to use current time.
%      .Creator - optional name of the creator or empty array.
%      .Instrument_Manufacturer - String with name of the manufacturer
%      .Instrument_Model - String with instrument model or software name
%      .Instrument_Serial - String with serial number of instrument or 'not available'.
%      .Instrument_Version - Hardware and software version string of Instrument
%      .CalibrationDate - Date of last calibration of the form '2007-04-30T13:58:02.6+02:00'
%      .ProbingSystem_Type - one of  'Software','Contacting' or 'NonContacting'
%      .ProbingSystem_Identification - String identifying lens, probe tip, etc.
%      .Comment    - 'A user comment specific to this dataset'
%     pinfo - Info about data organisation
%   Additional key-value-pairs can be specified after the last argument:
%     'rotation' - a 3x3 real matrix R containing a rotation matrix that is applied
%                  to each 3D-data point P on readback. R1 = R*P+T
%     'translation'- a 3 element vector T containing a translation vector for the data
%                  that is applied to each 3D-data point P on readback: R1 = R*P+T
%     'VendorSpecific' - a path of a file to be packed into x3p file as a vendor
%                  specific extension. The filename portion of the file is used as
%                  the URI for this extension is 'http://www.mathworks.com/x3p/version_1'

	unitScale = get_unit_scale(units);
    if unitScale ~= 1
        z = unitScale*z;
        x = unitScale*x;
        y = unitScale*y;
    end
    setup_x3p_mex();
    pinfo = writeX3P_mex(filename,featureType,x,y,z,meta,varargin{:});
    pinfo.Units = units;
end