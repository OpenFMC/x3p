function scale = get_unit_scale(units)
    unitNames =  { 'm', 'dm', 'cm',  'mm',     'um',        'nm',            'pm' };
    unitScales = [ 1.0,  0.1, 0.01, 0.001, 0.000001, 0.000000001, 0.0000000000001 ]; 

    unitInd = find(strcmp(unitNames,units));
    if isempty(unitInd)
        error('Unrecognized unit type: %s',units);
    end
    scale = unitScales(unitInd);
end