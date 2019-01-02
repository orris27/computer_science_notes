## 操作
### 1. 分屏
1. 上下分屏:`:wp`
2. 左右分屏:`vsp <文件名>`(如果不加文件名的话,就还是当前的文件.此时修改1个文件,另一个文件也会改变)
3. 切换:`Ctrl+W+W`
4. 退出
    1. 退出1个:`:q`
    2. 退出2个:`:qall`(使用于其他)
### 2. 帮助
vim下光标放在函数上,用`shift + k`可以跳到对应man文档.按`q`退出

### 3. 解决粘贴混乱的问题
```
set pastetoggle=<F9>
```
进入insert模式后,按F9进入粘贴模式,然后粘贴,之后再按F9回到普通的insert模式

### 4. python自动注释
```
set fo=r
````

### 5. 部分操作系统backspace失效,比如不能删除前面的字符,或者在行首时不能删除换行
```
set backspace=2
```
## vim.rc
1. 使用指定的vimrc文件： 使用`-u`参数, 如`vim -u /home/user/orris/vimrc my-cartpole.py`

2. 将已经保存的文件的tab转化为4个空格的话:
```
:set ts=4
:set expandtab
:%retab!
```
### 1. 注释版
```
set nocompatible " 设置的话,就是不让vim模仿vi,这会避免很多不兼容问题

set tabstop=4 "tab键=4个空格
set expandtab

set softtabstop=4
set shiftwidth=4  "vim中的"v+>"会移动4个空格
set autoindent
set cindent
set cinoptions={0,1s,t0,n-2,p2s,(03s,=.5s,>1s,=1s,:1s
set nu "设置行号
set hlsearch
syntax on

"解决中文乱码
set fileencodings=utf-8,ucs-bom,gb18030,gbk,gb2312,cp936
set termencoding=utf-8
set encoding=utf-8 "一般乱码用这个就能解决

colorscheme ron " ls /usr/share/vim/vim74/colors/里有,ron是我Ubunut的配色方案

set fileformat=unix
set fileformats=unix,dos,mac

set nobomb "禁止utf-8 bom

filetype on "自动识别文件类型
[root@es-master conf.d]# vim /etc/logstash/conf.d/tcp.conf
[root@es-master conf.d]# vim ~/.vimrc 
[root@es-master conf.d]# vim /etc/logstash/conf.d/tcp.conf
[root@es-master conf.d]# vim ~/.vimrc 

" 设置界面
set number
"set numberwidth=6 "行号所占位置,一般取消就好
"set laststatus=2 "下面会有状态栏
"set cursorline "会在当前行下面显示一条白线
"set guifont=YaHei_Consolas_Hybrid:h12
"set shortmess=atI

" 设置自动缩进
set autoindent " 如果在tab1次的情况下回车或o,那么就会自动锁紧
```

### 2. 无注释
如果打开3个注释的话,就会复制粘贴时自动缩进
```
set nocompatible 
set tabstop=4
set expandtab

set shiftwidth=4  

set cinoptions={0,1s,t0,n-2,p2s,(03s,=.5s,>1s,=1s,:1s
set nu 
set hlsearch
syntax on

set fileencodings=utf-8,ucs-bom,gb18030,gbk,gb2312,cp936
set termencoding=utf-8
set encoding=utf-8
colorscheme ron 
set fileformat=unix
set fileformats=unix,dos,mac

set nobomb 
filetype on
set number

set softtabstop=4
set autoindent
set cindent
set pastetoggle=<F9>
set fo=r
set backspace=2

"set softtabstop=4
"set autoindent
"set cindent
```



### 3. 我双系统下的Ubuntu的vimrc
```
set nocompatible              " 去除VI一致性,必须
filetype off                  " 必须

set fileencodings=utf-8,ucs-bom,gb18030,gbk,gb2312,cp936
set termencoding=utf-8
set encoding=utf-8 

" 设置包括vundle和初始化相关的runtime path
set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()
" 另一种选择, 指定一个vundle安装插件的路径
"call vundle#begin('~/some/path/here')

" 让vundle管理插件版本,必须
Plugin 'VundleVim/Vundle.vim'

" 以下范例用来支持不同格式的插件安装.
" 请将安装插件的命令放在vundle#begin和vundle#end之间.
" Github上的插件
" 格式为 Plugin '用户名/插件仓库名'
Plugin 'tpope/vim-fugitive'
" 来自 http://vim-scripts.org/vim/scripts.html 的插件
" Plugin '插件名称' 实际上是 Plugin 'vim-scripts/插件仓库名' 只是此处的用户名可以省略
Plugin 'L9'
" 由Git支持但不再github上的插件仓库 Plugin 'git clone 后面的地址'
Plugin 'git://git.wincent.com/command-t.git'

" 插件在仓库的子目录中.
" 正确指定路径用以设置runtimepath. 以下范例插件在sparkup/vim目录下
Plugin 'rstacruz/sparkup', {'rtp': 'vim/'}

Plugin 'delimitMate.vim'
Plugin 'Valloric/YouCompleteMe'


" 你的所有插件需要在下面这行之前
call vundle#end()            " 必须
filetype plugin indent on    " 必须 加载vim自带和插件相应的语法和文件类型相关脚本
" 忽视插件改变缩进,可以使用以下替代:
"filetype plugin on
"
" 简要帮助文档
" :PluginList       - 列出所有已配置的插件
" :PluginInstall    - 安装插件,追加 `!` 用以更新或使用 :PluginUpdate
" :PluginSearch foo - 搜索 foo ; 追加 `!` 清除本地缓存
" :PluginClean      - 清除未使用插件,需要确认; 追加 `!` 自动批准移除未使用插件
"
" 查阅 :h vundle 获取更多细节和wiki以及FAQ
" 将你自己对非插件片段放在这行之后

set tabstop=4
set expandtab
set softtabstop=4
" used in v+>
set shiftwidth=4 
set autoindent
set cindent
set cinoptions={0,1s,t0,n-2,p2s,(03s,=.5s,>1s,=1s,:1s
set nu
set hlsearch
syntax on
set pastetoggle=<F9>
set fo=r

```
