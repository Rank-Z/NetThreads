# NetThreads

这是一个施工中的新项目

NetThreads 预计要开发成一个Linux下的多线程网络库，它期望有以下特点

1.基于Reactor模式实现，大体上以(non-blockingIO + IO multiplexing) 实现，支持多线程下的Reactor

  具体的Reactor细节有待考虑,优先以(Reactor+work thread pool) 这样的模式开发，即单Reactor多线程
  
  有望支持多Reactor
  
 2.提供统一事件源，将IO事件、信号事件和定时器事件统一封装为Event
 
 3.提供高层次的TCP封装，包含自动TCP故障处理
 
 4.提供一定程度的线程安全性，代码达到工业强度
