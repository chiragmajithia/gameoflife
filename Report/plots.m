close all;
X_1D = [1,2,4,8,16,32]
T_1D = [7.42,4.84,3.12,2.31,1.91,2.67]

X_2D =[1,2,4,8,16,32];
T_2D = [7.42,05.12,3.17,2.32,1.97,2.52]

X_SEQ = [1,2,4,8.16,32]
T_SEQ = ones(1,6) * 40.06

X_SEQ_FOR = [1,2,4,8,16,32]
T_SEQ_FOR = ones(1,6) * 11.24

X_1D_FOR = [1,2,4,8,16,32]
T_1D_FOR = [12.52,7.38,4.38,2.99,2.84,2.78]

X_SEQ_OPT = [1,2,4,8.16,32]
T_SEQ_OPT = ones(1,6) * 06.11

X = X_1D
figure;
title('Time / (Performance) comparison with number of processes')
hold on;
%plot(X,T_SEQ)

plot(X,T_SEQ_OPT,':go')

plot(X,T_1D,':rx')

plot(X,T_2D, ':b*')
legend('Sequential (optimized)','1D partitioning (optimized)', '2D partitioning (optimized) ')
saveas(gcf,'all_algo_performance.png')
%%
figure;
title('Time / (Performance) comparison with code optimization ')
hold on;
x = {'2D Array + For Loop','1D Array + For Loop','1D Array + Loop Unrolling'};
t = [T_SEQ(1), T_SEQ_FOR(1), T_SEQ_OPT(1)];
b = bar(t)
b(1).FaceColor = 'blue'

set(gca, 'XTick', 1:4, 'XTickLabel', x);
saveas(gcf,'code_optimization.png')


% plot(X,T_SEQ,':go')
% plot(X,T_SEQ_FOR,':rx')
% plot(X,T_SEQ_OPT,':b*')
%legend('Sequential with 2D boolean array (For loop)','Sequential with 1D int array(For loop)', 'Sequential with 1D int array (IF -loop unrolling)')

%%
figure;
title('Time / (Performance) comparison for 1D partitioning with number of processes ')
hold on;
plot(X,T_1D,':b*')
plot(X,T_1D_FOR,':rx')
legend('1D Partitioning with For Loop','1D Partitioning (IF -loop unrolling)')
saveas(gcf,'all_algo_code_optimization.png')
