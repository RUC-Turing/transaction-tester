# RUC Transaction Tester

一个测试框架，用于事务并发控制算法的正确性测试与性能评估。

# 构建

使用 `cmake` 构建：

```bash
export CXX=clang++ # 如果你的机器没有 Clang 编译器，请忽略这条命令
cd transaction-tester
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release # 若要调试，请将 Release 改为 Debug
make
ls -lah bin # 查看构建出的可执行文件
```

构建后，`build` 目录下的 `bin` 目录中会有两个可执行文件：

* `batch-test`：Batch 测试
* `interactive-test`：Interactive 测试
* `conflict-test`：Conflict 测试

每次对代码进行更改后，你需要再次执行 `make`。  
如果你新增/删除了源文件，或编辑了 `cmake` 配置（通常你不需要这么做），则需要重新执行 `cmake`。

# 接口

要使用本框架测试你的并发控制算法实现，你需要修改 `src/Algorithm` 目录下的 `Transaction.cc` 和 `Transaction.h` 文件。  
这两个文件中预先包含了一个没有并发控制（当有冲突时它将产生错误）的事务实现，这部分代码可以帮助你理解这些接口。

在框架中，我们定义了以下数据类型：

* `transaction_id_t`：定义为 `size_t`，表示事务的 ID；
* `timestamp_t`：定义为 `uint64_t`，表示时间戳；
* `RecordKey`：定义为 `uint64_t`，表示数据库中一条记录的 `key`（即主键）；
* `RecordData`：是一个结构体，表示数据库中一条记录的数据。

## 开始所有事务前

在所有事务开始执行前，框架会调用全局函数 `preloadData` 以载入初始数据：

```cpp
void preloadData(const std::unordered_map<RecordKey, RecordData> &initialRecords);
```

你需要将传入的数据存储到本地，对于多版本，你需要为每条记录创建其初始版本；对于单版本，你可以简单地复制 `initialRecords` 对象。  
注意，在本框架中，数据的插入操作只会在所有事务执行前通过 `preloadData` 接口进行一次，且不会有删除操作，所以你可能不需要考虑顶层的存储结构（将记录的 `key` 映射到记录数据的版本链）的考虑并发写，也就不需要加锁。

## 执行事务时

每个事务对应一个 `Transaction` 对象（每个 `Transaction` 对象的所有成员函数均只会在**一个**特定的线程中被调用）。每个事务可以通过其唯一的**正整数** `id` 来标识。`id` 在事务被创建，而非开始执行时确定 —— 所以你不应依赖于 `id` 在时间维度上的有序性。

你可以在 `Transaction` 对象中定义一些你的并发控制算法所需要维护的，或对于调试有帮助的成员变量。例子中仅有一个 `timestamp`，表示事务开始执行时的时间戳 —— 如果你不需要这个变量，你可以将它删除。

你需要实现以下五个成员函数：

### `Transaction::start`

```cpp
void start();
```

该函数在事务开始时被调用一次。你需要在该函数中初始化你所用到的成员变量，如：获取当前的时间戳，作为事务开始执行时的时间戳。

**注**：你可以使用 `getTimestamp` 函数获取时间戳，它返回的值是你调用该函数的次数（从 1 开始）。

```cpp
void Transaction::start() {
    timestamp = getTimestamp();
}
```

### `Transaction::read`

```cpp
bool read(const RecordKey &key, RecordData &result);
```

该函数在事务进行读操作时被调用，你需要获得 `key` 对应的数据，并将其写入到 `result` 参数中。如果操作成功，返回 `true`，否则返回 `false`。若返回 `false`，该事务将被回滚。

### `Transaction::write`

```cpp
bool write(const RecordKey &key, const RecordData &newData);
```

该函数在事务进行写操作时被调用，你需要将 `key` 对应的数据，修改为 `newData`。如果操作成功，返回 `true`，否则返回 `false`。若返回 `false`，该事务将被回滚。

### `Transaction::commit`

```cpp
bool commit();
```

该函数在事务的所有操作成功完成后被调用，即将事务提交。如果操作成功，返回 `true`，否则返回 `false`。若返回 `false`，该事务将被回滚。

### `Transaction::rollback`

```cpp
void rollback();
```

该函数会在事务的任一操作（读、写或提交）失败后被调用，即将事务回滚。

## 所有事务完成后

在所有事务完成后，框架会调用 `getSerializationOrder` 函数，来获取该函数中你对**已提交**的事务的**定序**。即，以你的并发控制算法，这些事务的执行顺序，等价于串行化条件下的哪种执行顺序。

```cpp
std::vector<transaction_id_t> getSerializationOrder();
```

你需要返回一个数组，数组的每一个成员，按照你对事务的定序，是一个已提交事务的 ID。

## 提示

* 由于数据的插入只会发生在事务执行前，所以你可能不需要对由 `key` 查找数据项的过程加锁。
* 由于多个事务会**并行**访问同一个数据项，为了线程安全，你可能需要为数据项的访问加锁。
* 除上述介绍过的成员函数外，你可以定义其他成员函数，来帮助代码复用。如，将事务提交/回滚后进行的清理操作的共同部分实现在单独的自定义函数中。
* 维护事务定序时，你可能需要对储存事务定序的数组加锁。
* 例子中的代码只是为了演示接口，为了更高的效率，你可以考虑改用更高效的算法或数据结构，如用无锁化代替加锁，用二叉树代替链表。

如果你对 C++ 标准库不熟悉，以下资料可能对你有帮助：

* 线程安全：[互斥量 `std::mutex`](https://zh.cppreference.com/w/cpp/thread/mutex) / [封锁守卫 `std::lock_guard`](https://zh.cppreference.com/w/cpp/thread/lock_guard) / [原子操作 `std::atomic`](https://zh.cppreference.com/w/cpp/atomic/atomic)
* 容器（线性表）：[数组 `std::vector`](https://zh.cppreference.com/w/cpp/container/vector) / [链表 `std::list`](https://zh.cppreference.com/w/cpp/container/list)
* 容器（集合与映射）：[有序集 `std::set`](https://zh.cppreference.com/w/cpp/container/set) / [有序映射 `std::map`](https://zh.cppreference.com/w/cpp/container/map) / [无序集 `std::unordered_set`](https://zh.cppreference.com/w/cpp/container/unordered_set) / [无序映射 `std::unordered_map`](https://zh.cppreference.com/w/cpp/container/unordered_map)

# 测试

框架附带了三种测试 —— Batch、Interactive 和 Conflict。每种测试都会在运行后输出结果，如下结果表示算法正确，并给出了已提交事务数、总事务数和平均 TPS（每秒事务处理量）：

```
Success: 6/6 commited in 0.000122241 seconds, 49083.4 TPS.
```


以下结果表示算法出错，即算法给出的串行序与实际执行的不符：

```
Error: The 1-th read result in transaction 5 is wrong: expected 'aaa' but got 'a'
```

## Batch

Batch 是一种简单的测试，它会对于给定的数据，并发执行预先给定的事务操作，并对结果进行正确性检验和性能评估。

要运行 Batch 测试，你需要准备两个文件：**预加载数据**文件和**事务操作**文件。

预加载数据文件的格式如下：

* 第一行一个正整数，表示记录条数。
* 之后的每一行，是一个正整数和一个字符串（不含空格），以一个空格隔开，分别为 `key` 和 `value`。

事务操作文件的格式如下：

* 第一行一个正整数，表示事务数量。
* 之后连续若干个部分，每个部分描述一个事务。
    * 每个部分的第一行是一个正整数，表示该事务的操作数量。
    * 之后的每一行是一个操作，操作分为读和写两种：
        * `READ key` 表示读取 `key` 对应的数据；
        * `WRITE key value` 表示将 `key` 对应的数据修改为 `value`。

Batch 测试由 `batch-test` 程序提供，用法为（其中 `-n` 参数表示并发执行的线程数量）：

```
Usage: ./bin/batch-test -d <path> -t <path> [-n <number>]
       ./bin/batch-test -?

Batch tester for transaction concurrency control algorithm implementation.

Options:
  -?, --help                            Show this help message and exit.
  -d, --preload-data=<path>             The file of dataset to be preloaded to the database.
  -t, --batch-transactions=<path>       The file of batch transactions to execute in the database.
  -n, --threads=<number>                The number of threads executing in parallel. (Default: 16)
```

### 例子

在 `build` 目录下，创建 `data.txt`，写入如下内容：

```
3
1 a
2 b
3 c
```

创建 `transactions.txt`，写入如下内容：

```
6
1
WRITE 1 aa
1
WRITE 1 aaa
1
WRITE 1 aaaa
1
READ 1
1
READ 1
1
READ 1
```

执行 Batch 测试：

```bash
# 在 build 目录下，执行 make 后
./bin/batch-test -d data.txt -t transactions.txt -n 16 # 线程数可省略
```

## Interactive

Interactive 是一种较强的测试，通过它，你可以使用可自定义的程序来交互式地调用事务接口，以模拟真实数据库应用场景下执行事务的操作。例如，写操作的内容可能与读操作的结果有关；所有事务不需要一次性发出，之后执行的事务可能与之前事务的结果有关。

本框架自带了来自 WKDB 的 YCSB 交互式测试，直接运行即可执行测试。你可以通过参数来自定义 YCSB 测试集的数据规模，或指定线程数量：

```
Usage: ./bin/interactive-test [-s <number>] [-f <number>] [-l <number>] [-t <number>] [-r <number>] [-n <number>]
       ./bin/interactive-test -?

Interactive tester for transaction concurrency control algorithm implementation.

Options:
  -?, --help                                Show this help message and exit.
  -s, --table-size=<number>                 The record count of generated being tested dataset. (Default: 10000)
  -f, --field-count=<number>                The number of fields in each record (Default: 10)
  -l, --field-length=<number>               The initial length of each field's value (Default: 10)
  -t, --transactions=<number>               The number of transactions being tested. (Default: 10000)
  -r, --requests-per-transaction=<number>   The number of request in each transaction being tested. (Default: 10)
  -n, --threads=<number>                    The number of threads executing in parallel. (Default: 16)
```

例如：

```bash
# 在 build 目录下，执行 make 后
./bin/interactive-test -n 16 # 线程数可省略
```

要自定义 Interactive 测试，你需要修改 `src/Tester/Interactive/main.cc` 程序：

* 首先，调用 `TransactionRunner::preloadData`，将初始状态下的数据导入到框架中；
* 然后，调用 `TransactionRunner::runTransaction`，执行事务；
    * 该函数接受一个参数，这个参数是一个回调函数，回调函数会在线程池的工作线程中执行；
    * 回调函数接受一个参数 `InteractiveTransaction &transaction`，即交互式事务的对象，它有以下三个方法：
        * `transaction.read(key, result)` 读取 `key` 对应的数据到 `result` 变量中，返回是否成功；
        * `transaction.write(key, newData)` 将 `key` 对应的数据修改为 `newData`，返回是否成功；
        * `transaction.commit()` 提交事务。
    * 对于 `read` 和 `write` 操作，你需要保证访问的数据项存在（处于效率考量，框架不会进行检查，故访问不存在的数据项是未定义行为）；
    * 事务的任何操作返回 `false`，均意味着事务已被回滚，之后不能再调用该事务的任何操作；
    * 回调函数返回前，事务必须被显式提交或因出错而被自动回滚；
    * `TransactionRunner::runTransaction` 返回一个 `std::future<bool>`，你可以通过调用其 `wait` 方法来等待该事务执行结束，结束后可以调用 `get` 方法来获得事务是否执行成功（详见[参考资料](https://zh.cppreference.com/w/cpp/thread/future)）；
* 等待所有事务运行结束后，调用 `TransactionRunner::validateAndPrintStatistics` 来对事务结果进行正确性检验和性能评估。

## Conflict

Conflict 是一种特殊的测试，它可以模拟执行指定的操作序列，以构造出冲突的事务操作，用于正确性测试。

要运行 Conflict 测试，你需要一个冲突事务描述文件，它的格式可以参考 [`conflict.txt`](conflict.txt)。

Conflict 测试由 `conflict-test` 程序提供，用法为：

```
Usage: ./bin/conflict-test -t <path>
       ./bin/conflict-test -?

Conflict tester for transaction concurrency control algorithm implementation.

Options:
  -?, --help                                Show this help message and exit.
  -t, --conflict-transactions=<path>        The file of conflict transactions to execute in the database.
```

`-t` 参数表示事务文件，如，使用以下命令运行附带的测试文件（来自 [WKDB](https://github.com/yqekzb123/homework-wkdb/blob/40e1da19798db5b1d32c2dffe55c2f0ef7830cff/input.txt)）：

```bash
./bin/conflict-test -t ../conflict.txt
```

**注意**：在 Conflict 测试中，所有操作均在一个线程中执行。它只能测出事务异常的问题，并不能测出多线程的条件竞争。
