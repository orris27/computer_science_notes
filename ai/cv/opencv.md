## Installation

### Approach 1: Git
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

### Approach 2: zip
The original version Jiang gives to me is `-DWITH_QT=ON`, but he turns off it when he installs opencv on my machine.
```bash
tar xf OpenCV-3.4.5.tar.gz
cd opencv-3.4.5
mkdir build
cd build
cmake -DCMAKE_C_COMPILER=/usr/bin/gcc-5 \
      -DCMAKE_CXX_COMPILER=/usr/bin/g++-5 \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_INSTALL_PREFIX=/usr/local \
      -DOPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-3.4.5/modules \
      -DWITH_CUDA=ON \
      -DWITH_CUBLAS=ON \
      -DDCUDA_NVCC_FLAGS="-D_FORCE_INLINES" \
      -DENABLE_CXX11=ON \
      -DWITH_QT=OFF \
      -DBUILD_EXAMPLES=OFF \
      -DBUILD_DOCS=OFF \
      ..
make -j8
sudo make install
```
If the OpenCV program reports the following error, please run `sudo ldconfig -v` and we can find that `libopencv_core.so.3.4` is inside the output and we can run `./main` without erorrs.
```
./main: error while loading shared libraries: libopencv_core.so.3.4: cannot open shared object file: No such file or directory
```


## Syntax


Rect

左上角 大小

clone 深层拷贝


Scalar 
