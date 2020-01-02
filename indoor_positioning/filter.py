def median_filter(x, window=3):
    '''x: python list'''
    x_ext = x + x[:2]
    y = []
    for i in range(len(x)):
        
        y.append(sorted(x_ext[i:i+window])[window//2])

    return y
    
def neighborhood_mean_filter(x, window=3):
    '''x: python list'''
    x_ext = x + x[:2]
    y = []
    for i in range(len(x)):
        
        y.append(sum(x_ext[i:i+window]) / window)

    return y
 


if __name__ == '__main__':
    x = [2, 3, 80, 6]
    print('Input:', x)
    print('median filtering:', median_filter(x))
    print('neighborhood mean filtering:', neighborhood_mean_filter(x))
