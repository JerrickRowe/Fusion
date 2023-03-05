movean_k = 10;

fusion_input_movmean.ax = movmean(fusion_input.ax, movean_k);
fusion_input_movmean.ay = movmean(fusion_input.ay, movean_k);
fusion_input_movmean.az = movmean(fusion_input.az, movean_k);
% fusion_input_movmean.gx = movemean(fusion_input.gx, fusion_input_movmean);
% fusion_input_movmean.gy = movemean(fusion_input.gy, fusion_input_movmean);
% fusion_input_movmean.gz = movemean(fusion_input.gz, fusion_input_movmean);

ts = fusion_input.ts;

tiledlayout(3,1)

ax_plot = nexttile;
hold on
plot(ts,fusion_input.ax);
plot(ts,fusion_input_movmean.ax);

ay_plot = nexttile;
hold on
plot(ts,fusion_input.ay);
plot(ts,fusion_input_movmean.ay);

az_plot = nexttile;
hold on
plot(ts,fusion_input.az);
plot(ts,fusion_input_movmean.az);

linkaxes([ax_plot,ay_plot,az_plot],"x")
hold off


tiledlayout(3,1)

ax_plot = nexttile;
hold on
plot(ts,fusion_input.ax);
plot(ts,fusion_input_movmean.ax);

ay_plot = nexttile;
hold on
plot(ts,fusion_input.ay);
plot(ts,fusion_input_movmean.ay);

az_plot = nexttile;
hold on
plot(ts,fusion_input.az);
plot(ts,fusion_input_movmean.az);

linkaxes([ax_plot,ay_plot,az_plot],"x")
hold off


