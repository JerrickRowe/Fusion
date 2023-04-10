ipath = ['-I',fullfile('..', 'Fusion')];
mex('-R2018a',"COPTIMFLAGS=-O0",ipath,"mexFusion.c","..\Fusion\*.c");
ipath = ['-I',fullfile('.', 'iir')];
mex('-R2018a',"COPTIMFLAGS=-O0",ipath,"mex_iir.c",".\iir.c");
mex -O FilterX.c

if exist('logfile', 'var')
else
    logfile = readtable("LOG00666-.xlsx");
end

ts_raw = logfile.frame_timestamp / 1000;
dt = [0; diff(ts_raw)];
ilen = length(ts_raw);
fs = 250;
fc = 17;
order = 3;
ts_interp = ts_raw(1):1/fs:ts_raw(length(ts_raw));

[b,a] = butter(order,fc/(fs/2));
[z,p,k] = tf2zpk(b,a);

ax_raw = logfile.imu1_ax;
ay_raw = logfile.imu1_ay;
az_raw = logfile.imu1_az;
gx_raw = logfile.imu1_gx;
gy_raw = logfile.imu1_gy;
gz_raw = logfile.imu1_gz;

ax_interp = interp1(ts_raw,ax_raw,ts_interp,"spline");
ay_interp = interp1(ts_raw,ay_raw,ts_interp,"spline");
az_interp = interp1(ts_raw,az_raw,ts_interp,"spline");
gx_interp = interp1(ts_raw,gx_raw,ts_interp,"spline");
gy_interp = interp1(ts_raw,gy_raw,ts_interp,"spline");
gz_interp = interp1(ts_raw,gz_raw,ts_interp,"spline");

ax_matlab_iir = matlab_iir(order,fs,fc,ax_interp);
ay_matlab_iir = matlab_iir(order,fs,fc,ay_interp);
az_matlab_iir = matlab_iir(order,fs,fc,az_interp);
gx_matlab_iir = matlab_iir(order,fs,fc,gx_interp);
gy_matlab_iir = matlab_iir(order,fs,fc,gy_interp);
gz_matlab_iir = matlab_iir(order,fs,fc,gz_interp);

ax_filtered_FilterX = FilterM(b,a, ax_interp,zeros(0, 1, fil_order)) + 0.01;
ay_filtered_FilterX = FilterM(b,a, ay_interp,zeros(0, 1, fil_order)) + 0.01;
az_filtered_FilterX = FilterM(b,a, az_interp,zeros(0, 1, fil_order)) + 0.01;
gx_filtered_FilterX = FilterM(b,a, gx_interp,zeros(0, 1, fil_order)) + 0.01;
gy_filtered_FilterX = FilterM(b,a, gy_interp,zeros(0, 1, fil_order)) + 0.01;
gz_filtered_FilterX = FilterM(b,a, gz_interp,zeros(0, 1, fil_order)) + 0.01;

ax_filtered_mex = mex_iir(ax_interp) + 0.02;
ay_filtered_mex = mex_iir(ay_interp) + 0.02;
az_filtered_mex = mex_iir(az_interp) + 0.02;
gx_filtered_mex = mex_iir(gx_interp) + 0.02;
gy_filtered_mex = mex_iir(gy_interp) + 0.02;
gz_filtered_mex = mex_iir(gz_interp) + 0.02;

% Compare raw IMU and filtered imu
plot_compare_acc_filtered = figure(1);
tiledlayout(3,2);
ax_fig = nexttile(1);
hold on
title("ax");
plot(ts_raw,ax_raw,'.g');
plot(ts_interp,ax_interp,'--y');
plot(ts_interp,ax_matlab_iir,"blue");
plot(ts_interp,ax_filtered_mex,"red");
plot(ts_interp,ax_filtered_FilterX,"black");
ay_fig = nexttile(3);
hold on
title("ay");
plot(ts_raw,ay_raw,'.g');
plot(ts_interp,ay_interp,'--y');
plot(ts_interp,ay_matlab_iir,"blue");
plot(ts_interp,ay_filtered_mex,"red");
plot(ts_interp,ay_filtered_FilterX,"black");
az_fig = nexttile(5);
hold on
title("az");
plot(ts_raw,az_raw,'.g');
plot(ts_interp,az_interp,'--y');
plot(ts_interp,az_matlab_iir,"blue");
plot(ts_interp,az_filtered_mex,"red");
plot(ts_interp,az_filtered_FilterX,"black");
gx_fig = nexttile(2);
hold on
title("gx");
plot(ts_raw,gx_raw,'.g');
plot(ts_interp,gx_interp,'--y');
plot(ts_interp,gx_matlab_iir,"blue");
plot(ts_interp,gx_filtered_mex,"red");
plot(ts_interp,gx_filtered_FilterX,"black");
gy_fig = nexttile(4);
hold on
title("gy");
plot(ts_raw,gy_raw,'.g');
plot(ts_interp,gy_interp,'--y');
plot(ts_interp,gy_matlab_iir,"blue");
plot(ts_interp,gy_filtered_mex,"red");
plot(ts_interp,gy_filtered_FilterX,"black");
gz_fig = nexttile(6);
hold on
title("gz");
plot(ts_raw,gz_raw,'.g');
plot(ts_interp,gz_interp,'--y');
plot(ts_interp,gz_matlab_iir,"blue");
plot(ts_interp,gz_filtered_mex,"red");
plot(ts_interp,gz_filtered_FilterX,"black");
linkaxes([ax_fig,ay_fig,az_fig,gx_fig,gy_fig,gz_fig],"x");
set(plot_compare_acc_filtered, 'Name', 'Compare raw IMU and filtered IMU');

% Get input of interpolated raw IMU data for mexFusion
ts_interp = transpose(ts_interp);
interp_method = 'linear';
fusion_input_interp.ts = ts_interp;
fusion_input_interp.dt = [0;diff(ts_interp)];
fusion_input_interp.ax = ax_interp;
fusion_input_interp.ay = ay_interp;
fusion_input_interp.az = az_interp;
fusion_input_interp.gx = gx_interp;
fusion_input_interp.gy = gy_interp;
fusion_input_interp.gz = gz_interp;

% Get input of window-filtered IMU data for mexFusion
windowSize = 3; 
windowFilter = (1/windowSize)*ones(1,windowSize);
fusion_input_acc_movmean = fusion_input_interp;
fusion_input_acc_movmean.ax = filter(windowFilter,1,fusion_input_acc_movmean.ax);
fusion_input_acc_movmean.ay = filter(windowFilter,1,fusion_input_acc_movmean.ay);
fusion_input_acc_movmean.az = filter(windowFilter,1,fusion_input_acc_movmean.az);
fusion_input_acc_movmean.gx = filter(windowFilter,1,fusion_input_acc_movmean.gx);
fusion_input_acc_movmean.gy = filter(windowFilter,1,fusion_input_acc_movmean.gy);
fusion_input_acc_movmean.gz = filter(windowFilter,1,fusion_input_acc_movmean.gz);

% Get input of iir-filtered IMU data for mexFusion
fusion_input_acc_iir = fusion_input_interp;
fusion_input_acc_iir.ax = ax_matlab_iir;
fusion_input_acc_iir.ay = ay_matlab_iir;
fusion_input_acc_iir.az = az_matlab_iir;
fusion_input_acc_iir.gx = gx_matlab_iir;
fusion_input_acc_iir.gy = gy_matlab_iir;
fusion_input_acc_iir.gz = gz_matlab_iir;

% Calculate AHRS by mexFusion
fusion_output_raw = mexFusion(fusion_input_interp);
fusion_output_acc_movmean = mexFusion(fusion_input_acc_movmean);
fusion_output_acc_iir = mexFusion(fusion_input_acc_iir);

% Get quaterion for comparing
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

% Compare filtered quaterion with raw quaterion
quat_comapre = quat_acc_iir;
qcomp_plot_1 = quaternion_plot_compare(ts_interp, quat_raw, quat_comapre);
set(qcomp_plot_1, 'Name', 'Compare filtered quaterion with raw quaterion');
figure(qcomp_plot_1);

% Compare filtered earth acc with raw earth acc
earth_raw.x = fusion_output_raw.earth_acc_x;
earth_raw.y = fusion_output_raw.earth_acc_y;
earth_raw.z = fusion_output_raw.earth_acc_z;
earth_iir.x = fusion_output_acc_iir.earth_acc_x;
earth_iir.y = fusion_output_acc_iir.earth_acc_y;
earth_iir.z = fusion_output_acc_iir.earth_acc_z;
earth_acc_plot = vector3_plot_compare(ts_interp,earth_raw, earth_iir);
set(earth_acc_plot, 'Name', 'Compare filtered earth acc with raw earth acc');
figure(earth_acc_plot);


spect = figure(3);
tiledlayout(3,2);

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
imagesc(t_raw+ts_interp(1),f_raw,log(abs(s_raw)));
set(gca,'YDir','normal');
xlim([ts_interp(1) ts_interp(length(ts_interp))]);
ylim([f_raw(1) f_raw(length(f_raw))]);

spect_raw_acc2 = nexttile(2);
hold on
title("spectrum acc raw");
imagesc(t_raw+ts_interp(1),f_raw,log(abs(s_raw)));
set(gca,'YDir','normal');
xlim([ts_interp(1) ts_interp(length(ts_interp))]);
ylim([f_raw(1) f_raw(length(f_raw))]);

filter_spect1 = nexttile(3);
hold on
title("Filter 1")
[s1, f1, t1] = spectrogram(signal_1,window,overlap_len,nfft,fs,'yaxis');
imagesc(t1+ts_interp(1),f1,log(abs(s1)));
set(gca,'YDir','normal');
xlim([ts_interp(1) ts_interp(length(ts_interp))]);
ylim([f1(1) f1(length(f1))]);

filter_spect2 = nexttile(4);
hold on
title("Filter 2")
[s2, f2, t2] = spectrogram(signal_2,window,overlap_len,nfft,fs,'yaxis');
imagesc(t2+ts_interp(1),f2,log(abs(s2)));
set(gca,'YDir','normal');
xlim([ts_interp(1) ts_interp(length(ts_interp))]);
ylim([f2(1) f2(length(f2))]);

spect_diff1 = nexttile(5);
hold on
title("spectrum diff 1");
f_diff1 = f_raw;
t_diff1 = t_raw;
s_diff1 = s1 - s_raw;
imagesc(t_diff1+ts_interp(1),f_diff1,log(abs(s_diff1)));
set(gca,'YDir','normal');
xlim([ts_interp(1) ts_interp(length(ts_interp))]);
ylim([f_diff1(1) f_diff1(length(f_diff1))]);

spect_diff2 = nexttile(6);
hold on
title("spectrum diff 2");
f_diff2 = f_raw;
t_diff2 = t_raw;
s_diff2 = s2 - s_raw;
imagesc(t_diff2+ts_interp(1),f_diff2,log(abs(s_diff2)));
set(gca,'YDir','normal');
xlim([ts_interp(1) ts_interp(length(ts_interp))]);
ylim([f_diff2(1) f_diff2(length(f_diff2))]);

linkaxes([spect_raw_acc,spect_raw_acc2,filter_spect1,filter_spect2,spect_diff1,spect_diff2],"x");
hold off

filter_response = figure(5);
tiledlayout(1,2);
zpk_fig = nexttile(1);
zplane(b,a);
text(real(z)-0.1,imag(z)-0.1,"Zeros")
text(real(p)-0.1,imag(p)-0.1,"Poles")
f_rsp_fig = nexttile(2);
hold on
[h,w] = freqz(b,a,'whole',fs);plot(w/pi,20*log10(abs(h)))
ax = gca;
ax.YLim = [-fs/2 20];
ax.XTick = 0:.1:1;
ax.XLim = [0 1];
xlabel('Normalized Frequency (\times\pi rad/sample)')
ylabel('Magnitude (dB)')
xline(fc/(fs/2))
hold off

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

function output = matlab_iir(order,fs,fc,v)
    [b,a] = butter(order,fc/(fs/2));
    output=filter(b,a,v);
end
