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


