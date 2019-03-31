import torch
from torch.autograd import Variable
import numpy as np
import matplotlib.pyplot as plt

batch_size = 64
art_components = 15
paint_points = np.vstack([np.linspace(-1, 1, art_components) for _ in range(batch_size)]) # [batch_size, art_components]
n_ideas = 5

def artist_works():     # painting from the famous artist (real target)
    a = np.random.uniform(1, 2, size=batch_size)[:, np.newaxis]
    paintings = a * np.power(paint_points, 2) + (a-1)
    paintings = torch.from_numpy(paintings).float()
    return paintings

artist_works()

G = torch.nn.Sequential(
        torch.nn.Linear(n_ideas, 128),
        torch.nn.ReLU(),
        torch.nn.Linear(128, art_components),
        )

D = torch.nn.Sequential(
        torch.nn.Linear(art_components, 128),
        torch.nn.ReLU(),
        torch.nn.Linear(128, 1),
        torch.nn.Sigmoid(),
        )

G_optimizer = torch.optim.Adam(G.parameters(), lr=0.0001)
D_optimizer = torch.optim.Adam(D.parameters(), lr=0.0001)

for epoch in range(10000):
    artworks = artist_works()
    ideas = Variable(torch.from_numpy((np.vstack(torch.randn(5) for _ in range(batch_size)))))
    fake_out = G(ideas)

    fake_pred = D(fake_out)
    real_pred = D(artworks)

    # The loss maybe incorrect? use BCELoss?
    D_loss = - torch.mean(torch.log(real_pred)) - torch.mean(torch.log(1 - fake_pred))
    G_loss = torch.mean(torch.log(1 - fake_pred))

    # should we use different optimizers? => yes
    D_optimizer.zero_grad()
    D_loss.backward(retain_graph=True)
    D_optimizer.step()
    
    G_optimizer.zero_grad()
    G_loss.backward()
    G_optimizer.step()
    

    plt.ion()
    plt.show()

    if epoch % 100 == 0:
        print("epoch {0}: G_loss={1} D_loss={2}".format(epoch, G_loss.data.numpy(), D_loss.data.numpy()))
        steps = np.linspace(-1, 1, art_components)
        plt.cla()
        plt.plot(steps, fake_out[0].data.numpy())
        plt.plot(steps, 2 * np.power(paint_points[0], 2) + (2-1))
        plt.plot(steps, 1 * np.power(paint_points[0], 2) + (1-1))
        plt.show()
        plt.pause(0.5)

plt.ioff()
plt.show()
