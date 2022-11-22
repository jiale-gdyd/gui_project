UsageEnvironment:
    "UsageEnvironment"和"TaskScheduler"类用于调度延迟事件、为异步读取事件分配处理程序以及输出错误/警告消息。此外，"HashTable"类定义了通用哈希表的接口，供其余代码使用。
    这些都是抽象基类；它们必须被子类化以便在实现中使用。这些子类可以利用程序将在其中运行的环境的特定属性 —— 例如，它的GUI和/或脚本环境

groupsock"
    这个库中的类封装了网络接口和套接字。特别是，"Groupsock"类封装了一个用于发送(和/或接收)多播数据报的套接字

liveMedia:
    这个库为各种流媒体类型和编解码器定义了一个类层次结构——植根于"Medium"类

BasicUsageEnvironment:
    该库定义了"UsageEnvironment"类的一个具体实现(即子类)，用于简单的控制台应用程序。使用select()循环处理读取事件和延迟操作
