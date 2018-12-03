import torch
import torchvision
import numpy as np
import matplotlib.pyplot as plt


time_steps = 30
input_size = 1
hidden_size = 64
num_classes = 1
num_epochs = 10
batch_size = 1

steps = np.linspace(0, np.pi * 2, 100, dtype=np.float32)
x_np = np.sin(steps)
y_np = np.cos(steps)

#plt.plot(steps, y_np, 'r-', label='target (cos)')
#plt.plot(steps, x_np, 'b-', label='input (sin)')
#plt.legend(loc='best')
#plt.show()


class RNN(torch.nn.Module):
    def __init__(self, input_size, hidden_size, num_classes):
        super(RNN, self).__init__()
        self.rnn1 = torch.nn.RNN(input_size=input_size, hidden_size=hidden_size, num_layers=1, batch_first=True)
        self.fc1 = torch.nn.Linear(hidden_size, num_classes)

    def forward(self, net, h_state): # net: (batch_size, 1, time_steps, 1)
        # 传递time_steps个timestep的时候,每个timestep输出的结果就是这个sin_y对应的cos_y
        net_out, h_state = self.rnn1(net, h_state)
        outs = []
        for time_step in range(net.size(1)):
            outs.append(self.fc1(net_out[:, time_step, :]))
        
        #net = self.fc1(outs)
        return torch.stack(outs, dim=1), h_state


model = RNN(input_size, hidden_size, num_classes)

optimizer = torch.optim.Adam(model.parameters(), lr=0.001)
loss_fn = torch.nn.MSELoss() # shape of argument? after activation fn


plt.figure(1, figsize=(12, 5))
plt.ion()
h_state = None
for epoch in range(100):
    start, end = epoch * np.pi, (epoch + 1) * np.pi
    
    steps = np.linspace(start, end, time_steps)
    
    x = np.sin(steps)
    y = np.cos(steps)

    x = torch.from_numpy(x[np.newaxis, :, np.newaxis]).type(torch.FloatTensor)
    y = torch.from_numpy(y[np.newaxis, :, np.newaxis]).type(torch.FloatTensor)

    y_pred, h_state = model(x, h_state)
    h_state = h_state.data
    
    loss = loss_fn(y_pred, y)
    
    optimizer.zero_grad()
    loss.backward()
    optimizer.step()

    print("epoch {0}: loss={1}".format(epoch, loss.data.numpy()))
    plt.plot(steps, np.squeeze(y_pred.data.numpy()))
    plt.plot(steps, y.numpy().flatten()) 
    plt.draw()
    plt.pause(0.5)

plt.ioff()
plt.show()
