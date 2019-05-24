## Syntax

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
