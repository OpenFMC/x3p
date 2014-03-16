function setup_x3p_mex()
    ogpPath = getenv('OPENGPS_LOCATION');
    if isempty(ogpPath)
        cfile = mfilename('fullpath');
        [cdir,~,~] = fileparts(cfile);
        setenv('OPENGPS_LOCATION',cdir);
    end
end