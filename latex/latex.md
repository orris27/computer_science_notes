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
`\`\`N""` 
