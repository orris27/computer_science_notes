'''
    use: comment out 2 out of 3 in the main codes and run
'''
import torch
import numpy as np
import torch.nn.functional as F
from torch.autograd import Variable
import matplotlib.pyplot as plt


batch_size = 200


x = torch.unsqueeze(torch.linspace(-5, 5, batch_size), dim=1)
y = x.pow(2) + torch.rand(x.size())

x, y = Variable(x), Variable(y)

def save():
    model = torch.nn.Sequential(
            torch.nn.Linear(1, 10),
            torch.nn.ReLU(),
            torch.nn.Linear(10, 1),
            )
    optimizer = torch.optim.SGD(model.parameters(), lr=0.01)
    loss_fn = torch.nn.MSELoss()

    plt.ion()
    plt.show()

    for epoch in range(1000 + 1):
        y_pred = model(x)
        loss = loss_fn(y_pred, y)
        
        if epoch % 20 == 0:
            plt.cla()
            plt.scatter(x.data.numpy(), y.data.numpy())
            plt.plot(x.data.numpy(), y_pred.data.numpy(), 'r-', lw=5)
            plt.text(0.5, 0, 'Loss=%.4f' % loss.data[0], fontdict={"size": 20, "color": "red"})
            plt.pause(0.1)
        
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

    torch.save(model, "model.pkl") # save the whole model
    torch.save(model.state_dict(), "model_params.pkl") # save the params of the model
    plt.ioff()

def load():
    model = torch.load("model.pkl") # no need to define the same model again
    y_pred = model(x)

    plt.scatter(x.data.numpy(), y.data.numpy())
    plt.plot(x.data.numpy(), y_pred.data.numpy(), 'r-', lw=5)
    plt.show()

def load2():
    model = torch.nn.Sequential(
            torch.nn.Linear(1, 10),
            torch.nn.ReLU(),
            torch.nn.Linear(10, 1),
            )
    
    model.load_state_dict(torch.load("model_params.pkl"))
    y_pred = model(x)

    plt.scatter(x.data.numpy(), y.data.numpy())
    plt.plot(x.data.numpy(), y_pred.data.numpy(), 'r-', lw=5)
    plt.show()

#save()

load()

#load2()
