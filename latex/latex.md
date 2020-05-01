## Syntax

### Formating
1. newline: Note that a single newline represents a blank
+ `\\`
+ a blank line

### Chinese support
1. `\usepackage[UTF8]{ctex}`

2. settings >> compiler => XeLaTeX

### Hyperlink
```
\usepackage{hyperref}
\hypersetup{
    colorlinks=true,
    linkcolor=blue,
    filecolor=magenta,      
    urlcolor=cyan,
}

%....
\href{https://github.com/orris27}{Github}
```



## Relative Link
[Templates](https://www.overleaf.com/gallery/)

## bibliography
main.tex:
```
\begin{document}
    % ...
    Sample: \cite{dai2017towards}.
    \bibliographystyle{eg-alpha-doi}
    \bibliography{reference}
\end{document}
```
reference.bib:
```
@inproceedings{dai2017towards,
  title={Towards diverse and natural image descriptions via a conditional gan},
  author={Dai, Bo and Fidler, Sanja and Urtasun, Raquel and Lin, Dahua},
  booktitle={Proceedings of the IEEE International Conference on Computer Vision},
  pages={2970--2979},
  year={2017}
}
```
## Math
### 1. symbol
+ `\log_5{x}`
+ `\ln{x}`
+ `\displaystyle{\frac{1}{x^2+1}}` = `\dfrac{}`
+ `\left|\frac{x}{x+1}\right|`, `3\left(\frac{x}{x+1}\right)`, `\left. \frac{dy}{dx} \right|_{x=1}`(remain only right vertical bar)

## Packages
### 1. amsmath
a single equation
```
\begin{equation} \label{eq1}
y = x^2 + 1
\end{equation}
A single equation \ref{eq1}.
```

long equation: the first part will be aligned to the left and the second part will be displayed in the next line and aligned to the right. `*` can toggle the equation numbering. When numbering is allowed, we can label each row individually.
```
\begin{multline*}
    p(x) = p(x) = 3x^6 + 14x^5y + 590x^4y^2 + 19x^3y^3 \\
    - 12x^2y^4 - 12xy^5 + 2y^6 - a^3b^3
\end{multline*}
```

align several equations: the ampersand character `&` determines where the equations align
```
\begin{align*}
    2x - 5y & = 8 \\
    3x + 9y & = -12 
\end{align*}

% Another example

% ampersand character determines where euqations align and separater different equations in one row
\begin{align*}
    x       & = y      & w     & = z            & a  & = b + c \\
    2x      & = -y     & 3w    & = \frac{1}{2}z & a  & = b \\
    -4 + 5x & = 2 + y  & w + 2 & = -1 + w       & ab & = cb
\end{align*}

```
grouping and centering equations: display a set of equations without alignment
```
\begin{gather*}
    2x - 5y = 8 \\
    3x^2 + 9y = 3a + c
\end{gather*}
```
constraints problem
```
\begin{equation*}
\begin{aligned}
& \underset{x}{\text{minimize}}
& & f_0(x) \\
& \text{subject to}
& & f_i(x) \leq b_i, \; i = 1, \ldots, m.
\end{aligned}
\end{equation*}
```

### 2. graphicx
`\usepackage{graphicx}`

1. include the image in the document
```
\graphicspath{{./images/}} % 1. the images are kept in the folder named images; 2. it is best practice to specify the graphics path to be relative to the main .tex file, denoting the main .tex file directory as ./; 3. Trailing slash / can make the path absolute; 4. If no graphics path is set, the LaTeX will look for images in the folder where the .tex file the image is included in is saved

\includegraphics{images/leaky_relu.png} % 1. including extension is okay but not suggested; 2. should not contain white spaces nor multiple dots
```

2. 1

### 3. Ubuntu
```
sudo apt install texlive-latex-extra
sudo apt install texlive-science
pdflatex paper_for_review.tex
```
### 4. table
```
\begin{table}
    \caption{Data sets.\label{table:datasets}}
    \centering
        \begin{tabular}{ |c|c|c|c|c| }
        \hline
        Dataset & N & n & Unscaled & Scaling Used \\
        \hline
        a9a & 32561 & 123 & & \\
        australian & 690 & 14 &  & \\
        colon-cancer & 62 & 2000 & \checkmark & into[-1, 1]  \\
        \hline
        \end{tabular}
\end{table}

```


### 5. other
\`\`N""


horizontal line thicker
```
\usepackage{makecell}
\Xhline{2\arrayrulewidth} % replace \hline

```


context padding for table
```
\bgroup
\def\arraystretch{1.5}%  1 is the default, change whatever you need
\begin{tabular}{|c|...}
...
\end{tabular}
\egroup
```


example table
```
\usepackage{makecell}





\begin{table}
    \caption{Objective function values for \proxsg{}, \rda{}, \proxsvrg{} and \algacro{} on convex problems.\label{table:f_value_convex}}
    \centering
    \resizebox{\columnwidth}{!}{%
    \def\arraystretch{1.1}%  1 is the default, change whatever you need
    \begin{tabular}{@{\extracolsep{4pt}}cccccc @{}}
        \Xhline{3\arrayrulewidth}
        \multirow{2}{*}{Problems} & \multirow{2}{*}{\proxsg{}} & \multirow{2}{*}{\rda{}} & \multirow{2}{*}{\proxsvrg{}} & \multicolumn{2}{c}{\algacro{}} \\
        \cline{5-6}

        & &  &  & Dual Average & Batching \\
        \hline
        a9a & 0.341 & 0.338 & {\color{red} 0.332} & {\color{blue} 0.333} & 0.337  \\
        australian & 0.398 & 0.426 & {\color{red} 0.396} & {\color{red} 0.396} & {\color{red} 0.396}  \\
        colon-cancer & {\color{red} 0.692} & 0.693 & {\color{red} 0.692} & 0.693 & 0.693  \\
        cor-rna & 0.191 & 0.195 & {\color{red} 0.189} & {\color{red} 0.189} & 0.19  \\
        covtype.libsvm & 0.111 & {\color{red} 0.023} & 0.309 & 0.972 & {\color{blue} 0.105}  \\
        diabetes & {\color{red} 0.579} & 0.596 & {\color{red} 0.579} & 0.58 & {\color{red} 0.579}  \\
        duke-scaled & {\color{red} 0.693} & {\color{red} 0.693} & {\color{red} 0.693} & {\color{red} 0.693} & {\color{red} 0.693}  \\
        fourclass & {\color{blue} 0.515} & 0.522 & {\color{red} 0.514} & {\color{blue} 0.515} & {\color{blue} 0.515}  \\
        \hline
        best & 9 & 3 & 17 & 13 & 16 \\
        second best & 2 & 0 & 0 & 5 & 3 \\
        % sum of best and second best & 11 & 3 & 17 & \color{blue}{18} & \color{red}{19} \\ 
        sum  & 11 & 3 & 17 & \color{blue}{18} & \color{red}{19} \\
        %\hline
        \Xhline{3\arrayrulewidth}
    \end{tabular}
    }
\end{table}

```

bibtex:
```
pdflatex paper_for_review
bibtex paper_for_review
pdflatex paper_for_review
pdflatex paper_for_review

```
figure
```
\begin{figure}[]
    \captionsetup{justification=centering}
    \begin{subfigure}[t]{0.323\textwidth}
        %\includegraphics[scale=0.3]{figures/vgg16_mnist/vgg16_mnist_accuracy_nonconvex.pdf}
        \includegraphics[width=\linewidth]{figures/vgg16_mnist/vgg16_mnist_accuracy_nonconvex.pdf}
    \end{subfigure}
    \begin{subfigure}[t]{0.323\textwidth}
        %\includegraphics[scale=0.3]{figures/vgg16_mnist/vgg16_mnist_f_value_nonconvex.pdf}
        \includegraphics[width=\linewidth]{figures/vgg16_mnist/vgg16_mnist_f_value_nonconvex.pdf}
    \end{subfigure}
    \begin{subfigure}[t]{0.323\textwidth}
        %\includegraphics[scale=0.3]{figures/vgg16_mnist/vgg16_mnist_group_sparsity_nonconvex.pdf}
        \includegraphics[width=\linewidth]{figures/vgg16_mnist/vgg16_mnist_group_sparsity_nonconvex.pdf}
    \end{subfigure}
    \caption{vgg16 mnist}
\end{figure}


\begin{figure}
    \centering
    \includegraphics[width=\textwidth]{figures/np10_no100000_mnist_vgg16}
    \caption{Validation accuracy, number of parameters, FLOPs and training time for \algacroplus{} on MNIST with VGG16.}
\end{figure}

```

#### Algorithm
```
\usepackage{amsmath,amsfonts,graphicx}
\usepackage{algorithm}
\usepackage{algpseudocode}
\usepackage{multirow}

\begin{algorithm}
    \caption{DPLL}
    \begin{algorithmic}[1]
        \State (proplist, F) $\gets$ UnitProp($F$)
        \If {$\Box \in F$ }
            \State \textbf{return} UNSET
        \EndIf
        \If {F = \{\}}
            \State \textbf{return} SAT
        \EndIf
        \If {AP(F) $\symbol{92}$ \text{Vars}(decList) == $\phi$}
            \State \textbf{return} SAT
        \EndIf
        \State decList[level] = proplist
        \State Pick $p \in \text{AP}(F)$, Pick $l\in \{p, \neg p\}$
        \State level = level + 1
        \If {DPLL($F\wedge l$, decList, level) == SAT }
            \State \textbf{return} SAT
        \EndIf
        \State \textbf{return} DPLL($F\wedge \neg l$, decList, level)
    \end{algorithmic}
\end{algorithm}
```

change only the table width without changing the font size
```
\begin{tabularx}{\textwidth} { 
   >{\centering\arraybackslash}X 
   >{\centering\arraybackslash}X 
   >{\centering\arraybackslash}X 
   >{\centering\arraybackslash}X 
   >{\centering\arraybackslash}X 
   >{\centering\arraybackslash}X 
   >{\centering\arraybackslash}X }
			\Xhline{3\arrayrulewidth}
            Backbone & Dataset &\proxsg{} & \rda{} & \proxsvrg{} & \algacro{} & \algacroplus{}\\
			\hline%\hline
			\mobilenet{}& \cifar{} & 14.17 & 74.05 & 92.26 & 9.15 & \textbf{\textbf{2.90}}\\
			& \fashionmnist{} & 5.28 & 74.67 & 75.40 & 4.15 & \textbf{\textbf{1.23}}\\\hdashline
			\resnet{} & \cifar{} & 11.60 & 41.01 & 37.92 & 2.12 & \textbf{\textbf{0.88}} \\
			 &\fashionmnist{} & 6.34 & 42.46 & 35.07 & 5.44 & \textbf{\textbf{0.29}}\\
			\Xhline{3\arrayrulewidth} 
		\end{tabularx}
```
