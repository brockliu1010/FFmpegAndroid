
#include <jni.h>
#include <string>
#include <JniHelper.h>
#include "../include/libyuv.h"

extern "C" {

}

void nv21ToI420(jbyte *src_nv21_data, jint width, jint height, jbyte *src_i420_data);

void nv21ToNv12(jbyte *src_nv21_data, jint width, jint height, jbyte *src_nv12_data);

void scaleI420(jbyte *src_i420_data, jint width, jint height, jbyte *dst_i420_data, jint dst_width,
               jint dst_height, jint mode);

void rotateI420(jbyte *src_i420_data, jint width, jint height, jbyte *dst_i420_data, jint degree);

void
rotateNV21ToNV12(jbyte *src_nv21_data, jint width, jint height, jbyte *dst_nv12_data, jint degree);

JNIEXPORT jint JNICALL compressYUV
        (JNIEnv *env, jclass clazz, jbyteArray src, jint width, jint height,
         jbyteArray dst, jint dst_width, jint dst_height, jint mode, jint degree,
         jboolean isMirror) {
    jbyte *Src_data = env->GetByteArrayElements(src, NULL);
    jbyte *Dst_data = env->GetByteArrayElements(dst, NULL);

//    jbyteArray temp_i420_array = env->NewByteArray(env->GetArrayLength(src));
//    jbyte *temp_i420_data = env->GetByteArrayElements(temp_i420_array, NULL);

//    jbyte *temp_i420_data = static_cast<jbyte *>(malloc(env->GetArrayLength(src)));
//    jbyte *temp_i420_data_scale = static_cast<jbyte *>(malloc(env->GetArrayLength(src)));


    //nv21转化为i420(标准YUV420P数据) 这个temp_i420_data大小是和Src_data是一样的
//    nv21ToI420(Src_data, width, height, temp_i420_data);
    //进行缩放的操作，这个缩放，会把数据压缩
//    scaleI420(temp_i420_data, width, height, temp_i420_data_scale, dst_width, dst_height, mode);
//    //如果是前置摄像头，进行镜像操作
//    if (isMirror) {
//        //进行旋转的操作
//        rotateI420(temp_i420_data_scale, dst_width, dst_height, temp_i420_data_rotate, degree);
//        //因为旋转的角度都是90和270，那后面的数据width和height是相反的
//        mirrorI420(temp_i420_data_rotate, dst_height, dst_width, Dst_data);
//    } else {
//        //进行旋转的操作
//        rotateI420(temp_i420_data_scale, dst_width, dst_height, Dst_data, degree);
//    }

    jbyte *temp_i420_data = (jbyte *) malloc(sizeof(jbyte) * dst_width * dst_height * 3 / 2);


    if (degree != 0) {
        nv21ToI420(Src_data, width, height, temp_i420_data);
        rotateI420(temp_i420_data, dst_width, dst_height, Dst_data, degree);
//        rotateI420(Src_data, dst_width, dst_height, Dst_data, degree);
    } else {
        nv21ToI420(Src_data, width, height, Dst_data);
    }

    free(temp_i420_data);
    env->ReleaseByteArrayElements(dst, Dst_data, 0);
    env->ReleaseByteArrayElements(src, Src_data, 0);
    return 0;
}

JNIEXPORT jint JNICALL cropYUV
        (JNIEnv *env, jclass clazz, jbyteArray src, jint width, jint height,
         jbyteArray dst, jint dst_width, jint dst_height, jint left, jint top) {

    return 0;
}

//NV21格式数据排列方式是YYYYYYYY（w*h）VUVUVUVU(w*h/2)，
//对于NV12的格式，排列方式是YYYYYYYY（w*h）UVUVUVUV（w*h/2）
//NV21转化为YUV420P数据
void nv21ToI420(jbyte *src_nv21_data, jint width, jint height, jbyte *src_i420_data) {
    //Y通道数据大小
    jint src_y_size = width * height;
    //U通道数据大小
    jint src_u_size = (width >> 1) * (height >> 1);

    //NV21中Y通道数据
    jbyte *src_nv21_y_data = src_nv21_data;
    //由于是连续存储的Y通道数据后即为VU数据，它们的存储方式是交叉存储的
    jbyte *src_nv21_vu_data = src_nv21_data + src_y_size;

    //YUV420P中Y通道数据
    jbyte *src_i420_y_data = src_i420_data;
    //YUV420P中U通道数据
    jbyte *src_i420_u_data = src_i420_data + src_y_size;
    //YUV420P中V通道数据
    jbyte *src_i420_v_data = src_i420_data + src_y_size + src_u_size;

    //直接调用libyuv中接口，把NV21数据转化为YUV420P标准数据，此时，它们的存储大小是不变的
    libyuv::NV21ToI420((const uint8 *) src_nv21_y_data, width,
                       (const uint8 *) src_nv21_vu_data, width,
                       (uint8 *) src_i420_y_data, width,
                       (uint8 *) src_i420_u_data, width >> 1,
                       (uint8 *) src_i420_v_data, width >> 1,
                       width, height);

}


//yyyyyyyyuvuv => yyyyyyyyvuvu
JNIEXPORT jint JNICALL compressNV12
        (JNIEnv *env, jclass clazz, jbyteArray src, jint width, jint height,
         jbyteArray dst, jint dst_width, jint dst_height, jint mode, jint degree,
         jboolean isMirror) {
    jbyte *Src_data = env->GetByteArrayElements(src, NULL);
    jbyte *Dst_data = env->GetByteArrayElements(dst, NULL);

    rotateNV21ToNV12(Src_data, width, height, Dst_data, degree);

    env->ReleaseByteArrayElements(dst, Dst_data, 0);
    env->ReleaseByteArrayElements(src, Src_data, 0);
    return 0;
}

void nv21ToNv12(jbyte *src_nv21_data, jint width, jint height, jbyte *src_nv12_data) {
    //Y通道数据大小
    jint src_y_size = width * height;
    //U通道数据大小
    jint src_u_size = (width >> 1) * (height >> 1);


    memcpy(src_nv12_data, src_nv21_data, src_y_size);
    for (int i = 0; i < src_u_size; i++) {
        *(src_nv12_data + src_y_size + i * 2) = *(src_nv21_data + src_y_size + i * 2 + 1);
        *(src_nv12_data + src_y_size + i * 2 + 1) = *(src_nv21_data + src_y_size + i * 2);
    }
}

//进行缩放操作，此时是把1080 * 1920的YUV420P的数据 ==> 480 * 640的YUV420P的数据
void
scaleI420(jbyte *src_i420_data, jint width, jint height, jbyte *dst_i420_data, jint dst_width,
          jint dst_height, jint mode) {
    //Y数据大小width*height，U数据大小为1/4的width*height，V大小和U一样，一共是3/2的width*height大小
    jint src_i420_y_size = width * height;
    jint src_i420_u_size = (width >> 1) * (height >> 1);
    //由于是标准的YUV420P的数据，我们可以把三个通道全部分离出来
    jbyte *src_i420_y_data = src_i420_data;
    jbyte *src_i420_u_data = src_i420_data + src_i420_y_size;
    jbyte *src_i420_v_data = src_i420_data + src_i420_y_size + src_i420_u_size;

    //由于是标准的YUV420P的数据，我们可以把三个通道全部分离出来
    jint dst_i420_y_size = dst_width * dst_height;
    jint dst_i420_u_size = (dst_width >> 1) * (dst_height >> 1);
    jbyte *dst_i420_y_data = dst_i420_data;
    jbyte *dst_i420_u_data = dst_i420_data + dst_i420_y_size;
    jbyte *dst_i420_v_data = dst_i420_data + dst_i420_y_size + dst_i420_u_size;

    //调用libyuv库，进行缩放操作
    libyuv::I420Scale((const uint8 *) src_i420_y_data, width,
                      (const uint8 *) src_i420_u_data, width >> 1,
                      (const uint8 *) src_i420_v_data, width >> 1,
                      width, height,
                      (uint8 *) dst_i420_y_data, dst_width,
                      (uint8 *) dst_i420_u_data, dst_width >> 1,
                      (uint8 *) dst_i420_v_data, dst_width >> 1,
                      dst_width, dst_height,
                      (libyuv::FilterMode) mode);
}

void
rotateI420(jbyte *src_i420_data, jint width, jint height, jbyte *dst_i420_data, jint degree) {
    jint src_i420_y_size = width * height;
    jint src_i420_u_size = (width >> 1) * (height >> 1);

    jbyte *src_i420_y_data = src_i420_data;
    jbyte *src_i420_u_data = src_i420_data + src_i420_y_size;
    jbyte *src_i420_v_data = src_i420_data + src_i420_y_size + src_i420_u_size;

    jbyte *dst_i420_y_data = dst_i420_data;
    jbyte *dst_i420_u_data = dst_i420_data + src_i420_y_size;
    jbyte *dst_i420_v_data = dst_i420_data + src_i420_y_size + src_i420_u_size;

    //要注意这里的width和height在旋转之后是相反的
    if (degree == libyuv::kRotate90 || degree == libyuv::kRotate270) {
        libyuv::I420Rotate((const uint8 *) src_i420_y_data, width,
                           (const uint8 *) src_i420_u_data, width >> 1,
                           (const uint8 *) src_i420_v_data, width >> 1,
                           (uint8 *) dst_i420_y_data, height,
                           (uint8 *) dst_i420_u_data, height >> 1,
                           (uint8 *) dst_i420_v_data, height >> 1,
                           width, height,
                           (libyuv::RotationMode) degree);
    }
}

void
rotateNV21ToNV12(jbyte *src_nv21_data, jint width, jint height, jbyte *dst_nv12_data,
                 jint degree) {

    int uvHeight = height >> 1;
    int imgSize = width * height;
    if (degree == 90 || degree == 270) {
        //270 往左倒
        //从右上角开始遍历
        if (degree == 270) {
            //旋转y
            int count = 0;
            for (int i = width - 1; i >= 0; i--) {
                for (int j = 0; j < height; j++) {
                    dst_nv12_data[count++] = src_nv21_data[j * width + i];
                }
            }
            //u,v
            for (int j = width - 1; j > 0; j -= 2) {
                for (int i = 0; i < uvHeight; i++) {
                    //vu旋转到uv
                    //u, 把nv21靠后的u,给到nv12靠前的u
                    dst_nv12_data[count++] = src_nv21_data[imgSize + width * i + j];
                    //v, 把nv21靠前的v,给到nv12靠后的u
                    dst_nv12_data[count++] = src_nv21_data[imgSize + width * i + j - 1];
                }
            }
        } else if (degree == 90) {
            //90 往右倒
            //旋转y
//            int count = 0;
//            for (int i = 0; i < width; i++) {
//                for (int j = height - 1; j >= 0; j--) {
//                    dst_nv12_data[count++] = src_nv21_data[j * width + i];
//                }
//            }
//            //u,v
//            for (int j = 0; j < width - 1; j += 2) {
//                for (int i = uvHeight - 1; i >= 0; i--) {
//                    //vu旋转到uv
//                    //u, 把nv21靠后的u,给到nv12靠前的u
//                    dst_nv12_data[count++] = src_nv21_data[imgSize + width * i + j + 1];
//                    //v, 把nv21靠前的v,给到nv12靠后的u
//                    dst_nv12_data[count++] = src_nv21_data[imgSize + width * i + j];
//                }
//            }
            //以上代码效率太低,旋转一次80毫秒左右

            jbyte *temp_i420_data = (jbyte *) malloc(sizeof(jbyte) * imgSize * 3 / 2);
            jbyte *temp_i420_rotated_data = (jbyte *) malloc(sizeof(jbyte) * imgSize * 3 / 2);

            nv21ToI420(src_nv21_data, width, height, temp_i420_data);
            rotateI420(temp_i420_data, width, height, temp_i420_rotated_data, degree);
            free(temp_i420_data);

            //Y通道数据大小
            jint src_y_size = imgSize;
            //U通道数据大小
            jint src_u_size = (width >> 1) * (height >> 1);

            //NV12中Y通道数据
            jbyte *dst_nv21_y_data = dst_nv12_data;
            //由于是连续存储的Y通道数据后即为VU数据，它们的存储方式是交叉存储的
            jbyte *dst_nv21_vu_data = dst_nv12_data + src_y_size;

            //YUV420P中Y通道数据
            jbyte *src_i420_y_data = temp_i420_rotated_data;
            //YUV420P中U通道数据
            jbyte *src_i420_u_data = temp_i420_rotated_data + src_y_size;
            //YUV420P中V通道数据
            jbyte *src_i420_v_data = temp_i420_rotated_data + src_y_size + src_u_size;

            //直接调用libyuv中接口，把NV21数据转化为YUV420P标准数据，此时，它们的存储大小是不变的
            libyuv::I420ToNV12((uint8 *) src_i420_y_data, height,
                               (uint8 *) src_i420_u_data, height >> 1,
                               (uint8 *) src_i420_v_data, height >> 1,
                               (uint8 *)  dst_nv21_y_data, height,
                               (uint8 *)  dst_nv21_vu_data, height,
                               height,width);

            free(temp_i420_rotated_data);

        }
    } else if (degree == 0) {
        nv21ToNv12(src_nv21_data, width, height, dst_nv12_data);
    }
}


JNINativeMethod nativeMethod[] = {
        {"compressYUV",  "([BII[BIIIIZ)I", (void *) compressYUV},
        {"compressNV12", "([BII[BIIIIZ)I", (void *) compressNV12},
        {"cropYUV",      "([BII[BIIII)I",  (void *) cropYUV}
};


std::string myClassName = "com/kedacom/demo/appcameratoh264/jni/YuvUtil";

JNIEXPORT jint
JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {

    return JniHelper::handleJNILoad(vm, reserved, myClassName,
                                    nativeMethod,
                                    sizeof(nativeMethod) / sizeof(nativeMethod[0]));
}
