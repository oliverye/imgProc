hybrid=load('recordHybrid.txt');
pf=load('recordPF.txt');
ms=load('recordMS.txt');
plot(1:length(hybrid),-hybrid,'r');
hold on
plot(1:length(pf),-pf,'b:');
hold on
plot(1:length(ms),-ms,'k--');
legend('MeanShift-�����˲��㷨','�����˲��㷨','MeanShift�㷨')