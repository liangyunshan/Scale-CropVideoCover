# Scale-CropVideoCover                                                  缩放裁剪视频封面                                               
en:                                                                     中文:   
- Base on FFMPEG                                                        基于FFMPEG
- Specific steps:                                                       实现步骤：
    - 1.extract cover image from video;                                     1.从视频中提取原始封面图片帧数据
    - 2.scale image to appropriate size;                                    2.将原始图片按比例缩放
    - 3.finally,crop it to final size.                                      3.最后根据实际需要进行裁剪
 ## Actual effect                                                       实际效果
    - 1.video name:r8.mp4 size:1920*1080                                    1.视频名称：r8.mp4 尺寸：1920*1080
        - original cover image(1920*1080):                                      原始封面图片(1920*1080):
        
![image](Source/11111.jpeg) 

        - final cover image(282*420):                                           最终封面图片(282*420): 
        
![image](Source/r8.jpeg)  

     - 2.video name:douyin.mp4 size:368*640                                 1.视频名称：douyin.mp4 尺寸：368*640
        - original cover image(368*640):                                        原始封面图片(368*640):
        
            NULL                                                                    空
            
        - final cover image(282*420):                                            最终封面图片(282*420):
        
![image](Source/douyin.jpeg) 
   
  ## Make                                                               编译
    - Modify the Makefile as needed                                         根据实际情况修改MakeFile
    - Execute the 'make' command                                            执行make命令
  ## Use                                                                使用
    - Switch to the path of the file where the program resides              切换到可执行程序所在文件夹的目录下
    - ./extractImg <timestamp> <videoPath> <coverName>                      ./extractImg <时间戳> <视频路径> <封面图片名字>
    example:                                                                例子：
        ./extractImg 1 r8.mp4 r8.jpeg                                           ./extractImg 1 r8.mp4 r8.jpeg 
        ps:extract cover at the $timestamp from video which path                ps:提取视频$videoPath在时间戳$timestamp时的图片帧数据为封面
        is $videoPath ,and save image into $coverName
    
  ## Note                                                               注意     
    - you may meet the problem:                                              你可能会遇到这个问题：
        error while loading shared libraries: libavformat.so.56:              error while loading shared libraries: libavformat.so.56:
        cannot open shared object file: No such file or directory             cannot open shared object file: No such file or directory
    - solution:                                                              解决办法：
        this problem is due to lack of FFMPEG libraries,so you                  这个问题是由于程序无法链接FFMPEG的运行库，因此你
        should export lib,as shown below:                                       需要这么做，如下所示：
           export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH                            export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH
        the "./lib" is my FFMPEG's lib path,you should take your                其中的"./lib"是我的FFMPEG的库所在路径，你需要替换成你的
      
    
