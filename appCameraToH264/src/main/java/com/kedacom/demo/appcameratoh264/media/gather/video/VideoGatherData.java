package com.kedacom.demo.appcameratoh264.media.gather.video;

import com.kedacom.demo.appcameratoh264.media.base.YuvData;
import com.kedacom.demo.appcameratoh264.media.gather.api.IGatherData;

/**
 * Created by yuhanxun
 * 2018/9/29
 * description:
 */
public class VideoGatherData implements IGatherData, Cloneable{
    private YuvData yuvData;
    private long timestamp;

    public VideoGatherData(YuvData yuvData, long timestamp) {
        this.yuvData = yuvData;
        this.timestamp = timestamp;
    }

    public YuvData getYuvData() {
        return yuvData;
    }

    public void setYuvData(YuvData yuvData) {
        this.yuvData = yuvData;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(long timestamp) {
        this.timestamp = timestamp;
    }

    public VideoGatherData clone() throws CloneNotSupportedException {
        VideoGatherData ret = null;
        ret = (VideoGatherData) super.clone();
        ret.setYuvData(yuvData.clone());
        return ret;
    }
}
