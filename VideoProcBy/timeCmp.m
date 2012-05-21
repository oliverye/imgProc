hybrid=load('recordHybrid.txt');
pf=load('recordPF.txt');
ms=load('recordMS.txt');
plot(1:length(hybrid),-hybrid,'r');
hold on
plot(1:length(pf),-pf,'b:');
hold on
plot(1:length(ms),-ms,'k--');
legend('MeanShift-粒子滤波算法','粒子滤波算法','MeanShift算法')