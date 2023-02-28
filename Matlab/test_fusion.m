if exist('logfile', 'var')
else
    logfile = readtable("LOG00241.xlsx");
end

ts = logfile.tick_ms / 1000;
dt = [0; diff(ts)];
ilen = length(ts);

fusion_input.ts = ts(1:ilen);
fusion_input.dt = dt(1:ilen);
fusion_input.ax = logfile.imu1_ax(1:ilen);
fusion_input.ay = logfile.imu1_ay(1:ilen);
fusion_input.az = logfile.imu1_az(1:ilen);
fusion_input.gx = logfile.imu1_gx(1:ilen);
fusion_input.gy = logfile.imu1_gy(1:ilen);
fusion_input.gz = logfile.imu1_gz(1:ilen);
fusion_output = mexFusion(fusion_input);
