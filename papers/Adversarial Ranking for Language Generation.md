URL: https://arxiv.org/pdf/1705.11001.pdf

## Objectiveness
Generate high-quality language descriptions based on GAN

## Problems to be solved
+ The absolute binary predicate assigned by the discriminator is not informative

## Points
+ Use a ranker to rank the human-written sentences higher than the machine-written sentences relatively, which further requires the generator to generate synthetic sentences that can be ranked higher than real one.

## Main idea
The generator is trained using Policy Gradient. The rewards is computed by the discriminator. The intermedia rewards are estimated based on the generated complete sentences which is sampled by Monte Carlo rollout methods which is applies to current sentences.

The discriminator evaluates the relative scores among sentences, which contain input sentences and reference sentences. A softmax-like formula with parameter ![](https://latex.codecogs.com/svg.latex?\gamma) and cosine similarity are used to compute the ranking scores for a certain sequence ![](https://latex.codecogs.com/svg.latex?s) given a comparison set ![](https://latex.codecogs.com/svg.latex?C). 

![](./images/rankgan_cosine_similarity.png)

![](./images/rankgan_softmax_like.png)

The ranking scores of an input sentence is the expectation of its scores given different reference senteces sampled across the reference space. The reference set ![](https://latex.codecogs.com/svg.latex?U) is constructed by randomly sampling a set of references from human-written sentences. The comparison set ![](https://latex.codecogs.com/svg.latex?C) is constructed according to the type of the input sentence ![](https://latex.codecogs.com/svg.latex?s), i.e., ![](https://latex.codecogs.com/svg.latex?C) is sampled from fake data and donated as ![](https://latex.codecogs.com/svg.latex?C^-) if the input sentence is real data, while ![](https://latex.codecogs.com/svg.latex?C) is sampled from real data and donated as ![](https://latex.codecogs.com/svg.latex?C^+) if the input sentence is fake data.

![](./images/rankgan_expected_ranking_scores.png)


## Tips
+ softmax-like formula in which lower ![](https://latex.codecogs.com/svg.latex?\gamma) results in all sentences to be nearly equiprobable, while higher ![](https://latex.codecogs.com/svg.latex?\gamma) increases the biases towards the sentence with the greater scores.

+ Train the discriminator(ranker) by minimizing ![](https://latex.codecogs.com/svg.latex?log%28R_%7B%5Cphi%7D%28s%7CU%2C%20C%5E&plus;%29%29) instead of maximizing ![](https://latex.codecogs.com/svg.latex?log%281-R_%7B%5Cphi%7D%28s%7CU%2CC%5E&plus;%29%29) to make the network model learn better
