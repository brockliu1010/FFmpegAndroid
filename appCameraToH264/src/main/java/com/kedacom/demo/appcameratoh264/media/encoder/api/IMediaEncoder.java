package com.kedacom.demo.appcameratoh264.media.encoder.api;

/**
 * Created by yuhanxun
 * 2018/9/20
 * description:
 */
public interface IMediaEncoder {
    void init();
    void config(IEncoderParam param);
    void start();
    void stop();
    void release();
    void putPacket(IPacketData packetData);
    void changeBitrate(int byterate);

    State getState();

    long getLengthEncoded();

    //fps
    int getRTBitrate();
    int getRTInputRate();
    int getRTOutputRate();

    int getQueueSize();

    void setCallback(Callback callback);
    void setOnStateChangedListener(OnStateChangedListener onStateChangedListener);

    interface Callback{
        void onDataEncoded(IFrameData encodedData);
    }
    interface OnStateChangedListener {
        void onState(IMediaEncoder encoder, State state);
    }

    enum State {
        //空闲
        IDLE,
        //启动中
        Starting,
        //编码中
        Encoding,
        //结束中,回到空闲
        EncodeStoping,
    }
}
