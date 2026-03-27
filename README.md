# 传染病空间传播模拟（SIR 模型）

本项目模拟传染病在二维网格空间中的传播过程，采用 **SIR 类模型**（易感-感染-恢复），并基于 **曼哈顿距离** 和空间异质性感染率进行传播。代码提供了三种实现版本：**串行版**、**OpenMP 共享内存并行版** 和 **MPI 分布式并行版**，可用于对比不同并行策略下的模拟效率。

## 项目结构
.
├── file_reader.c # 输入文件读取函数（维度、感染率矩阵）
├── main_serial.c # 串行版本主程序
├── main_shared.c # OpenMP 共享内存并行版本
├── main_distributed.c # MPI 分布式内存并行版本
├── pop_128_128.txt # 示例输入数据（128×128 网格，感染率矩阵）
└── README.md # 项目说明

## 模型简介

- **网格**：二维矩形网格，每个单元格代表一个个体。
- **状态**：
  - `0`：易感者（Susceptible）
  - `1`：感染者（Infectious）
  - `(RECOVER_TIME, 2)`：正在恢复中的个体（经过一段恢复期后变为恢复者）
- **传播机制**：
  - 感染者在曼哈顿距离 ≤ R 的范围内，以一定概率感染易感者。
  - 感染概率 = `感染率 / 距离`（距离为 0 时特殊处理）。
  - 感染率由输入文件提供，可随空间位置变化。
- **恢复机制**：
  - 感染者经过 `REC_TIME` 步后恢复，变为恢复者（状态 1 → 2 → ... → 恢复）。

## 编译与运行

### 依赖

- GCC 编译器（支持 OpenMP 和 MPI）
- OpenMP 库（编译 shared 版本时）
- MPI 库（如 MPICH 或 OpenMPI，编译 distributed 版本时）

### 编译

**串行版本**  
```bash
gcc -o main_serial main_serial.c -lm
```
OpenMP 并行版本

```bash
gcc -o main_shared main_shared.c -lm -fopenmp
```
MPI 分布式版本

```bash
mpicc -o main_distributed main_distributed.c -lm
```
运行
所有版本均接受四个命令行参数：

text
./程序名 <R> <REC_TIME> <MAX_RUNS> <infile>
R：感染传播的最大曼哈顿距离（整数）

REC_TIME：恢复所需时间步长（整数）

MAX_RUNS：模拟运行的次数（每次随机选择初始感染者）

infile：输入数据文件路径

示例（使用提供的 pop_128_128.txt）
bash
# 串行
./main_serial 3 5 1000 pop_128_128.txt

# OpenMP 并行（使用 8 线程）
export OMP_NUM_THREADS=8
./main_shared 3 5 1000 pop_128_128.txt

# MPI 分布式（使用 4 个进程）
mpirun -np 4 ./main_distributed 3 5 1000 pop_128_128.txt
输入文件格式
输入文件为文本文件，首行是网格维度（两个整数，空格分隔），之后是网格内每个单元格的感染率（浮点数）。例如 pop_128_128.txt 首行为：

text
128 128
接着是 128×128 = 16384 个浮点数，表示每个位置的感染率（通常为 0~1 之间）。

输出说明
程序运行后输出以下统计信息（仅主进程输出）：

text
AVERAGE CELLS INFECTED: 134.567
TOTAL TIME TAKEN: 12.345 second
MAX INFECTED: 456
STANDARD DEVIATION: 45.678
AVERAGE CELLS INFECTED：所有模拟运行中平均感染的细胞总数（包括初始感染者）

TOTAL TIME TAKEN：总运行时间（秒）

MAX INFECTED：单次运行中最大感染细胞数

STANDARD DEVIATION：感染细胞数的标准差（评估波动性）

注：MPI 版本由于并行方式不同，输出可能与串行版本略有差异，但整体统计量应一致。

并行实现要点
OpenMP 版本 (main_shared.c)
使用 #pragma omp parallel for reduction(+:INFECTED_CELLS) 对多次运行进行并行。

随机数生成器（normal_rand）采用全局种子，需注意线程安全（此处使用全局变量，可能导致结果不可复现，实际应用中可改为线程局部种子）。

MPI 版本 (main_distributed.c)
将 MAX_RUNS 次模拟均匀分配给各进程。

主进程（rank 0）读取输入文件并广播给所有进程。

使用 MPI_Gatherv 收集各进程的局部感染数，最后统计全局结果。

随机数种子使用时间与时钟异或，保证不同进程随机性。

自定义与扩展
修改网格大小：直接更改输入文件即可。

调整传播规则：可修改 INFECT_UPDATE 和 INFECT_UPDATE_FAST 函数中的概率计算公式。

增加输出：可在 RUN 函数中添加日志记录，输出每个时间步的感染数量。

许可证
本项目仅用于学习与展示，代码可自由修改使用。

作者：[李梓赫]
