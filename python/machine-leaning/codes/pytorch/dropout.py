import torch
import matplotlib.pyplot as plt

batch_size = 20
learning_rate = 0.01

x = torch.unsqueeze(torch.linspace(-1, 1, batch_size), dim=1)
#print(x.size())
y = x + torch.normal(torch.zeros(x.size()), torch.ones(x.size())) * 0.3



x_test = torch.unsqueeze(torch.linspace(-1, 1, batch_size), 1)
y_test = x_test + 0.3*torch.normal(torch.zeros(x_test.size()), torch.ones(x_test.size()))



plt.ion()

plt.scatter(x.numpy(), y.numpy())
plt.show()


model = torch.nn.Sequential(
        torch.nn.Linear(1, 300),
        torch.nn.ReLU(),
        torch.nn.Linear(300, 300),
        torch.nn.ReLU(),
        torch.nn.Linear(300, 1),
        )
model_dropped = torch.nn.Sequential(
        torch.nn.Linear(1, 300),
        torch.nn.Dropout(0.5),
        torch.nn.ReLU(),
        torch.nn.Linear(300, 300),
        torch.nn.Dropout(0.5),
        torch.nn.ReLU(),
        torch.nn.Linear(300, 1),
        )



opt = torch.optim.Adam(model.parameters(), lr=learning_rate)
# !! important. "model_dropped.parameters" don't forget to alter
opt_dropped = torch.optim.Adam(model_dropped.parameters(), lr=learning_rate)

loss_fn = torch.nn.MSELoss()

for epoch in range(500):
    y_pred = model(x)
    loss = loss_fn(y_pred, y)
    opt.zero_grad() # clear the current grads
    loss.backward() 
    opt.step() # update the grads

    y_pred_dropped = model_dropped(x)
    loss_dropped = loss_fn(y_pred_dropped, y)
    opt_dropped.zero_grad() # clear the current grads
    loss_dropped.backward() 
    opt_dropped.step() # update the grads


    if epoch % 20 == 0:

        model.eval()
        model_dropped.eval()
        y_pred = model(x_test)
        y_pred_dropped = model_dropped(x_test)
        model.train()
        model_dropped.train()

        plt.cla() # clear the current axes
        plt.scatter(x.data.numpy(), y.data.numpy(), c='magenta', label='train')
        plt.scatter(x_test.data.numpy(), y_test.data.numpy(), c='cyan', label='test')
        plt.plot(x.data.numpy(), y_pred.data.numpy(), 'r-', lw=3, label='overfitting')
        plt.plot(x.data.numpy(), y_pred_dropped.data.numpy(), 'b-', lw=3, label='dropout(50%')
        print('epoch %d: loss=%.4f loss_dropped=%.4f' % (epoch, loss_fn(y_pred, y_test).data.numpy(), loss_fn(y_pred_dropped, y_test).data.numpy()))
        plt.legend(loc='upper left')
        plt.ylim((-2.5, 2.5))
        plt.pause(0.1) # pause
    



plt.ioff()
plt.show() # if commented, then the picture box will disappear when the program finishes


