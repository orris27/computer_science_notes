## 1. 安装
1. 普通安装
```
# 如果是python3源码安装好后,直接安装tensorflow的话,是不会有任何报错的

python -V
#----------------------------------------------------------------------------------------------
# Python 3.6.4
#----------------------------------------------------------------------------------------------


pip -V
#----------------------------------------------------------------------------------------------
# pip 9.0.1 from /usr/local/python3/lib/python3.6/site-packages (python 3.6)
#----------------------------------------------------------------------------------------------


pip install -U tensorflow
pip install tensorflow==1.8.0
```
2. gpu: [参考资料](https://github.com/orris27/orris/blob/master/python/machine-leaning/nvidia.md)
```
nvcc -V # 检查CUDA版本
#-------------------------------------------------------------------------------------------
# nvcc: NVIDIA (R) Cuda compiler driver
# Copyright (c) 2005-2017 NVIDIA Corporation
# Built on Mon_Jun_26_16:13:28_CDT_2017
# Cuda compilation tools, release 9.0, V9.0.102
#-------------------------------------------------------------------------------------------


cat /proc/driver/nvidia/version 
#-------------------------------------------------------------------------------------------
# NVRM version: NVIDIA UNIX x86_64 Kernel Module  384.130  Wed Mar 21 03:37:26 PDT 2018
# GCC version:  gcc version 5.4.0 20160609 (Ubuntu 5.4.0-6ubuntu1~16.04.10) 
#-------------------------------------------------------------------------------------------

cat /usr/local/cuda/version.txt 
#-------------------------------------------------------------------------------------------
# CUDA Version 8.0.61
#-------------------------------------------------------------------------------------------

nvidia-smi # 检测driver是否出问题
# nvidia-settings # 若弹出设置对话框,也表示驱动安装成功

echo $LD_LIBRARY_PATH # 检查使用的cuda
#-------------------------------------------------------------------------------------------
# /usr/local/cuda-9.0/lib64
#-------------------------------------------------------------------------------------------

ls /usr/local/cuda-8.0/lib64/libcudnn.so* # 查看是否该cuda下有cudnn
#-------------------------------------------------------------------------------------------
# /usr/local/cuda-8.0/lib64/libcudnn.so  /usr/local/cuda-8.0/lib64/libcudnn.so.7.1.3
#-------------------------------------------------------------------------------------------



lsmod | grep nouveau # 如果没有输出,表示成功禁用nouveau驱动

conda create -n venv pip python=2.7 # conda版本
source activate venv
    
pip install --upgrade tensorflow-gpu
source deactivate
#conda remove -n your_env_name(虚拟环境名称) --all # 删除虚拟环境
```

## 2. 神经网络
```
# tf.placeholder
# Layer1 (conv+pooling)
    # W
    # b
    # activate
    # pool
# ...
# Layer2 (fully_connected)
    # W
    # b
    # activate
    # (dropout)
# rivals
# fight
    # check
```

## 3. tf.ConfigProto()
1. 使用在tf.Session里面
```
with tf.Session(config=tf.ConfigProto(...),...)
```
2. 参数
    1. log_device_placement=True : 是否打印设备分配日志
    2. allow_soft_placement=True ： 如果你指定的设备不存在，允许TF自动分配设备
    ```
    tf.ConfigProto(log_device_placement=True,allow_soft_placement=True)
    ```
3. 控制GPU资源使用率
    1. 刚一开始分配少量的GPU容量，然后按需慢慢的增加
        + 不会释放内存，所以会导致碎片
    ```
    config = tf.ConfigProto()
    config.gpu_options.allow_growth = True
    session = tf.Session(config=config, ...)
    ```
    2. 设置每个GPU应该拿出多少容量给进程使用，0.4代表 40%
    ```
    gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.7)
    config=tf.ConfigProto(gpu_options=gpu_options)
    session = tf.Session(config=config, ...)
    ```
4. 控制使用哪块GPU
```
~/ CUDA_VISIBLE_DEVICES=0  python your.py#使用GPU0
~/ CUDA_VISIBLE_DEVICES=0,1 python your.py#使用GPU0,1
#注意单词不要打错

#或者在 程序开头
os.environ['CUDA_VISIBLE_DEVICES'] = '0' #使用 GPU 0
os.environ['CUDA_VISIBLE_DEVICES'] = '0,1' # 使用 GPU 0，1
```

5. 查看GPU使用量
    + 这里使用了0.3.而`3.95×1024×0.3 = 1213MB`
```
totalMemory: 3.95GiB freeMemory: 3.40GiB

2018-09-06 00:55:11.122146: I tensorflow/core/common_runtime/gpu/gpu_device.cc:1053] Created TensorFlow device (/job:localhost/replica:0/task:0/device:GPU:0 with 1213 MB memory) -> physical GPU (device: 0, name: GeForce GTX 965M, pci bus id: 0000:01:00.0, compute capability: 5.2)
```
## 4. 训练
1. 计算所有梯度
    1. 哪些变量
    2. 那种计算方法
2. 处理梯度
3. 应用梯度到变量中
