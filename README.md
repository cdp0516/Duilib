基于duilib某个版本，修改了某些功能供个人使用

以下为改动记录：
2017.02.08
  为Container控件增加MoveTo方法，可以快速的调整子控件指针在内存中的顺序

2017.01.20
  增加控件的SetAnyTag方法，这样可以让控件保存一些类的对象（例如shared_ptr)
  
2016.12.13
  修改UIAnimation的一个bug，整个layout会多移动一帧。以及OnAnimationSetp函数，
  回调的次数现在等于total

2016.11.24
  实现VerticalSplitLayout

2016.11.21
  实现HorizontalSplitLayout

2016.11.20
  去掉预编译头文件，这样可以防止vs编辑器错误的提示找不到声明/文件之类的问题。
  会增加编译时间
