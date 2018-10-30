最好当前目录下有`MNIST_DATA/`
## 1. 神经网络
```
orris@orris-Laptop:~/fun/tensorflow$ python my-nn.py 
epoch 0: 958/10000
epoch 50: 6012/10000
epoch 100: 7263/10000
epoch 150: 7514/10000
epoch 200: 7541/10000
epoch 250: 7596/10000
epoch 300: 7611/10000
epoch 350: 7755/10000
epoch 400: 9425/10000
epoch 450: 9553/10000
epoch 500: 9621/10000
epoch 550: 9661/10000
epoch 600: 9673/10000
epoch 650: 9645/10000

```
## 2. 卷积神经网络
```
orris@orris-Laptop:~/fun/tensorflow$ python my-cnn.py 
# 和上面结果的结构类似,我这里直接复制粘贴了
epoch 0: 958/10000
epoch 50: 6012/10000
epoch 100: 7263/10000
epoch 150: 7514/10000
epoch 200: 7541/10000
epoch 250: 7596/10000
epoch 300: 7611/10000
epoch 350: 7755/10000
epoch 400: 9425/10000
epoch 450: 9553/10000
epoch 500: 9621/10000
epoch 550: 9661/10000
epoch 600: 9673/10000
epoch 650: 9645/10000
```

## 3. InceptionV3
```
python inceptionv3.py # 直接python就行了
#---------------------------------------------------------------------------------------------
# 2018-10-28 09:57:36.468979: step 0, duration = 0.066
# 2018-10-28 09:57:36.469098: Forward across 100 steps, 0.001 +/- 0.007 sec / batch
# 2018-10-28 09:57:36.564198: Forward across 100 steps, 0.002 +/- 0.011 sec / batch
# 2018-10-28 09:57:36.638271: Forward across 100 steps, 0.002 +/- 0.014 sec / batch
# 2018-10-28 09:57:36.704537: Forward across 100 steps, 0.003 +/- 0.015 sec / batch
# 2018-10-28 09:57:36.771428: Forward across 100 steps, 0.004 +/- 0.016 sec / batch
# 2018-10-28 09:57:36.838128: Forward across 100 steps, 0.004 +/- 0.017 sec / batch
# 2018-10-28 09:57:36.905402: Forward across 100 steps, 0.005 +/- 0.018 sec / batch
# 2018-10-28 09:57:36.972944: Forward across 100 steps, 0.006 +/- 0.019 sec / batch
# 2018-10-28 09:57:37.039910: Forward across 100 steps, 0.006 +/- 0.020 sec / batch
# 2018-10-28 09:57:37.134661: Forward across 100 steps, 0.007 +/- 0.022 sec / batch
# 2018-10-28 09:57:37.201070: step 10, duration = 0.066
# 2018-10-28 09:57:37.201156: Forward across 100 steps, 0.008 +/- 0.023 sec / batch
# 2018-10-28 09:57:37.268278: Forward across 100 steps, 0.009 +/- 0.024 sec / batch
# 2018-10-28 09:57:37.335317: Forward across 100 steps, 0.009 +/- 0.024 sec / batch
# 2018-10-28 09:57:37.403018: Forward across 100 steps, 0.010 +/- 0.025 sec / batch
# 2018-10-28 09:57:37.469903: Forward across 100 steps, 0.011 +/- 0.026 sec / batch
# 2018-10-28 09:57:37.537236: Forward across 100 steps, 0.011 +/- 0.026 sec / batch
# 2018-10-28 09:57:37.603862: Forward across 100 steps, 0.012 +/- 0.027 sec / batch
# 2018-10-28 09:57:37.670202: Forward across 100 steps, 0.013 +/- 0.027 sec / batch
# 2018-10-28 09:57:37.736906: Forward across 100 steps, 0.013 +/- 0.028 sec / batch
# 2018-10-28 09:57:37.803668: Forward across 100 steps, 0.014 +/- 0.028 sec / batch
# 2018-10-28 09:57:37.870225: step 20, duration = 0.066
# 2018-10-28 09:57:37.870352: Forward across 100 steps, 0.015 +/- 0.029 sec / batch
# 2018-10-28 09:57:37.936967: Forward across 100 steps, 0.015 +/- 0.029 sec / batch
# 2018-10-28 09:57:38.004080: Forward across 100 steps, 0.016 +/- 0.030 sec / batch
# 2018-10-28 09:57:38.071237: Forward across 100 steps, 0.017 +/- 0.030 sec / batch
# 2018-10-28 09:57:38.138080: Forward across 100 steps, 0.017 +/- 0.030 sec / batch
# 2018-10-28 09:57:38.204885: Forward across 100 steps, 0.018 +/- 0.031 sec / batch
# 2018-10-28 09:57:38.272248: Forward across 100 steps, 0.019 +/- 0.031 sec / batch
# 2018-10-28 09:57:38.339158: Forward across 100 steps, 0.019 +/- 0.031 sec / batch
# 2018-10-28 09:57:38.406362: Forward across 100 steps, 0.020 +/- 0.032 sec / batch
# 2018-10-28 09:57:38.473167: Forward across 100 steps, 0.021 +/- 0.032 sec / batch
# 2018-10-28 09:57:38.540046: step 30, duration = 0.067
# 2018-10-28 09:57:38.540121: Forward across 100 steps, 0.021 +/- 0.032 sec / batch
# 2018-10-28 09:57:38.606997: Forward across 100 steps, 0.022 +/- 0.032 sec / batch
# 2018-10-28 09:57:38.673674: Forward across 100 steps, 0.023 +/- 0.033 sec / batch
# 2018-10-28 09:57:38.740278: Forward across 100 steps, 0.023 +/- 0.033 sec / batch
# 2018-10-28 09:57:38.806730: Forward across 100 steps, 0.024 +/- 0.033 sec / batch
# 2018-10-28 09:57:38.873554: Forward across 100 steps, 0.025 +/- 0.033 sec / batch
# 2018-10-28 09:57:38.940433: Forward across 100 steps, 0.025 +/- 0.033 sec / batch
# 2018-10-28 09:57:39.006751: Forward across 100 steps, 0.026 +/- 0.033 sec / batch
# 2018-10-28 09:57:39.074289: Forward across 100 steps, 0.027 +/- 0.034 sec / batch
# 2018-10-28 09:57:39.142706: Forward across 100 steps, 0.027 +/- 0.034 sec / batch
# 2018-10-28 09:57:39.209395: step 40, duration = 0.067
# 2018-10-28 09:57:39.209506: Forward across 100 steps, 0.028 +/- 0.034 sec / batch
# 2018-10-28 09:57:39.276455: Forward across 100 steps, 0.029 +/- 0.034 sec / batch
# 2018-10-28 09:57:39.343269: Forward across 100 steps, 0.029 +/- 0.034 sec / batch
# 2018-10-28 09:57:39.409823: Forward across 100 steps, 0.030 +/- 0.034 sec / batch
# 2018-10-28 09:57:39.476522: Forward across 100 steps, 0.031 +/- 0.034 sec / batch
# 2018-10-28 09:57:39.543370: Forward across 100 steps, 0.031 +/- 0.034 sec / batch
# 2018-10-28 09:57:39.610167: Forward across 100 steps, 0.032 +/- 0.034 sec / batch
# 2018-10-28 09:57:39.704446: Forward across 100 steps, 0.033 +/- 0.035 sec / batch
# 2018-10-28 09:57:39.771045: Forward across 100 steps, 0.034 +/- 0.035 sec / batch
# 2018-10-28 09:57:39.837834: Forward across 100 steps, 0.034 +/- 0.035 sec / batch
# 2018-10-28 09:57:39.904690: step 50, duration = 0.067
# 2018-10-28 09:57:39.904771: Forward across 100 steps, 0.035 +/- 0.035 sec / batch
# 2018-10-28 09:57:39.971439: Forward across 100 steps, 0.036 +/- 0.035 sec / batch
# 2018-10-28 09:57:40.038043: Forward across 100 steps, 0.036 +/- 0.035 sec / batch
# 2018-10-28 09:57:40.105359: Forward across 100 steps, 0.037 +/- 0.034 sec / batch
# 2018-10-28 09:57:40.172317: Forward across 100 steps, 0.038 +/- 0.034 sec / batch
# 2018-10-28 09:57:40.239009: Forward across 100 steps, 0.038 +/- 0.034 sec / batch
# 2018-10-28 09:57:40.305619: Forward across 100 steps, 0.039 +/- 0.034 sec / batch
# 2018-10-28 09:57:40.372718: Forward across 100 steps, 0.040 +/- 0.034 sec / batch
# 2018-10-28 09:57:40.439659: Forward across 100 steps, 0.040 +/- 0.034 sec / batch
# 2018-10-28 09:57:40.506306: Forward across 100 steps, 0.041 +/- 0.034 sec / batch
# 2018-10-28 09:57:40.572838: step 60, duration = 0.066
# 2018-10-28 09:57:40.573035: Forward across 100 steps, 0.042 +/- 0.034 sec / batch
# 2018-10-28 09:57:40.640622: Forward across 100 steps, 0.042 +/- 0.033 sec / batch
# 2018-10-28 09:57:40.733777: Forward across 100 steps, 0.043 +/- 0.034 sec / batch
# 2018-10-28 09:57:40.800269: Forward across 100 steps, 0.044 +/- 0.033 sec / batch
# 2018-10-28 09:57:40.867538: Forward across 100 steps, 0.045 +/- 0.033 sec / batch
# 2018-10-28 09:57:40.933977: Forward across 100 steps, 0.045 +/- 0.033 sec / batch
# 2018-10-28 09:57:41.000842: Forward across 100 steps, 0.046 +/- 0.033 sec / batch
# 2018-10-28 09:57:41.095712: Forward across 100 steps, 0.047 +/- 0.033 sec / batch
# 2018-10-28 09:57:41.162221: Forward across 100 steps, 0.048 +/- 0.032 sec / batch
# 2018-10-28 09:57:41.256908: Forward across 100 steps, 0.048 +/- 0.032 sec / batch
# 2018-10-28 09:57:41.323650: step 70, duration = 0.067
# 2018-10-28 09:57:41.323740: Forward across 100 steps, 0.049 +/- 0.032 sec / batch
# 2018-10-28 09:57:41.390342: Forward across 100 steps, 0.050 +/- 0.032 sec / batch
# 2018-10-28 09:57:41.457232: Forward across 100 steps, 0.050 +/- 0.031 sec / batch
# 2018-10-28 09:57:41.523838: Forward across 100 steps, 0.051 +/- 0.031 sec / batch
# 2018-10-28 09:57:41.591100: Forward across 100 steps, 0.052 +/- 0.031 sec / batch
# 2018-10-28 09:57:41.657747: Forward across 100 steps, 0.052 +/- 0.030 sec / batch
# 2018-10-28 09:57:41.724441: Forward across 100 steps, 0.053 +/- 0.030 sec / batch
# 2018-10-28 09:57:41.791705: Forward across 100 steps, 0.054 +/- 0.029 sec / batch
# 2018-10-28 09:57:41.858393: Forward across 100 steps, 0.054 +/- 0.029 sec / batch
# 2018-10-28 09:57:41.925216: Forward across 100 steps, 0.055 +/- 0.028 sec / batch
# 2018-10-28 09:57:41.992181: step 80, duration = 0.067
# 2018-10-28 09:57:41.992285: Forward across 100 steps, 0.056 +/- 0.028 sec / batch
# 2018-10-28 09:57:42.059582: Forward across 100 steps, 0.056 +/- 0.027 sec / batch
# 2018-10-28 09:57:42.126263: Forward across 100 steps, 0.057 +/- 0.027 sec / batch
# 2018-10-28 09:57:42.193367: Forward across 100 steps, 0.058 +/- 0.026 sec / batch
# 2018-10-28 09:57:42.260797: Forward across 100 steps, 0.058 +/- 0.025 sec / batch
# 2018-10-28 09:57:42.327969: Forward across 100 steps, 0.059 +/- 0.025 sec / batch
# 2018-10-28 09:57:42.394813: Forward across 100 steps, 0.060 +/- 0.024 sec / batch
# 2018-10-28 09:57:42.461919: Forward across 100 steps, 0.061 +/- 0.023 sec / batch
# 2018-10-28 09:57:42.528618: Forward across 100 steps, 0.061 +/- 0.022 sec / batch
# 2018-10-28 09:57:42.595687: Forward across 100 steps, 0.062 +/- 0.022 sec / batch
# 2018-10-28 09:57:42.663408: step 90, duration = 0.068
# 2018-10-28 09:57:42.663473: Forward across 100 steps, 0.063 +/- 0.021 sec / batch
# 2018-10-28 09:57:42.730889: Forward across 100 steps, 0.063 +/- 0.020 sec / batch
# 2018-10-28 09:57:42.800666: Forward across 100 steps, 0.064 +/- 0.019 sec / batch
# 2018-10-28 09:57:42.867798: Forward across 100 steps, 0.065 +/- 0.018 sec / batch
# 2018-10-28 09:57:42.936173: Forward across 100 steps, 0.065 +/- 0.016 sec / batch
# 2018-10-28 09:57:43.003605: Forward across 100 steps, 0.066 +/- 0.015 sec / batch
# 2018-10-28 09:57:43.070968: Forward across 100 steps, 0.067 +/- 0.013 sec / batch
# 2018-10-28 09:57:43.138703: Forward across 100 steps, 0.067 +/- 0.012 sec / batch
# 2018-10-28 09:57:43.205473: Forward across 100 steps, 0.068 +/- 0.009 sec / batch
# 2018-10-28 09:57:43.272549: Forward across 100 steps, 0.069 +/- 0.007 sec / batch
#---------------------------------------------------------------------------------------------

```




## 4. Slim实现ResNetV2,评测耗时
```
python resnet.py
# 输出和上面一样
```
