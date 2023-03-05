ipath = ['-I',fullfile('..', 'Fusion')];
mex('-R2018a',"COPTIMFLAGS=-O0",ipath,"mexFusion.c","..\Fusion\*.c");

if exist('logfile', 'var')
else
    logfile = readtable("LOG00249.xlsx");
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

ts_interp = (ts(1):0.004:ts(length(ts)));
ts_interp = transpose(ts_interp);
interp_method = 'linear';
fusion_input_interp.ts = ts_interp;
fusion_input_interp.dt = [0;diff(ts_interp)];
fusion_input_interp.ax = interpn(ts,fusion_input.ax,fusion_input_interp.ts,interp_method);
fusion_input_interp.ay = interpn(ts,fusion_input.ay,fusion_input_interp.ts,interp_method);
fusion_input_interp.az = interpn(ts,fusion_input.az,fusion_input_interp.ts,interp_method);
fusion_input_interp.gx = interpn(ts,fusion_input.gx,fusion_input_interp.ts,interp_method);
fusion_input_interp.gy = interpn(ts,fusion_input.gy,fusion_input_interp.ts,interp_method);
fusion_input_interp.gz = interpn(ts,fusion_input.gz,fusion_input_interp.ts,interp_method);

movean_k = 250;
fusion_input_acc_movmean = fusion_input_interp;
fusion_input_acc_movmean.ax = movmean(fusion_input_acc_movmean.ax, movean_k);
fusion_input_acc_movmean.ay = movmean(fusion_input_acc_movmean.ay, movean_k);
fusion_input_acc_movmean.az = movmean(fusion_input_acc_movmean.az, movean_k);

fig_compare_movmean = figure(1);
tiledlayout(3,1);

ax_plot = nexttile;
hold on
title("ax");
% plot(fusion_input_interp.ts,fusion_input_interp.ax);
plot(fusion_input.ts,fusion_input.ax);
plot(fusion_input_acc_movmean.ts,fusion_input_acc_movmean.ax);

ay_plot = nexttile;
hold on
title("ay");
% plot(fusion_input_interp.ts,fusion_input_interp.ay);
plot(fusion_input.ts,fusion_input.ay);
plot(fusion_input_acc_movmean.ts,fusion_input_acc_movmean.ay);

az_plot = nexttile;
hold on
title("az");
% plot(fusion_input_interp.ts,fusion_input_interp.az);
plot(fusion_input.ts,fusion_input.az);
plot(fusion_input_acc_movmean.ts,fusion_input_acc_movmean.az);

linkaxes([ax_plot,ay_plot,az_plot],"x");

fusion_output_raw = mexFusion(fusion_input_interp);
fusion_output_acc_movmean = mexFusion(fusion_input_acc_movmean);

quat_raw.w = fusion_output_raw.qw;
quat_raw.x = fusion_output_raw.qx;
quat_raw.y = fusion_output_raw.qy;
quat_raw.z = fusion_output_raw.qz;

quat_acc_movmean.w = fusion_output_acc_movmean.qw;
quat_acc_movmean.x = fusion_output_acc_movmean.qx;
quat_acc_movmean.y = fusion_output_acc_movmean.qy;
quat_acc_movmean.z = fusion_output_acc_movmean.qz;

qcomp_plot_1 = quaternion_plot_compare(ts_interp, quat_raw, quat_acc_movmean);
set(qcomp_plot_1, 'Name', 'Compare quaterion curve between raw and movmean');
figure(qcomp_plot_1);

spect = figure(3);
tiledlayout(3,1);
ts = ts_interp;

% anz_plot = nexttile;
% title("anz");
% plot(fusion_output_acc_movmean.ts,fusion_output_acc_movmean.earth_acc_z);

fs = 250;
segment_len = fs;
window = hamming(segment_len);
overlap_len = segment_len-1;
nfft = fs;

signal_1 = sqrt(fusion_input_acc_movmean.ax.^2+fusion_input_acc_movmean.ay.^2+fusion_input_acc_movmean.az.^2);
signal_2 = sqrt(fusion_input_interp.ax.^2+fusion_input_interp.ay.^2+fusion_input_interp.az.^2);

spect_lin_acc = nexttile;
hold on
title("spectrum filtered")
[s1, f1, t1] = spectrogram(signal_1,window,overlap_len,nfft,fs,'yaxis');
imagesc(t1+ts(1),f1,log(abs(s1)));
set(gca,'YDir','normal');
xlim([ts(1) ts(length(ts))]);
ylim([f1(1) f1(length(f1))]);

spect_raw_acc = nexttile;
hold on
title("spectrum acc raw");
[s2, f2, t2] = spectrogram(signal_2,window,overlap_len,nfft,fs,'yaxis');
imagesc(t2+ts(1),f2,log(abs(s2)));
set(gca,'YDir','normal');
xlim([ts(1) ts(length(ts))]);
ylim([f2(1) f2(length(f2))]);

spect_diff = nexttile;
hold on
title("spectrum diff");
f_diff = f2;
t_diff = t2;
s_diff = s1 - s2;
imagesc(t_diff+ts(1),f_diff,log(abs(s_diff)));
set(gca,'YDir','normal');
xlim([ts(1) ts(length(ts))]);
ylim([f_diff(1) f_diff(length(f_diff))]);

linkaxes([spect_lin_acc,spect_raw_acc,spect_diff],"x");


function fig = quaternion_plot_compare(ts,a,b)
    fig = figure(2);
    tiledlayout(4,1);
    
    w_plot = nexttile;
    hold on
    title("w");
    plot(ts,a.w);
    plot(ts,b.w);
    
    x_plot = nexttile;
    hold on
    title("x");
    plot(ts,a.x);
    plot(ts,b.x);
    
    y_plot = nexttile;
    hold on
    title("y");
    plot(ts,a.y);
    plot(ts,b.y);

    z_plot = nexttile;
    hold on
    title("z");
    plot(ts,a.z);
    plot(ts,b.z);
    
    linkaxes([w_plot,x_plot,y_plot,z_plot],"x");
    hold off
end


