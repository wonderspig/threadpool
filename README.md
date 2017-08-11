# threadpool
功能：threadpool，支持半同步/半异步

说明：threadpool.hpp为源文件，main.cpp为事例文件

     packaged_task<>实例是不可复制的，仅是可移动的，所以不能使用function<>来实现任务队列。因为function<>需要存储可复制构造的函数对象。包装一个自定义函数，用来处理至可移动的类型。只需要处理那些没有函数参数和无返回值的函数

需要改进：
目前所有线程共享一个工作队列，势必会造成资源抢占，导致性能下降
