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
+ `\log_5{x}`
+ `\ln{x}`
+ `\displaystyle{\frac{1}{x^2+1}}` = `\dfrac{}`
+ `\left|\frac{x}{x+1}\right|`, `3\left(\frac{x}{x+1}\right)`, `\left. \frac{dy}{dx} \right|_{x=1}`(remain only right vertical bar)
