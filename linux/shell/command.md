## 1. grep
### 1-1. 过滤掉多个字符串
过滤掉crond, sshd, network, rsyslog
```
xxx | grep -vE 'crond|sshd|network|rsyslog'
```

## 2. time
测试命令所需的时间
### 2-1. 测试生成20,000个目录所需的时间
1. time包含住命令,就是在测试该命令的时间了 
2. 创建20,000个目录需要循环20,000次,可以利用`{1..20000}`/`seq 20000`来创建1~20,000的数字
```
time(
	for i in {1..20000}
	do
		mkdir dir/$i
	done

)
```

## 3. seq
seq 10={1..10}={1,2,3,4,5,6,7,8,9,10}

## 4. join
把两个数据库类型的文件进行natural join
### 4-1. 员工的名字和工资用id链接
```
join employee.txt bonus.txt | sort -k2
```
## 5. find
根据文件名去查找,并且可以在这些文件上执行命令
### 5-1. 语法
1. `{} \;`为固定写法
2. ok会向我们询问
3. find条件约束中间加-or和-and(默认)
```
find . -type f -exec ls -l {} \;
find . -type f -ok ls -l {} \;
find . -type d -exec ls -l {} \;
find . -type d -print
find . -perm -644 -print
find . -name "*" -print
find . -user orris -print
find . -type f -size 0 -exec ls -l {} \;
```
### 5-2. 找到的文件名按文件处理
如果使用"-print | xargs"或者"-exec"的话,文件名都会作为文件名处理

### 5-3. 如果不用查当前目录下子目录内的内容,怎么办
给find加参数-maxdepth 1
#### 5-3-1. 查找当前目录下1级的文件夹
`find . -type d -exec ls -l {} \;`会列出当前目录的文件,是因为ls -l会对.这个文件夹执行.所以当前文件夹下的所有文件都会出来
```
find . -maxdepth 1 -type d
```
## 6. xargs
如果直接对用find匹配后文件执行exec命令,就会发生参数列太长的问题,内存会太多.所以xargs就是把匹配到的结果放在管道里,一批批地对这些结果执行命令
### 6-1. 找出当前目录下文件名以c开头的普通文件
```
find . -name \* -type f -print | grep "\<c"
```
### 6-2. 列出所有文件
```
find . -type f -print | xargs -exec ls -l {} \;
```

## 6. sed
### 6-1. options
1. --version            显示sed版本。
2. --help               显示帮助文档。
3. **-n**,--quiet,--silent  静默输出，默认情况下，sed程序在所有的脚本指令执行完毕后，将自动打印模式空间中的内容，这些选项可以屏蔽自动打印。
4. **-e** script            允许多个脚本指令被执行。
5. -f,--file script-file 从文件中读取脚本指令，对编写自动脚本程序来说很棒！ 
6. **-i**,--in-place        直接修改源文件，经过脚本指令处理后的内容将被输出至源文件（源文件被修改）慎用！ 但是可以用来黑客植入脚本
7. -l N, --line-length=N 该选项指定l指令可以输出的行长度，l指令用于输出非打印字符。
8. --posix             禁用GNU sed扩展功能。
9. **-r**, --regexp-extended  在脚本指令中使用扩展正则表达式
10. -s, --separate      默认情况下，sed将把命令行指定的多个文件名作为一个长的连续的输入流。而GNU sed则允许把他们当作单独的文件，这样如正则表达式则不进行跨文件匹配。
11. -u, --unbuffered    最低限度的缓存输入与输出。
#### 简单例子
```
sed -n '/[0-9]{3}/p' b.txt 
sed -n '/[0-9]\{3\}/p' b.txt 
sed -nr '/[0-9]{3}/p' b.txt 
find ./ -type f | xargs sed -i '1 i <script src="www.baidu.com"></script>' # 1表示第1行,i表示insert
find ./ -type f | xargs sed -in '/<script src="www.baidu.com"><\/script>/d'
sed 's/\<io//g;s/\<io//g' b.txt
sed -e 's/\<io//g' -e 's/\<io//g' b.txt
```

### 6-2. script
#### 6-2-1. 指定行
1. 打印所有行
```
sed 'p' b.txt 
```
2. 删除2,3,4行
```
sed '2,4d' b.txt
```
#### 6-2-2. 替换时的& \1 \2
##### 查看交换分区的大小
1. 查看交换分区大小:`free -m`;total列Swap行的就是了 
2. 如果没有最后的".\*"的话,为什么结果是9624   0   9624?因为sed的s只把前面的替换成\1了,后面的还会输出.所以我们应该把后面的包括进来,一起替换. 
3. 使用* +时一定要定义前后的字符,否则会从后面一路匹配过来

```
free -m | sed -nr '/\<Swap/s/Swap:\ +([0-9]+)\ .*/\1/p'
# 或者
free -m | sed -n '/Swap/p' | awk '{print $2}'
```
