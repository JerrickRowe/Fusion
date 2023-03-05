ipath = ['-I',fullfile('..', 'Fusion')];
mex('-R2018a',"COPTIMFLAGS=-O0",ipath,"mexFusion.c","..\Fusion\*.c");

if exist('logfile', 'var')
else
    logfile = readtable("LOG00249.xlsx");
end

ts = logfile.tick_ms / 1000;
dt = [0; diff(ts)];
ilen = length(ts);
fs = 250;

fusion_input.ts = ts(1:ilen);
fusion_input.dt = dt(1:ilen);
fusion_input.ax = logfile.imu1_ax(1:ilen);
fusion_input.ay = logfile.imu1_ay(1:ilen);
fusion_input.az = logfile.imu1_az(1:ilen);
fusion_input.gx = logfile.imu1_gx(1:ilen);
fusion_input.gy = logfile.imu1_gy(1:ilen);
fusion_input.gz = logfile.imu1_gz(1:ilen);

ts_interp = (ts(1):1/fs:ts(length(ts)));
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
ts = ts_interp;


windowSize = 3; 
windowFilter = (1/windowSize)*ones(1,windowSize);
fusion_input_acc_movmean = fusion_input_interp;
fusion_input_acc_movmean.ax = filter(windowFilter,1,fusion_input_acc_movmean.ax);
fusion_input_acc_movmean.ay = filter(windowFilter,1,fusion_input_acc_movmean.ay);
fusion_input_acc_movmean.az = filter(windowFilter,1,fusion_input_acc_movmean.az);
fusion_input_acc_movmean.gx = filter(windowFilter,1,fusion_input_acc_movmean.gx);
fusion_input_acc_movmean.gy = filter(windowFilter,1,fusion_input_acc_movmean.gy);
fusion_input_acc_movmean.gz = filter(windowFilter,1,fusion_input_acc_movmean.gz);

% movean_k = 250;
% fusion_input_acc_movmean.ax = movmean(fusion_input_interp.ax, movean_k);
% fusion_input_acc_movmean.ay = movmean(fusion_input_interp.ay, movean_k);
% fusion_input_acc_movmean.az = movmean(fusion_input_interp.az, movean_k);

fc = 10;
N = 6;
% F3dB = fc/(fs/2);
% Ap = 0.01;
% d = fdesign.lowpass('N,F3dB',N,F3dB);
% setspecs(d,'N,F3dB,Ap',N,F3dB,Ap);
% Hcheby1 = design(d,'cheby1','SystemObject',true);
% Hbutter = design(d,'butter','SystemObject',true);
% hfvt = fvtool(Hbutter);
% [b,a] = cheby1(N,Ap,fc/(fs/2));
[b,a] = butter(N,fc/(fs/2),'low');

fusion_input_acc_iir = fusion_input_interp;
% fusion_input_acc_iir.ax = filter(b, a, fusion_input_acc_iir.ax);
% fusion_input_acc_iir.ay = filter(b, a, fusion_input_acc_iir.ay);
% fusion_input_acc_iir.az = filter(b, a, fusion_input_acc_iir.az);
% fusion_input_acc_iir.gx = filter(b, a, fusion_input_acc_iir.gx);
% fusion_input_acc_iir.gy = filter(b, a, fusion_input_acc_iir.gy);
% fusion_input_acc_iir.gz = filter(b, a, fusion_input_acc_iir.gz);

f.b = b;
f.a = a;

fusion_input_acc_iir.ax = filter_butter(f, fusion_input_acc_iir.ax);
fusion_input_acc_iir.ay = filter_butter(f, fusion_input_acc_iir.ay);
fusion_input_acc_iir.az = filter_butter(f, fusion_input_acc_iir.az);
fusion_input_acc_iir.gx = filter_butter(f, fusion_input_acc_iir.gx);
fusion_input_acc_iir.gy = filter_butter(f, fusion_input_acc_iir.gy);
fusion_input_acc_iir.gz = filter_butter(f, fusion_input_acc_iir.gz);

fig_compare_movmean = figure(1);
tiledlayout(3,1);

ax_plot = nexttile;
hold on
title("ax");
% plot(fusion_input_interp.ts,fusion_input_interp.ax);
plot(fusion_input.ts,fusion_input.ax);
plot(fusion_input_acc_movmean.ts,fusion_input_acc_movmean.ax);
plot(fusion_input_acc_iir.ts,fusion_input_acc_iir.ax);

ay_plot = nexttile;
hold on
title("ay");
% plot(fusion_input_interp.ts,fusion_input_interp.ay);
plot(fusion_input.ts,fusion_input.ay);
plot(fusion_input_acc_movmean.ts,fusion_input_acc_movmean.ay);
plot(fusion_input_acc_iir.ts,fusion_input_acc_iir.ay);

az_plot = nexttile;
hold on
title("az");
% plot(fusion_input_interp.ts,fusion_input_interp.az);
plot(fusion_input.ts,fusion_input.az);
plot(fusion_input_acc_movmean.ts,fusion_input_acc_movmean.az);
plot(fusion_input_acc_iir.ts,fusion_input_acc_iir.az);

linkaxes([ax_plot,ay_plot,az_plot],"x");

fusion_output_raw = mexFusion(fusion_input_interp);
fusion_output_acc_movmean = mexFusion(fusion_input_acc_movmean);
fusion_output_acc_iir = mexFusion(fusion_input_acc_iir);

quat_raw.w = fusion_output_raw.qw;
quat_raw.x = fusion_output_raw.qx;
quat_raw.y = fusion_output_raw.qy;
quat_raw.z = fusion_output_raw.qz;

quat_acc_movmean.w = fusion_output_acc_movmean.qw;
quat_acc_movmean.x = fusion_output_acc_movmean.qx;
quat_acc_movmean.y = fusion_output_acc_movmean.qy;
quat_acc_movmean.z = fusion_output_acc_movmean.qz;

quat_acc_iir.w = fusion_output_acc_iir.qw;
quat_acc_iir.x = fusion_output_acc_iir.qx;
quat_acc_iir.y = fusion_output_acc_iir.qy;
quat_acc_iir.z = fusion_output_acc_iir.qz;

quat_comapre = quat_acc_iir;

qcomp_plot_1 = quaternion_plot_compare(ts_interp, quat_raw, quat_comapre);
set(qcomp_plot_1, 'Name', 'Compare filtered quaterion curve with raw');
figure(qcomp_plot_1);

earth_raw.x = fusion_output_raw.earth_acc_x;
earth_raw.y = fusion_output_raw.earth_acc_y;
earth_raw.z = fusion_output_raw.earth_acc_z;

earth_iir.x = fusion_output_acc_iir.earth_acc_x;
earth_iir.y = fusion_output_acc_iir.earth_acc_y;
earth_iir.z = fusion_output_acc_iir.earth_acc_z;

earth_acc_plot = vector3_plot_compare(ts,earth_raw, earth_iir);
set(earth_acc_plot, 'Name', 'Compare filtered earth acc curve with raw');
figure(earth_acc_plot);

spect = figure(3);
tiledlayout(3,2);

% anz_plot = nexttile;
% title("anz");
% plot(fusion_output_acc_movmean.ts,fusion_output_acc_movmean.earth_acc_z);

segment_len = fs;
window = hamming(segment_len);
overlap_len = segment_len-1;
nfft = fs;

signal_1 = sqrt(fusion_input_acc_movmean.ax.^2+fusion_input_acc_movmean.ay.^2+fusion_input_acc_movmean.az.^2);
signal_2 = sqrt(fusion_input_acc_iir.ax.^2+fusion_input_acc_iir.ay.^2+fusion_input_acc_iir.az.^2);

raw_signal = sqrt(fusion_input_interp.ax.^2+fusion_input_interp.ay.^2+fusion_input_interp.az.^2);

spect_raw_acc = nexttile(1);
hold on
title("spectrum acc raw");
[s_raw, f_raw, t_raw] = spectrogram(raw_signal,window,overlap_len,nfft,fs,'yaxis');
imagesc(t_raw+ts(1),f_raw,log(abs(s_raw)));
set(gca,'YDir','normal');
xlim([ts(1) ts(length(ts))]);
ylim([f_raw(1) f_raw(length(f_raw))]);

spect_raw_acc2 = nexttile(2);
hold on
title("spectrum acc raw");
imagesc(t_raw+ts(1),f_raw,log(abs(s_raw)));
set(gca,'YDir','normal');
xlim([ts(1) ts(length(ts))]);
ylim([f_raw(1) f_raw(length(f_raw))]);

filter_spect1 = nexttile(3);
hold on
title("Filter 1")
[s1, f1, t1] = spectrogram(signal_1,window,overlap_len,nfft,fs,'yaxis');
imagesc(t1+ts(1),f1,log(abs(s1)));
set(gca,'YDir','normal');
xlim([ts(1) ts(length(ts))]);
ylim([f1(1) f1(length(f1))]);

filter_spect2 = nexttile(4);
hold on
title("Filter 2")
[s2, f2, t2] = spectrogram(signal_2,window,overlap_len,nfft,fs,'yaxis');
imagesc(t2+ts(1),f2,log(abs(s2)));
set(gca,'YDir','normal');
xlim([ts(1) ts(length(ts))]);
ylim([f2(1) f2(length(f2))]);

spect_diff1 = nexttile(5);
hold on
title("spectrum diff 1");
f_diff1 = f_raw;
t_diff1 = t_raw;
s_diff1 = s1 - s_raw;
imagesc(t_diff1+ts(1),f_diff1,log(abs(s_diff1)));
set(gca,'YDir','normal');
xlim([ts(1) ts(length(ts))]);
ylim([f_diff1(1) f_diff1(length(f_diff1))]);

spect_diff2 = nexttile(6);
hold on
title("spectrum diff 2");
f_diff2 = f_raw;
t_diff2 = t_raw;
s_diff2 = s2 - s_raw;
imagesc(t_diff2+ts(1),f_diff2,log(abs(s_diff2)));
set(gca,'YDir','normal');
xlim([ts(1) ts(length(ts))]);
ylim([f_diff2(1) f_diff2(length(f_diff2))]);

linkaxes([spect_raw_acc,spect_raw_acc2,filter_spect1,filter_spect2,spect_diff1,spect_diff2],"x");

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


function fig = vector3_plot_compare(ts,a,b)
    fig = figure(4);
    tiledlayout(3,1);

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
    
    linkaxes([x_plot,y_plot,z_plot],"x");
    hold off
end

