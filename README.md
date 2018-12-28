# Scale-CropVideoCover
- base on FFMPEG
- Specific steps:
    - 1.extract cover image from video;
    - 2.scale image to appropriate size;
    - 3.finally,crop it to final size.
 ## Actual effect
    - 1.video name:r8.mp4 size:1920*1080  
        - original cover image(1920*1080):
        
![image](Source/11111.jpeg) 

        - final cover image(282*420):
        
![image](Source/r8.jpeg)  

     - 2.video name:douyin.mp4 size:368*640  
        - original cover image(368*640): 
        
            NULL  
            
        - final cover image(282*420):
        
![image](Source/douyin.jpeg) 
   
  ## Make
    - Modify the Makefile as needed
    - Execute the 'make' command
  ## Use
    - Switch to the path of the file where the program resides
    - ./extractImg <timestamp> <videoPath> <coverName>
    example:
        ./extractImg 1 r8.mp4 r8.jpeg
        ps:extract cover at the $timestamp from video which path is $videoPath ,and save image into $coverName
    
  ## Note
    you may meet the problem:
        error while loading shared libraries: libavformat.so.56: cannot open shared object file: No such file or directory
    solution:
        this problem is due to lack of FFMPEG libraries,so you should export lib,as shown below:
        - export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH
        the "./lib" is my FFMPEG's lib path,you should take your
      
    
