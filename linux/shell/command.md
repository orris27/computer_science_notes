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
可以像{1..30}这样输出一段数字,而且可以用-s指定分隔符,默认为\n
+seq 10={1..10}={1,2,3,4,5,6,7,8,9,10}
### 3-1. 建立img1,2,3的文件夹
```
mkdir img{1...10}
```
### 3-2. 以:分隔输出0到5的数字
```
seq -s: 0 5
```

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
对文件进行行处理的stream editor
### 6-1. 格式
`sed option 'script' file1 file2 file3`
### 6-2. options
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

### 6-3. script
#### 6-3-1. 语法
总体结构是`/pattern/action`,而pattern可以是行数,如`空`,`5`,`2,4`.
1. /pattern/p  # 打印匹配pattern的行
2. /pattern/d  # 删除匹配pattern的行
3. /pattern/s/pattern1/pattern2/   # 查找符合pattern的行，将该行第一个匹配pattern1的字符串替换为pattern2
4. /pattern/s/pattern1/pattern2/g  # 查找符合pattern的行，将该行所有匹配pattern1的字符串替换为pattern2
#### 6-3-2. 指定行
1. 打印所有行
```
sed 'p' b.txt 
```
2. 删除2,3,4行
```
sed '2,4d' b.txt
```
#### 6-3-3. 替换时的& \1 \2
```
sed -r 's/[a-z]{3}/--&--/g' b.txt
```
##### 查看交换分区的大小
1. 查看交换分区大小:`free -m`;total列Swap行的就是了 
2. 如果没有最后的".\*"的话,为什么结果是9624   0   9624?因为sed的s只把前面的替换成\1了,后面的还会输出.所以我们应该把后面的包括进来,一起替换. 
3. 使用* +时一定要定义前后的字符,否则会从后面一路匹配过来
basename include/stdio.h .h     -> "stdio"

```
free -m | sed -nr '/\<Swap/s/Swap:\ +([0-9]+)\ .*/\1/p'
# 或者
free -m | sed -n '/Swap/p' | awk '{print $2}'
```

## 7. awk
根据分隔符(F,可以为多个)对文件按列处理
+ 按一个或多个空格或\t来分隔
### 7-1. 格式
```
awk option 'script' file1 file2 ...
awk option -f scriptfile file1 file2 ...
```

### 7-2. options
1. `-F`: 指定列分隔符为冒号<=>`'BEGIN {FS=":"}'`


### 7-3. script
#### 7-3-1. 语法
```
/pattern/{actions}
condition{actions}
```
##### condition用比较运算符实现缺货判断
```
awk '$2<60{print $0 "\treorder"} $2>=60 {print $0}' product.txt
```
#### 7-3-2. 变量
1. `$0`表示一行
2. `$1`,`$2`表示第1列,第2列
3. `FILENAME`  当前输入文件的文件名，该变量是只读的
4. `NR`  当前行的行号，该变量是只读的，R代表record
5. `NF`  当前行所拥有的列数，该变量是只读的，F代表field
6. `OFS` 输出格式的列分隔符，缺省是空格
7. `FS`  输入文件的列分融符，缺省是连续的空格和Tab
8. `ORS` 输出格式的行分隔符，缺省是换行符
9. `RS`  输入文件的行分隔符，缺省是换行符
##### 查看/etc/passwd的第1列
```
awk 'BEGIN {FS=":"} {print $1;}' /etc/passwd
```
##### 查看/etc/passwd的第1列和第7列
```
awk 'BEGIN {FS=":"} {printf "%s\t%s\n",$1, $7}' /etc/passwd
```
#### 7-3-3. BEGIN END
1. 在'script'里面,如果直接空格`"condition{actions}"`的话,就两个同时处理
2. 如果用END来处理,则先前面全部结束,之后再执行END后的操作
3. 如果是BEGIN,则先处理BEGIN后面的`{actions}`,之后再处理下有一个`{actions}`

## 8. paste
将文件1和文件2按列组成一行
### 字母的组合
```
seq 26 > a.log
echo {a..z} | tr " " "\n" > b.log
paste a.log b.log >c.log
```


## 9. 输入输出
### 9-1. 重定向`>`, `<`, `>>`
#### 9-1-1. `<`
cat用来将文件名转换为内容
```
cat
saber
^C
cat < a.txt
cat a.txt
```
### 9-2. echo
-e解析转义字符;-n不换行
```
echo "hello\n"
echo -e "hello\n"
echo -n "hello"
```
### 9-3. 管道,`|`
左边的输出当做右边命令的输入
### 9-4. tee
输出到stdout,同时做一个副本到指定文件
```
ls
ls | a.txt
ls | tee a.txt
```

## 10. dirname, basename
1. `dirname /server/scripts/n.sh`=>/server/scripts
2. `basename /server/scripts/n.sh`=>n.sh
+ 第二个参数可以跟它的后缀名,从而把后缀名也能剔除掉
```
basename include/stdio.h .h     # => "stdio"
```

## 11. 数学运算
### 11-1. bc
可以计算浮点数.
#### 11-1-1. 使用
1. `bc`进去就能算
2. 管道交给`bc`处理
#### 11-1-2. 计算2\*3
```
echo 2*3 | bc
```
### 11-2. $(())
支持自增/自减,平方,位运算,比较运算符等
#### 计算2\*3
```
echo $((2*3))
```
#### 自增变量a
```
a=2
echo $((a++))
echo $a
echo $((++a))
echo $a
```
#### 比较2和3的大小(这里1为真,而0为假)
```
echo $((2<3))
echo $((2>3))
```
### 11-3. let
先数学运算再赋值.但是`$(())`效率更高
```
a=5
let b=a+10
echo $b
c=a+10
echo $c
```
### 11,位运算,比较运算符等

计算2*3
echo $((2*3))-4. expr
evaluate expression.和`$(())`类似,但是整数运算符左右必须有空格.如果使用`$[]`包住公式的话,就没有空格的限制
#### 11-4-1. 判断扩展名
`ssh-copy-id`里用来判断扩展名
```
if expr "$L_ID_FILE" : ".*\.pub$" >/dev/null 
	then
  		PUB_ID_FILE="$L_ID_FILE"
	else
  		PUB_ID_FILE="$L_ID_FILE.pub"
fi
```
#### 11-4-2. 判断整数
```
expr 1 + $1 &> /dev/null 
[ $? -eq 0 ] && echo "int" || echo "chars"
```
#### 例子
```
expr 1 + 2
expr 1+2
expr $[1+2]
```

## 12. read
### 12-1. 格式
`read [option] [variable_name]`
### 12-2. option
1. `-p`:prompt
2. `-t`:timeout
### 12-3. 一个read命令输入多个变量
直接在变量名的地方多写几个就行了,到时候输入的时候按空格分隔
```
read -t 60 -p 'input a&b:' a b
```


## 0. 实战
### 0-1. 找到/etc/passwd下的shell出现次数
```
cat /etc/passwd | awk -F: '$7!=""{print $7}' | sort | uniq -c
```
