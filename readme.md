报错：
libavformat/hls.c:834: error: undefined reference to 'atof'
libavformat/hlsproto.c:141: error: undefined reference to 'atof'
libavcodec/v4l2_buffers.c:439: error: undefined reference to 'mmap64'
clang++: error: linker command failed with exit code 1 (use -v to see invocation)

原因
出现该错误的原因，是因为上面shell脚本中，指定编译的头文件与库文件的查找目录都是21，这样编译出来的依赖库只能用在minSdkVersion 21的项目中，而我的项目最低是兼容到16，那就要重新修改了16。

参考：https://www.jianshu.com/p/7c6ea7734343
