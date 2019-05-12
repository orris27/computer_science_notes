URL: https://www.aaai.org/ocs/index.php/AAAI/AAAI18/paper/download/16360/16061

## Objectiveness
+ The signal given from discriminator lacks intermedia information. => Problem of long text generation



## Points
+ Manager & Worker
+ Discriminator leaks its own high-level extracted feature to the generator

## Main idea
![framework of LeakGAN](./images/leakgan_framework.png)

The Worker module takes the last(current) word ![](https://latex.codecogs.com/svg.latex?x_t) as its input at each step ![](https://latex.codecogs.com/svg.latex?t) and outputs a matrix ![](https://latex.codecogs.com/svg.latex?O_t), which is further combined with goal embedding vector ![](https://latex.codecogs.com/svg.latex?w_t) by matrix product to determine the distribution of the next word through a softmax.

The Manager module takes the extracted feature ![](https://latex.codecogs.com/svg.latex?f_t) as input, which is further combined with the current hidden state ![](https://latex.codecogs.com/svg.latex?h_{t-1}^M) of the Manager model to produce the feature sub-goal vector ![](https://latex.codecogs.com/svg.latex?g_t). To corporate the feature sub-goal vector ![](https://latex.codecogs.com/svg.latex?g_t), a linear transformation ![](https://latex.codecogs.com/svg.latex?\phi) with weight matrix ![](https://latex.codecogs.com/svg.latex?W_{\phi}) is performed on a summation over recent ![](https://latex.codecogs.com/svg.latex?c) goals to produce the action sub-goal vector ![](https://latex.codecogs.com/svg.latex?w_t).

The adversarial text generation uses ![](https://latex.codecogs.com/svg.latex?D_{\phi}) as a learned reward function. The discriminator ![](https://latex.codecogs.com/svg.latex?D_{\phi}) takes the current state ![](https://latex.codecogs.com/svg.latex?s_t) as its input to extract the feature ![](https://latex.codecogs.com/svg.latex?f_t) which is leaked into the generation procudure of the generator ![](https://latex.codecogs.com/svg.latex?G_{\theta}). A final sigmoid classification layer with weight vector ![](https://latex.codecogs.com/svg.latex?{\phi}_l) is performed on the extracted feature ![](https://latex.codecogs.com/svg.latex?f_t) to compute the reward for the current state ![](https://latex.codecogs.com/svg.latex?s_t).






At each time step, the last word is input to the Worker network, which  predicts the probabilities of next word. 


## Training Techniques

