import torch
import torchvision
import numpy as np
import matplotlib.pyplot as plt
import torch.utils.data as Data
import torch.nn.functional as F
from torch.autograd import Variable

download_mnist = False
mnist_dir = "/home/orris/dataset/mnist"
time_steps = 28
input_size = 28
hidden_size = 64
num_classes = 10
num_epochs = 10
batch_size = 64


train_dataset = torchvision.datasets.MNIST(
        root = mnist_dir,
        train = True, # True: training data; False: testing data
        transform = torchvision.transforms.ToTensor(), # ndarray => torch tensor
        download = download_mnist, # whether download or not
        )


train_dataloader = Data.DataLoader(dataset = train_dataset, batch_size = batch_size, shuffle = True, num_workers = 2)


test_dataset = torchvision.datasets.MNIST(root=mnist_dir, train=False)

test_x = Variable(torch.unsqueeze(test_dataset.test_data, dim=1).type(torch.FloatTensor)[:2000]/255.)
test_y = Variable(test_dataset.test_labels[:2000])

class LSTM(torch.nn.Module):
    def __init__(self, input_size, hidden_size, num_classes):
        super(LSTM, self).__init__()
        self.lstm1 = torch.nn.LSTM(input_size=input_size, hidden_size=hidden_size, num_layers=1, batch_first=True)
        self.fc1 = torch.nn.Linear(hidden_size, num_classes)

    def forward(self, net): # net: (batch_size, 1, 28, 28)
        net_out, (h_n, h_c) = self.lstm1(net, None) # (batch_size, time_steps, input_size)
        net = net_out[:, -1, :]
        net = self.fc1(net)
        return net


model = LSTM(input_size, hidden_size, num_classes)


optimizer = torch.optim.Adam(model.parameters(), lr=0.001)
loss_fn = torch.nn.CrossEntropyLoss() # shape of argument? after activation fn

for epoch in range(num_epochs + 1):
    for step ,(x_batch, y_batch) in enumerate(train_dataloader):
        x_batch = Variable(x_batch)
        y_batch = Variable(y_batch)

        x_batch = x_batch.view(x_batch.size(0), time_steps, input_size)

        out = model(x_batch) # (-infty, infty)
        loss = loss_fn(F.softmax(out), y_batch)

        optimizer.zero_grad() # clear the current grads
        loss.backward() 
        optimizer.step() # update the grads

        if step % 50 == 0:
            test_x = test_x.view(test_x.size(0), time_steps, input_size)
            test_output = model(test_x)

            #pred_y = torch.max(test_output, 1)[1].data.numpy()
            y_pred = torch.squeeze(torch.max(F.softmax(test_output), dim=1)[1]) # the output of torch.max is a tuple with (max_values, max_indices)
            accuracy = float((y_pred.data.numpy() == test_y.data.numpy()).astype(int).sum()) / float(test_y.size(0))
            print('Epoch: ', epoch, '| train loss: %.4f' % loss.data.numpy(), '| test accuracy: %.2f' % accuracy)
