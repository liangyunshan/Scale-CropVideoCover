/*=============================================================================  
#     FileName: filter_video.c  
#         Desc: an example of ffmpeg fileter 
#       Author: licaibiao  
#   LastChange: 2017-03-16   
=============================================================================*/   
#define _XOPEN_SOURCE 600 /* for usleep */  
#include <unistd.h>  
  
//#include <libavcodec/avcodec.h>
//#include <libavformat/avformat.h>
//#include <libavfilter/avfiltergraph.h>
//#include <libavfilter/avcodec.h>
//#include <libavfilter/buffersink.h>
//#include <libavfilter/buffersrc.h>
//#include <libavutil/opt.h>
//#include <libavutil/time.h>
//#include <libavutil/frame.h>

#include "include/libavcodec/avcodec.h"
#include "include/libavformat/avformat.h"
#include "include/libavfilter/avfiltergraph.h"
#include "include/libavfilter/avcodec.h"
#include "include/libavfilter/buffersink.h"
#include "include/libavfilter/buffersrc.h"
#include "include/libavutil/opt.h"
#include "include/libavutil/time.h"
#include "include/libavutil/frame.h"
#include "include/libavutil/pixfmt.h"

#include "include/libswscale/swscale.h"

#define TARGET_WIDTH 282.0
#define TARGET_HEIGHT 420.0

int SavetoJPEG(AVFrame *pFrameYUV,AVStream *pVStream,char *filepath,int width, int height){
    AVFormatContext* pFormatCtx;
    AVOutputFormat* fmt;
    AVStream* video_st;
    AVCodecContext* pCodecCtx;
    AVCodec* pCodec;

    uint8_t* picture_buf;
    AVFrame* picture;
    AVPacket pkt;
    int y_size;
    int got_picture=0;
    int size;

    int ret=0;

    int in_w=width,in_h=height;    //YUV's width and height
    char* out_file = filepath;    //Output file

    //Method 1
    pFormatCtx = avformat_alloc_context();
    //Guess format
    fmt = av_guess_format("mjpeg", NULL, NULL);
    pFormatCtx->oformat = fmt;
    //Output URL
    if (avio_open(&pFormatCtx->pb,out_file, AVIO_FLAG_READ_WRITE) < 0){
        printf("Couldn't open output file.");
        return -1;
    }


    //Method 2. More simple
    //avformat_alloc_output_context2(&pFormatCtx, NULL, NULL, out_file);
    //fmt = pFormatCtx->oformat;

    video_st = avformat_new_stream(pFormatCtx, 0);
    video_st->time_base.num = pVStream->time_base.num;
    video_st->time_base.den = pVStream->time_base.den;
    if (video_st==NULL){
        return -1;
    }
    pCodecCtx = video_st->codec;
    pCodecCtx->codec_id = fmt->video_codec;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUVJ420P;
    pCodecCtx->width = TARGET_WIDTH;
    pCodecCtx->height = TARGET_HEIGHT;
    pCodecCtx->time_base.num = pVStream->codec->time_base.num;
    pCodecCtx->time_base.den = pVStream->codec->time_base.den;
    //Output some information
    av_dump_format(pFormatCtx, 0, out_file, 1);

    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
    if (!pCodec){
        printf("Codec not found.");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec,NULL) < 0){
        printf("Could not open codec.");
        return -1;
    }
    pCodecCtx->qmin = pCodecCtx->qmax = 3;
    pCodecCtx->flags|=CODEC_FLAG_QSCALE;
    picture = av_frame_alloc();
//    size = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
    size = avpicture_get_size(pCodecCtx->pix_fmt, TARGET_WIDTH, TARGET_HEIGHT);
    picture_buf = (uint8_t *)av_malloc(size);
    if (!picture_buf)
    {
        return -1;
    }
//    avpicture_fill((AVPicture *)picture, picture_buf, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
    avpicture_fill((AVPicture *)picture, picture_buf, pCodecCtx->pix_fmt, TARGET_WIDTH, TARGET_HEIGHT);
    printf("****linesize[0]:%d---linesize[1]:%d---linesize[2]:%d\n",picture->linesize[0],picture->linesize[1],picture->linesize[2]);
    //Write Header
    avformat_write_header(pFormatCtx,NULL);

//    y_size = pCodecCtx->width * pCodecCtx->height;
    y_size = TARGET_WIDTH * TARGET_HEIGHT;
    av_new_packet(&pkt,y_size*3);
//    picture->data[0] = pFrameYUV->data[0];// Y
//    picture->data[1] = pFrameYUV->data[1];// U
//    picture->data[2] = pFrameYUV->data[2];// V
    /***********************************/
//    memcpy(picture->data[0], pFrameYUV->data[0] , pCodecCtx->width * pCodecCtx->height);
//    memcpy(picture->data[1], pFrameYUV->data[1] , pCodecCtx->width * pCodecCtx->height/4);
//    memcpy(picture->data[2], pFrameYUV->data[2] , pCodecCtx->width * pCodecCtx->height/4);
    /***********************************/
    picture->linesize[0] = TARGET_WIDTH;
    picture->linesize[1] = TARGET_WIDTH/2;
    picture->linesize[2] = TARGET_WIDTH/2;

    int i , j;
    int wOffset = 0;
    int hOffset = 0;
    if(width>TARGET_WIDTH)
    {
        wOffset = width-TARGET_WIDTH;
    }
    else
    {
        hOffset = height-TARGET_HEIGHT;
    }

//    for( i = 0; i < pCodecCtx->height; i++ )
//    {
//        memcpy(picture->data[0]+wOffset/2+i*pCodecCtx->width, pFrameYUV->data[0]+wOffset/2+ i * pFrameYUV->linesize[0], pCodecCtx->width-wOffset);
//    }
//    for( i = 0; i < pCodecCtx->height / 2; i++ )
//    {
//        memcpy(picture->data[1]+wOffset/4+i*pCodecCtx->width/2, pFrameYUV->data[1] +wOffset/4+ i * pFrameYUV->linesize[1], pCodecCtx->width / 2-wOffset/2);
//    }
//    for( i = 0; i < pCodecCtx->height / 2; i++ )
//    {
//        memcpy(picture->data[2]+wOffset/4+i*pCodecCtx->width/2, pFrameYUV->data[2]+wOffset/4+ i * pFrameYUV->linesize[2], pCodecCtx->width / 2-wOffset/2);
//    }

    //width crop
//    for( i = 0; i < pCodecCtx->height-; i++ )
//    {
//        memcpy(picture->data[0]+i*282, pFrameYUV->data[0]+wOffset/2+ i * pFrameYUV->linesize[0], 282);
//    }
//    for( i = 0; i < pCodecCtx->height / 2; i++ )
//    {
//        memcpy(picture->data[1]+i*282/2, pFrameYUV->data[1] +wOffset/4+ i * pFrameYUV->linesize[1], 141);
//    }
//    for( i = 0; i < pCodecCtx->height / 2; i++ )
//    {
//        memcpy(picture->data[2]+i*282/2, pFrameYUV->data[2]+wOffset/4+ i * pFrameYUV->linesize[2], 141);
//    }

    for( i = 0; i < TARGET_HEIGHT; i++ )
    {
        j = hOffset/2;
        if(j<TARGET_HEIGHT-hOffset/2)
            memcpy(picture->data[0]+i*(int)TARGET_WIDTH, pFrameYUV->data[0]+wOffset/2+ (i+j) * pFrameYUV->linesize[0], (int)TARGET_WIDTH);
        j++;
    }
    for( i = 0; i < TARGET_HEIGHT / 2; i++ )
    {
        j = hOffset/4;
        if(j<TARGET_HEIGHT-hOffset/4)
            memcpy(picture->data[1]+i*(int)TARGET_WIDTH/2, pFrameYUV->data[1] +wOffset/4+ (i+j) * pFrameYUV->linesize[1], (int)TARGET_WIDTH/2);
        j++;
    }
    for( i = 0; i < TARGET_HEIGHT / 2; i++ )
    {
        j = hOffset/4;
        if(j<TARGET_HEIGHT-hOffset/4)
            memcpy(picture->data[2]+i*(int)TARGET_WIDTH/2, pFrameYUV->data[2]+wOffset/4+ (i+j) * pFrameYUV->linesize[2], (int)TARGET_WIDTH/2);
        j++;
    }


//    picture->width = in_w;
//    picture->height = in_h;
    picture->width = TARGET_WIDTH;
    picture->height = TARGET_HEIGHT;
    picture->format = AV_PIX_FMT_YUVJ420P;
    picture->pts = 0;
    //Encode
    ret = avcodec_encode_video2(pCodecCtx, &pkt,picture, &got_picture);
    if(ret < 0){
        printf("Encode Error.\n");
        return -1;
    }
    if (got_picture==1){
        pkt.stream_index = video_st->index;
        ret = av_write_frame(pFormatCtx, &pkt);
    }

    av_packet_unref(&pkt);
    //Write Trailer
    av_write_trailer(pFormatCtx);

    printf("Encode Successful.\n");

    if (video_st){
        avcodec_close(video_st->codec);
        av_free(picture);
        av_free(picture_buf);
    }
    avio_close(pFormatCtx->pb);
    avformat_free_context(pFormatCtx);
    return 0;
}

//void SaveYUV(AVFrame *pFrame, int width, int height, int iFrame) {
//    FILE *pFile;
//    char szFilename[32];
//    int  y;
//
//    // Open file
//    sprintf(szFilename, "/Users/shiyaoxsy/frame%d.yuv", iFrame);
//    pFile=fopen(szFilename, "wb+");
//    if(pFile==NULL)
//        return;
//    // Write data
//    int y_size = width * height;
//    fwrite(pFrame->data[0], 1, y_size, pFile);
//    fwrite(pFrame->data[1], 1, y_size/4, pFile);
//    fwrite(pFrame->data[2], 1, y_size/4, pFile);
//
//    // Close file
//    fclose(pFile);
//}

int main(int argc, char * argv[]) {

    if(argc != 4){
        fprintf(stderr, "usage: %s timestamp input_file video_output_file \n",argv[0]);
        return -1;
    }
    AVFormatContext *pFormatCtx;           // 包含了一个视频流的格式内容
    int             i, videoindex,PictureSize;      //videoindex:视频类型数据所在帧索引值
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    AVFrame *pFrame,*pFrameRGB;
    AVPacket packet;
    int ret, got_picture;
    char *filepath = argv[2];
    int64_t time_,time_total;

    struct SwsContext *pSwsCtx;
    uint8_t *outBuff;
    time_total = av_gettime_relative();         //av_gettime_relative拿到的是1970年1月1日到现在的时间，也就是格林威治时间
    av_register_all();
    avformat_network_init();

    pFormatCtx = avformat_alloc_context();  //AVFormatContext必须初始化为NULL或者用avformat_alloc_context()进行初始化
    time_ = av_gettime_relative();

   // 打开一个输入流(filepath),(输入流必须是已经用avformat_close_input关闭的)，
    //并将输入流中的数据读入到buf，以及判断输入文件的格式，
    //并将输入流格式保存到指针AVFormatContext里面iformat(struct AVInputFormat *)成员
    //在调用该函数之前，须确保av_register_all()和avformat_network_init()已调用
    if(avformat_open_input(&pFormatCtx,filepath,NULL,NULL)!=0){
        printf("Couldn't open input stream.\n");
        return -1;
    }

    //avformat_find_stream_info可以读取一部分视音频数据并且获得一些相关的信息
    if(avformat_find_stream_info(pFormatCtx,NULL)<0){
        printf("Couldn't find stream information.\n");
        return -1;
    }

    videoindex=-1;

    for(i=0; i<pFormatCtx->nb_streams; i++){
         //指明了目标是视频类型数据(可能包括视频，音频或者字幕类型)
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
            videoindex=i;   //视频类型数据索引值
            break;
        }
    }

    if(videoindex == -1){
        printf("Didn't find a video stream.\n");
        return -1;
    }

    pCodecCtx=pFormatCtx->streams[videoindex]->codec;           //获取视频类型流数据的AVCodecContext
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);           //查找FFmpeg的解码器
    if(pCodec==NULL){
        printf("Codec not found.\n");
        return -1;
    }
    if(avcodec_open2(pCodecCtx, pCodec,NULL)<0){            //使用解码器pCodec(AVCodec *)初始化上下文pCodecCtx(AVCodecContext *)
        printf("Could not open codec.\n");
        return -1;
    }

    //AVFrame 存放从AVPacket中解码出来的原始数据，其必须通过av_frame_alloc来创建，通过av_frame_free来释放
    pFrame=av_frame_alloc();
    pFrameRGB=av_frame_alloc();
    if( pFrame == NULL || pFrameRGB == NULL)
    {
        printf("avframe malloc failed!\n");
        return -1;
    }

    float factor = 1.0;
    if(pCodecCtx->width >= pCodecCtx->height)
    {
        factor = pCodecCtx->height/TARGET_HEIGHT;
    }
    else
    {
        factor = pCodecCtx->width/TARGET_WIDTH;
    }
//    PictureSize = avpicture_get_size(AV_PIX_FMT_YUVJ420P, pCodecCtx->width, pCodecCtx->height);             //根据像素格式和视频分辨率获得picture存储大小
    PictureSize = avpicture_get_size(AV_PIX_FMT_YUVJ420P, pCodecCtx->width/factor, pCodecCtx->height/factor);
//    PictureSize = avpicture_get_size(AV_PIX_FMT_YUVJ420P, 282, 420);
    outBuff = (uint8_t*)av_malloc(PictureSize*sizeof(uint8_t));

    if(outBuff == NULL){
        printf("av malloc failed!\n");
        return -1;
    }
    //将pFrameRGB的数据按AV_PIX_FMT_YUVJ420P格式自动"关联"到outBuff
//    avpicture_fill((AVPicture *)pFrameRGB, outBuff, AV_PIX_FMT_YUVJ420P, pCodecCtx->width, pCodecCtx->height);
    avpicture_fill((AVPicture *)pFrameRGB, outBuff, AV_PIX_FMT_YUVJ420P, pCodecCtx->width/factor, pCodecCtx->height/factor);
//    avpicture_fill((AVPicture *)pFrameRGB, outBuff, AV_PIX_FMT_YUVJ420P, 282, 420);
    /*******************
     * function:sws_getContext() 初始化SwsContext
     **srcW：源图像的宽
     **srcH：源图像的高
     **srcFormat：源图像的像素格式
     **dstW：目标图像的宽
     **dstH：目标图像的高
     **dstFormat：目标图像的像素格式
     **flags：设定图像拉伸使用的算法
    * ********************/
//    pSwsCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUVJ420P, SWS_BICUBIC, NULL, NULL, NULL);
    pSwsCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width/factor, pCodecCtx->height/factor, AV_PIX_FMT_YUVJ420P, SWS_BICUBIC, NULL, NULL, NULL);
//    pSwsCtx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, 282, 420, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    time_ = av_gettime_relative() - time_;
    printf("open file and find info, cost time: %0.3fs\n",time_/1000000.0);

    time_ = av_gettime_relative();
    int64_t timestamp = atoi(argv[1]);
    timestamp = av_rescale(timestamp, pFormatCtx->streams[videoindex]->time_base.den, (int64_t)pFormatCtx->streams[videoindex]->time_base.num);

    /************************
     * function:av_seek_frame() 将音/视频seek到指定的位置
     * AVFormatContext *s       // 封装格式上下文
    *int streamIndex                // 流的索引。默认值为-1，因为媒体文件中可能既包含视频又包含音频，可以通过streamIndex来指定究竟是以视频还是音频来移。
    *int64_t timestamp.           // 时间戳。你要移动到哪个时间位置。
    *int flag                               // 标识位。表示我们移动的策略(究竟是向前移，还是向后移)
    **************************/
    av_seek_frame(pFormatCtx, videoindex, timestamp, AVSEEK_FLAG_BACKWARD);
    avcodec_flush_buffers(pFormatCtx->streams[videoindex]->codec);                  //在再次解码之前，必须使用avcodec_flush_buffers重新编码
    time_ = av_gettime_relative()-time_;
    printf("seek frame, costs time: %0.3fs\n",time_ / 1000000.0);
    AVPacket avpkt;
    av_init_packet(&avpkt);
    avpkt.data = NULL;
    avpkt.size = 0;

    time_ = av_gettime_relative();
    while (av_read_frame(pFormatCtx, &packet) >=0 ) {           //读取码流pFormatCtx中的音频若干帧或者视频一帧到packet
        if(packet.stream_index == videoindex ){
            if(packet.flags){
                //解码一帧视频数据,输入一个压缩编码的结构体AVPacket，输出一个解码后的结构体AVFrame
                if((ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet))<0){
                    printf("Decode Error!\n");
                    return -1;
                }
                //解码一帧视频数据，直到got_picture为真值
                //没有数据帧则got_picture为0否则为非0值
                while(got_picture==0){
                    sleep(0.01);
                    if((ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &avpkt))<0){
                        printf("Decode Error!\n");
                        return -1;
                    }
                }
                if(got_picture){
                    time_ = av_gettime_relative()-time_;
                    printf("read and decode frame, costs time: %0.3fs\n",time_/1000000.0);

                    time_ = av_gettime_relative();
                    sws_scale(pSwsCtx, (uint8_t const * const *)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data, pFrameRGB->linesize);
//                    if (SavetoJPEG(pFrameRGB,pFormatCtx->streams[videoindex],argv[3],pCodecCtx->width,pCodecCtx->height)<0) {
                    if (SavetoJPEG(pFrameRGB,pFormatCtx->streams[videoindex],argv[3],pCodecCtx->width/factor,pCodecCtx->height/factor)<0) {
//                    if (SavetoJPEG(pFrameRGB,pFormatCtx->streams[videoindex],argv[3],282,420)<0) {
                        printf("Write Image Error!\n");
                        return -1;
                    }
                    time_ = av_gettime_relative()-time_;
                    printf("write frame, costs time: %0.3fs\n",time_/1000000.0);
                    av_packet_unref(&packet);
                    break;
                }

            }

        }
        av_packet_unref(&packet);
    }
    sws_freeContext(pSwsCtx);
    av_packet_unref(&avpkt);
    av_free(outBuff);
    av_free(pFrameRGB);
    av_free(pFrame);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    time_total = av_gettime_relative()-time_total;
    printf("all done, costs time: %0.3fs\n",time_total/1000000.0);
    return 0;
}
