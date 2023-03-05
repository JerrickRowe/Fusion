function out = filter_butter(f,in)
    out = filter(f.b, f.a, in);
end
