hybrid2=load('recordHybrid.txt');
%hybrid=load('recordHybrid - Hybrid30-PF100-3.txt');
hybrid=load('recordHybrid - skate2-Hybrid30-PF100-2.txt');

plot(1:length(hybrid),-hybrid,'r');
hold on
plot(1:length(hybrid2),-hybrid2,'b:');

v=mean(-hybrid)
v2=mean(-hybrid2)
% hold on
% plot(1:length(hybrid),mean(-hybrid),'r--');
% hold on
% plot(1:length(hybrid),mean(-hybrid2),'b--');
ylabel('ʱ��(ms)')
xlabel('֡��')
legend('MeanShift-�����˲��㷨','�Ľ�MeanShift-�����˲��㷨-���߳�')