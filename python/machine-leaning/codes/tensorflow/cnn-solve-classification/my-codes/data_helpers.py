#import tensorflow as tf
import re
import numpy as np


def clean_str(string):
    """
    Tokenization/string cleaning for all datasets except for SST.
    """
    string = string.strip()
    string = re.sub(r"[^A-Za-z0-9(),!?\'\`]", " ", string)
    string = re.sub(r"\'s", " \'s", string)
    string = re.sub(r"\'ve", " \'ve", string)
    string = re.sub(r"n\'t", " n\'t", string)
    string = re.sub(r"\'re", " \'re", string)
    string = re.sub(r"\'d", " \'d", string)
    string = re.sub(r"\'ll", " \'ll", string)
    string = re.sub(r",", " , ", string)
    string = re.sub(r"!", " ! ", string)
    string = re.sub(r"\(", " \( ", string)
    string = re.sub(r"\)", " \) ", string)
    string = re.sub(r"\?", " \? ", string)
    string = re.sub(r"\s{2,}", " ", string)
    return string.strip().lower()


def load_data_and_labels(positive_data_file,negative_data_file):
    # 打开文件字符串
    positive_data = open(positive_data_file,"rb").read().decode('utf-8')
    negative_data = open(negative_data_file,"rb").read().decode('utf-8')

    # 分隔成列表
    positive_data = positive_data.split('\n')[:-1]
    negative_data = negative_data.split('\n')[:-1]
    
    # 清洗列表中的每个字符串
    positive_data = [clean_str(row) for row in positive_data]
    negative_data = [clean_str(row) for row in negative_data]

    # 构造特征值
    features = positive_data + negative_data
    
    # 构造标签
    positive_labels = [[0,1] for _ in positive_data]
    negative_labels = [[1,0] for _ in negative_data]

    labels = np.concatenate([positive_labels, negative_labels], 0)

    return [features,labels]


def next_batch(data, batch_size, num_epochs,shuffle=True):
    '''
        获得数据,一批一批的
    '''
    
    # 转换data为numpy array
    data = np.array(data)
    # 获得1个epoch有多少个batch_size
    num_batches = int((len(data) - 1)/batch_size) + 1
    # for(i:0=>epoch的次数)
    for i in range(num_epochs):
        # 如果洗牌的话
        if shuffle:
            # 洗牌
            shuffle_indices = np.random.permutation(np.arange(len(data))) # 会将[0, len(data))的整数洗牌
            data = data[shuffle_indices]
        # for(i:0=>1个epoch中batch的个数)
        for i in range(num_batches):
            # 计算起始index
            start_index = i * batch_size
            # 计算结束index
            end_index = min((i+1)*batch_size, len(data))
            # 如果不够batch_size的话
            if end_index == len(data):
                start_index = -1 * batch_size
                
            # yield出列表
            yield data[start_index:end_index]
    
