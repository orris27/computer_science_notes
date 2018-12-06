## 1. Installation

ref: [OpenCV: Installation in Linux](https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html)
```
git clone https://github.com/opencv/opencv.git

cd opencv/

mkdir build
cd build/


sudo cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
sudo make -j7 # runs 7 jobs in parallel
sudo make install
```


## 2. Syntax


Rect

左上角 大小

clone 深层拷贝


Scalar 
