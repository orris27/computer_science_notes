import torch
import torchvision
import matplotlib.pyplot as plt
import torch.utils.data as Data
import torch.nn.functional as F
from torch.autograd import Variable


num_epochs = 100
batch_size = 64
learning_rate = 0.001
download_mnist = False
root = "/home/orris/dataset/mnist"


train_dataset = torchvision.datasets.MNIST(
        root = root,
        train = True,
        transform = torchvision.transforms.ToTensor(),
        download = download_mnist,
        )
test_dataset = torchvision.datasets.MNIST(
        root = root,
        train = False,
        )

train_dataloader = Data.DataLoader(
        dataset = train_dataset,
        batch_size = batch_size,
        shuffle = True,
        num_workers = 2,
        )

#x = train_data.train_data / 255
#y = train_data.train_labels

# !! Change here
test_x = torch.unsqueeze(test_dataset.test_data, dim=1).type(torch.FloatTensor)[:2000].cuda()/255.  
test_y = test_dataset.test_labels[:2000].cuda()


#plt.imshow(train_data.train_data[0].numpy(), cmap="gray")
#plt.show()

class CNN(torch.nn.Module):
    def __init__(self):
        super(CNN, self).__init__()
        self.conv1 = torch.nn.Sequential( # (batch_size, 1, 28, 28)
                torch.nn.Conv2d(
                    in_channels = 1,
                    out_channels = 16,
                    kernel_size = 5,
                    stride = 1,
                    padding = 2),
                torch.nn.ReLU(),
                torch.nn.MaxPool2d(kernel_size = 2), # (batch_size, 16, 14, 14)
                )
        self.conv2 = torch.nn.Sequential(
                torch.nn.Conv2d(
                    in_channels = 16,
                    out_channels = 32,
                    kernel_size = 5,
                    stride = 1,
                    padding = 2),
                torch.nn.ReLU(),
                torch.nn.MaxPool2d(kernel_size = 2), # (batch_size, 32, 7, 7)
                )
        self.fc1 = torch.nn.Linear(32 * 7 * 7, 10)

    def forward(self, net): # not underlined
        net = self.conv1(net)
        net = self.conv2(net)
        net = net.view(net.size(0), -1)
        net = self.fc1(net)
        return net

model = CNN()
# !! Change here
model = model.cuda()
print(model)
#exit(0)

optimizer = torch.optim.Adam(model.parameters(), lr=learning_rate)
loss_fn = torch.nn.CrossEntropyLoss() # shape of argument? after activation fn

for epoch in range(num_epochs + 1):
    for step ,(x_batch, y_batch) in enumerate(train_dataloader):
        # !! Change here
        x_batch = Variable(x_batch).cuda()
        y_batch = Variable(y_batch).cuda()
        out = model(x_batch) # (-infty, infty)
        #loss = loss_fn(F.softmax(out), y_batch)
        loss = loss_fn(out, y_batch)

        optimizer.zero_grad() # clear the current grads
        loss.backward() 
        optimizer.step() # update the grads


        if step % 50 == 0:
            test_output = model(test_x)
            # !! Change here
            #pred_y = torch.max(test_output, 1)[1].cuda().data.numpy()
            pred_y = torch.max(test_output, 1)[1].cuda().data
            #accuracy = float((pred_y == test_y.data).astype(int).sum()) / float(test_y.size(0))
            accuracy = int((pred_y == test_y.data).sum()) / test_y.size(0)
            print('Epoch: ', epoch, '| train loss: %.4f' % loss.data.cpu().numpy(), '| test accuracy: %.2f' % accuracy)
