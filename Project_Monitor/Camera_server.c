#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>

#define PORT 8080
#define WIDTH 640
#define HEIGHT 480
#define BACKLOG 10  // 最大连接排队数量

struct buffer {
    void   *start;
    size_t length;
};

struct thread_data {
    int client_socket;
    int fd;
    struct buffer* buffers;
    struct v4l2_format fmt;
};

void* handle_client(void* arg) {
    struct thread_data* data = (struct thread_data*)arg;
    int client_socket = data->client_socket;
    int fd = data->fd;
    struct buffer *buffers = data->buffers;
    struct v4l2_format fmt = data->fmt;

    printf("Client connected\n");

    // 采集和发送数据
    while (1) {
        struct v4l2_buffer buf;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(fd, VIDIOC_DQBUF, &buf) < 0) {
            perror("读取缓冲区失败");
            break;
        }

        // 检查客户端连接状态
        int error = 0;
        socklen_t len = sizeof(error);
        int retval = getsockopt(client_socket, SOL_SOCKET, SO_ERROR, &error, &len);
        if (retval != 0 || error != 0) {
            perror("客户端连接断开");
            break;
        }

        //超时或其他客户端断开处理
        if(errno == EPIPE)
        {
            perror("客户端端口");
            break;
        }
        // 发送数据
        ssize_t bytes_sent = send(client_socket, buffers[buf.index].start, buf.bytesused, 0);
        if (bytes_sent < 0) {
            perror("发送数据失败");
            break;
        } else {
            printf("发送了 %zd 字节的数据\n", bytes_sent);
        }

        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("重新放入缓冲区失败");
            break;
        }

            //清理资源部分
           /* close(client_socket);
            printf("Client disconnected\n");
            free(data);
            return NULL;*/

    }

    // 关闭客户端连接
    close(client_socket);
    printf("Client disconnected\n");
    free(data);
    return NULL;
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    int fd;
    struct v4l2_format fmt;
    struct buffer buffers[4];

    // 创建套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created\n");

    // 设置套接字选项
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    printf("Socket options set\n");

    // 绑定端口
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("192.168.250.100"); // 设置为服务器的 IP 地址
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Bind to port %d\n", PORT);

    // 监听连接
    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Listening for connections\n");

    // 打开摄像头
    fd = open("/dev/video0", O_RDWR);
    if (fd < 0) {
        perror("打开摄像头失败");
        return -1;
    }
    printf("Camera opened\n");

    // 设置摄像头格式
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = WIDTH;
    fmt.fmt.pix.height = HEIGHT;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("设置格式失败");
        return -1;
    }
    printf("Camera format set\n");

    // 请求缓冲区
    struct v4l2_requestbuffers req;
    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("请求缓冲区失败");
        return -1;
    }
    printf("Buffers requested\n");

    // 映射缓冲区到用户空间
    for (int i = 0; i < 4; i++) {
        struct v4l2_buffer buf;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("查询缓冲区失败");
            return -1;
        }
        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        if (buffers[i].start == MAP_FAILED) {
            perror("映射缓冲区失败");
            return -1;
        }
    }
    printf("Buffers mapped\n");

    // 启动摄像头
    for (int i = 0; i < 4; i++) {
        struct v4l2_buffer buf;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0) {
            perror("启动摄像头失败");
            return -1;
        }
    }
    if (ioctl(fd, VIDIOC_STREAMON, &fmt.type) < 0) {
        perror("启动视频流失败");
        return -1;
    }
    printf("Camera streaming started\n");

    // 接受客户端连接
    while (1) {
        int new_socket;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // 创建新线程处理客户端
        pthread_t thread_id;
        struct thread_data* data = (struct thread_data*)malloc(sizeof(struct thread_data));
        data->client_socket = new_socket;
        data->fd = fd;
        data->buffers = buffers;
        data->fmt = fmt;

        if (pthread_create(&thread_id, NULL, handle_client, (void*)data) != 0) {
            perror("pthread_create");
            return -1;
        }

        // 分离线程，确保它在完成后能够自动清理资源
        pthread_detach(thread_id);
    }

    // 关闭摄像头
    ioctl(fd, VIDIOC_STREAMOFF, &fmt.type);
    for (int i = 0; i < 4; i++) {
        munmap(buffers[i].start, buffers[i].length);
    }
    close(fd);

    // 关闭套接字
    close(server_fd);
    return 0;
}