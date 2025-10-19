# Rpc框架

## 依赖库
### protobuf

### tinyxml2


## 日志开发
主要采用异步日志
日志模块  
```
1. 日志级别
2. 打印到文件，支持日期命名， 以及日志的滚动
3. c格式化
4. 线程安全
```

LogLevel:
```
Debug
Info
Error
```
LogEvenet:
```
文件名、行号
MsgNo
进程id
Thread id
日期、时间精确到毫秒
自定义的消息
```
## EventLoop模块

Reactor模块，主线程是一个循环不断地从epoll_wait中获取事件，一旦获得事件，就让别的线程处理事件


### 任务队列

```c++
std::unique_lock<std::mutex> locker(task_mtx_);
std::queue<std::function<void()>> tmp_tasks;
m_pending_tasks.swap(tmp_tasks);    // 当前的任务队列与临时队列交换
locker.unlock();
```

`m_pending_tasks`中已经没有任务了，交换后，`m_pending_tasks` 变为空队列（不再有任务），而所有原任务都保存在 `tmp_tasks` 中。后续可以安全地操作 `tmp_tasks`（此时已解锁，不影响其他线程对 `m_pending_tasks` 的修改）。

### 初始化

