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
### 3-1.option
1. `-s`:指定分隔符
2. `-w`:和最大的数字统一宽度,设置前导0.如`seq -w 10`和`seq -w 100`
### 3-2. 建立img1,2,3的文件夹
```
mkdir img{1...10}
```
### 3-3. 以:分隔输出0到5的数字
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
将文件1和文件2按列组成一行n
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
#### 颜色
格式为`echo -e "\033[40;30mcontent\033[0m"`,只要把`40`,`30`和`content`处变成想要的内容就可以了
1. `40`:背景颜色
2. `30`:字体颜色
3. `content`:输出内容
##### 字颜色
30-37
```
echo -e "\033[30m====black====\033[0m"
echo -e "\033[31m====red====\033[0m"
echo -e "\033[32m====green====\033[0m"
echo -e "\033[33m====yellow====\033[0m"
echo -e "\033[34m====blue====\033[0m"
echo -e "\033[35m====purple====\033[0m"
echo -e "\033[36m====cyan====\033[0m"
echo -e "\033[37m====gray====\033[0m"
echo -e "\033[38m====white====\033[0m"
```


##### 字背景颜色范围
40–47 


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
### 11-3. let,(())
先数学运算再赋值.但是`$(())`,`(())`效率更高
+ 推荐使用`(())`
+ `(())`和``$(()))``的区别在于前面那个是命令,后面是一个字符串.所以直接`((i++))`是可以的,而`$((i++))`是不允许的,只能`a=$((i++))`
+ `(())`常用在`while`中做判断
```
a=5
let b=a+10 # ((b=a+10))
echo $b
c=a+10 # ((c=a+10))
echo $c
```
### 11-4. expr
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
`&>/dev/null`=`>/dev/null 2>&1`,前面其实是把1,2,3,..都重定向到/dev/null
> https://unix.stackexchange.com/questions/70963/difference-between-2-2-dev-null-dev-null-and-dev-null-21
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
### 12-4. 文件读取每一行处理
1. 管道
```
cat a.log | while read line
do
	echo ==="$line"===
done
```
2. 重定向
```
while read line
do
	echo ==="$line"===
done < a.log
```
3. `exec`
```
exec < a.log
while read line
do
	echo ==="$line"===
done
```


## 13. vimdiff
比较两个文件(如两个txt文件)的差异
```
echo 123 > a.txt
echo 1234 > b.txt
vimdiff a.txt b.txt
```

## 14. head
获取前面几行,默认10行,`-n5`表示5行

## 15. tail
获取后面几行,默认10行,`-n5`表示5行
### option
1. `-n`:几行
2. `-f`:检测文件结尾的变化
+ 看日志很方便
```
tail -f a.txt # 然后如果我们在a.txt里写点东西,就会看到结果了
```

## 15. sort
以行为单位排序,默认按字符进行比较,`-n`则按数字大小比较
### 格式
`sort [option] file...`
### option
1. `-f`: 忽略字符大小写；
2. `-n`: 比较数值大小；
3. `-t`: 指定分隔符
4. `-k`: 分隔符给定后,指定第几列
5. `-u`: 重复的行，只显示一次；
6. `-r`: 倒排
### 对当前目录下的所有文件名排序输出
```
ls | sort
```
## 16. wc
### 格式
`wc [option] [file]...`
### option
1. `-l`:统计行数
2. `-c`:统计字节数
3. `-w`:统计单词数
4. `-L`:统计所有行中最大长度.我们统计一个字符串的长度除了`${#xx}`以外,也可以用`echo $xx | wc -L`.不要用`-c`和`-m`,会多1的,可能是因为C语言一样是用`\0`做结束的原因把
### 查看当前目录下主人为orris的可执行文件数量
找到所有普通文件后,打印相关信息,行处理这些信息筛选得到可执行文件,打印出来的输出作为wc的输入,统计行数就行了
+ wc如果参数为文件名,则会统计里面的结果.所以不能find的结果直接给wc
+ xargs一般配合`find .. |`,而不是单独的`|`
```
echo "orris's bins: $(find ./ -type f -user orris -print | xargs ls -l | sed -n '/-..x/p' | wc -l)"
```

## 17. uniq
移除相邻的重复的行
### option
1. `-c`: 显示每行重复的次数
2. `-d`: 仅显示重复过的行
3. `-u`: 仅显示不曾重复的行
### 寻找出现次数前三的单词
某个文件中第一列存的都是单词.先字典排序,重复的统计个数.这样第一列为个数,第二列为单词.根据第一列排序.最后取前三个
1. 统计个数:因为uniq只处理相邻行,所以先排序
2. 根据第一列排序方法:使用sort的k参数.k后面跟第几列
3. 11比2的字典序小怎么办:用sort的n参数,可以按数值大小排序
4. 取前三个怎么弄.用head -n3

```
#! /bin/sh

awk '{print $1}' $1 | sort | uniq -c | sort -k1nr | head -n3
```

## 18. cat
1. 打印文件内容
2. 清空文件内容
3. 合并两个文件内容
```
echo "123"> a.txt
echo "456" > b.txt
cat a.txt b.txt 
cat a.txt b.txt  > ab.txt
```
4. 非交互式编辑/追加内容
+ 编辑
```
cat > ab.txt <<EOF
hello
fun
EOF
```
+ 追加
```
cat >> ab.txt <<EOF
hello
fun
EOF
```
#### 用cat设计一个菜单
cat 利用EOF可以更好地格式化输出,里面的tab也会得到保留
```
#! /bin/sh
menu () {
cat <<EOF
	===========================
	1. [install lamp]
	2. [install lnmp]
	3. [exit]
	please input the number
	===========================
EOF
}
menu 
```

## 18. 压缩
### 18-1. tar
注意不要用`/`开头,尽量`cd`进去压缩
+ `/`开头的话,在压缩包里面会多出这些目录来保持这种路径,比如说压缩`/application/mysql/*`的话,会在压缩包里创建`application/mysql`这个目录,之后才把内容放进去
#### option
1. 压缩技术:`-z`,`-j`
2. 创建或提取:`-c`,`-x`
3. 显示输出:`-v`
4. 文件:`-f`
5. 查看内容:`-tf`,比如查看`a.tar.gz`的内容:`tar tf a.tar.gz`
6. 不压缩某个文件:`--exclude`
+ 不压缩一些文件:`--exclude-from`,指定的文件里面的内容就是我们不想要压缩进来的内容,当然要自己的文件名也要写进去,否则就自己就会被压缩进去
7. 解压到某个位置:`-C`,解压new_house到house这个目录下:`tar -zxvf new_house.tar.gz -C house`
#### 压缩时,会把文件夹也压缩进去吗?解压时,会产生和压缩包同名的目录吗?
因为压缩时第二个文件名可以是正则表达式,所以可以为多个.所以就直接都放在了压缩包里.然后解压的时候,也是直接把里面的东西拿到目录下. `tar.gz`本身不会作为文件名导入导出
1. 会
2. 不会

#### 实战
##### 将当前目录下的除了4.txt以外的文件都打包到no_4.tar.gz
```
tar -zcvf no_4.tar.gz --exclude 4.txt *.txt
```

##### 将dir1以及内容打包到gz_dir1.tar.gz中并解压
```
tar -zcvf gz_dir1.tar.gz ./dir1
tar -zxvf gz_dir1.tar.gz
```

##### 当前目录下除了1~3.txt的文件外,都打包到no_123.tar.gz里
```
for i in `seq 5` ; do touch $i.txt ; done # 当前文件夹下只有1~5.txt
touch filter.txt 
for i in `seq 3` ; do echo $i.txt >> filter.txt ; done 
tar -zcvf no_123.tar.gz -X filter.txt *.txt # 最后会连filter.txt也压缩进来
```

### 18-2. zip
#### 压缩minisql文件夹到minisql.zip
```
zip -r minisql.zip minisql/*
```
#### 解压zip
```
unzip xxx.zip
unzip -o xxx.zip # 会直接覆盖已经存在的文件
```

### 18-3. unrar
Linux不要用rar
#### 解压python.rar文件到当前目录(本身不创建目录)
```
unrar x python.rar
```

## 19. history
### 删除序号为2000的命令的历史
```
history -d 2000
```

## 20. netstat(deleted)
查看网络状况,一般我们使用`sudo netstat -lntup | grep mysqld`这样的方式
### option
1. `-u`:udp
2. `-t`:tcp

## 21. cut
截取内容
+ 指定分隔符(`-d`)取出对应列(`-f`)(类似于awk的用法)
+ 按照字节位置(`-c`)截取内容
### 假定文件内容是`I am a good boy,and I am 20`取出boy和20
```
# 1
cut -c 13-15,25-27 1.txt
# 2
cut -d" " -f5,8 1.txt  | sed -n 's/\,and//p'
# 3
awk -F '[ ,]'  '{print $5,$9}' 1.txt # awk可以指定多个分隔符
```


## 22. date
### 按一定格式输出
`date +xx`在xx的地方写下面的内容即可格式化输出日期.`+`后面不能有`:`
1. `%y`:18
2. `%Y`:2018
3. `%m`:07
4. `%d`:20
5. `%F`:2018-07-20
#### 18-06-27格式输出
```
date +%y-%m-%d
```
#### 备份dir1文件夹,加上日期
在crontab中如果不是执行脚本文件而是直接写tar的话,`%`必须转义
```
tar -zcvf dir1-$(date +%F).tar.gz ./dir1
```
#### 输出三天以前的时间
```
date +%F --date='3 day ago'
date +%F -d '3 day ago'
date +%F -d '+3 day' # 3天后Chain INPUT (policy ACCEPT)
target     prot opt source               destination         
ACCEPT     all  --  0.0.0.0/0            0.0.0.0/0            state RELATED,ESTABLISHED
ACCEPT     icmp --  0.0.0.0/0            0.0.0.0/0           
ACCEPT     all  --  0.0.0.0/0            0.0.0.0/0           
ACCEPT     tcp  --  0.0.0.0/0            0.0.0.0/0            state NEW tcp dpt:22
REJECT     all  --  0.0.0.0/0            0.0.0.0/0            reject-with icmp-host-prohibited
DROP       tcp  --  0.0.0.0/0            0.0.0.0/0            tcp dpt:22
DROP       tcp  --  0.0.0.0/0            0.0.0.0/0            tcp dpt:22

Chain FORWARD (policy ACCEPT)
target     prot opt source               destination         
REJECT     all  --  0.0.0.0/0            0.0.0.0/0            reject-with icmp-host-prohibited

Chain OUTPUT (policy ACCEPT)
target     prot opt source               destination         

date +%F -d '-3 day'
date +%F -d '3 day' # 3天后
date +%F-%H -d '3 hour'
```


## 23. tree
### 23-1. option
1. `-L 1`可以指定显示1层
2. `-d`可以只显示目录
#### 显示/下的一层树结构
```
tree -L 1 /
```


## 24. locate
通过updatedb,查找所有文件中文件名包含某个字符串的文件.查不到updatedb没有记录的文件.如果要的话,必须更新updatedb
### 查找mkdir的位置
```
locate -n 1 mkdir # 显示前面1行
```

## 25. 进程
### 25-1. strace
跟踪某个命令在系统中具体是怎么调用的
#### 跟踪cat 
```
strace cat b.txt
```
#### 跟踪sleep
```
strace sleep 2
```
### 25-2. ltrace
跟踪进程调用库函数的情况
#### 跟踪cat 
```
ltrace cat b.txt
```
### 25-3. bg
将暂停的任务放到后台执行,常使用`Ctrl+Z`+`bg`的方式配合
+ `Ctrl+Z`:暂停当前任务,而不是放到后台运行;只有再输入bg才会
#### 后台执行
1. `sh xx.sh &`
2. `nohub xx.sh &`
3. `screen`
4. `bg`

### 25-4. jobs
查看后台运行和暂停了的任务
+ 如果前台有运行中的任务,那么`jobs`本身就用不了.

### 25-5. fg
将当前后台运行的任务放到前台
+ 如果一个后台运行的进程不断输出到stdout,那么我们需要先拿到前台,之后才能`Ctrl+C`消灭
+ `Ctrl+C`:结束当前的前台任务(后台不行)
#### 将jobs中的第2个任务拿到前台
```
fg 2
```

### 25-6. nohub
用户退出系统之后继续工作
### 25-7. `source`, `sh`, `.`, `./`
1. `source`,`.`:都是直接在当前的环境中执行里面的语句,相当于文件里面的内容在当前环境中展开,类似于inline的感觉.
+ `source`和`.`可以将里面的局部变量和函数导出到当前环境
+ `/etc/functions`就是这样使用的
2. `sh`:单独开辟一个进程来处理脚本文件
+ `sh`无法引入被执行的文件里的变量
3. `./`:我们只讨论指定解释器的可执行脚本,如`#! /bin/sh`.这种情况下就是用指定的解释器去执行它,换句话说,等效于`/bin/sh ./xx.sh`,所以也是单独开一个进程处理脚本文件




## 26. 网络
### 26-1. hostname
#### 查看主机名
```
hostname # 查看当前主机名
```
#### 修改
1. 登出登录才生效
2. 短期
```
hostname new_orris # 临时修改当前主机名为new_orris
```

### 26-2. dig
查看dns
#### 查看百度的dns
```
dig www.baidu,com
```


### 26-3. nslookup
查看dns(Ubuntu上有)
#### 查看百度的dns
```
nslookup www.baidu.com
```

### 26-4. traceroute
可以跟踪的ping方法

### 26-5. telnet
检测服务器的端口是否可以连通
#### 检测
如果接通的话,就是处于输入/输出状态;否则就是连接失败的错误提示,并返回shell
```
telnet 10.0.0.25 80
telnet 10.0.0.25 111 # 查看111端口是否通
telnet 127.0.0.1 11211 # 可以直接操作memcache
```
### 26-6. tcpdump
抓包工具
```
tcpdump -n icmp -i eth0
```

### 26-7. route
查看/修改网关
#### option
1. `-n`:查看网关

#### 默认网关
当上面几行的网关都不行的时候,就使用该网关
+ Destination为0.0.0.0那行的Gateway,同时也应该是最后一行

##### 修改默认网关

###### 修改配置文件
+ 修改/etc/sysconfig/network
+ 网卡的配置文件(ifcfg-eth0)(网卡的配置文件优先级高)

###### 删除(临时生效)
如果重启,服务器又会从配置文件里找默认网关
```
route del default gw 10.0.0.254 # 后面一个为实际电脑的默认网关
```

###### 添加(临时生效)s
```
route add default gw 10.0.0.254
```

###### 重启网卡
```
/etc/init.d/network restart
```




### 26-8. netstat
Print network connections, routing tables, interface sta‐tistics, masquerade connections, and multicast memberships
+ 查看网络状况,一般我们使用`sudo netstat -lntup | grep mysqld`这样的方式
### option
1. `-u`:udp
2. `-t`:tcp


### 26-9. ping
#### option
1. `-c`:发送几次
#### ping 百度2次
```
ping -c2 www.baidu.com
```


### 26-10. scp
secure copy (remote file copy program)
#### option
1. `-P`:指定端口

### 26-11. nmap
检查端口
#### 查询10.0.0.7的从1到65535的端口情况
```
nmap 172.16.55.137 -p1-65535
```

### 26-12. curl
获取页面的内容,并打印到stdout中
#### option
1. `-I`:获取响应头
2. `-o`:输出到某个文件中,文件名自定
3. `-O`:输出到某个文件中,文件名根据URL来确定(所以我们不用写参数)e that you're looking for used information that you entered. Returning 
4. `-s`:silent,不输出错误信息和进度条
5. `-w`:决定输出格式.如输出状态码`curl -I -s -w "%{http_code}" www.baidu.com -o /dev/null`

### 26-13. wget
#### option
1. `--spider`:按爬虫的方式去爬,不会去下载东西,只是去确认这个网站是否存在
2. `--tries=N`:设定重试次数,会一直重试N次,直到确实访问到网站
3. `--timeout=seconds`:超时时间
#### 用wget来确认Web是否正常
```
wget --spider --timeout=10 --tries=2 127.0.0.1
echo $? # 如果Web正常,就是0;否则非0
```


### 26-14. ifconfig
#### 26-14-1. 在环回接口上绑定IP地址
```
ifconfig lo:0 172.19.28.82/32 up
```
#### 26-14-2. 禁用网络接口
```
ifconfig eth0:0 down
```

## 27. chkconfig
设置开机自启动.
+ 如果是自己设计的脚本的话,想要开机启动,可以添加到`/etc/rc.local`(添加的方法类似于`crontab`)
### 添加自定义服务到chkconfig
类似于systemctl
1. 根据对应语法添加到指定目录下
+ `chkconfig`的语法:只需要在开头加两行就行了,需要设定开机级别,启动优先级,关闭优先级(具体`man chkconfig`).`systemctl`则是模块的语法
- `ll /etc/rc.d/rc3.d/`下面看有没有对应级别,有的话就换一个
+ `chkconfig`的指定目录:`/etc/init.d`
+ 添加的两行需要有注释
```
#! /bin/sh
# chkconfig: 2345 21 60
# description: Starts and stops mysqld_multi
... # 比如说可以方mysql多实例 https://github.com/orris27/orris/blob/master/database/mysql/mysqld_multi.md
```
2. 对`chkconfig`重新加载下就好了
```
chkconfig --add xxx # 这里的xxx为放在指定目录下的文件名,如mysqld
```
3. 检查是否添加成功
```
chkconfig --list xxx # 如mysqld
```

## 28. kill
#### Terminated
如果进程A被杀死了,就会显示Terminated.见下面
> https://github.com/orris27/orris/blob/master/linux/shell/marvelous.md

## 29. md5sum
### 29-1. option
1. `-c`:read MD5 sums from the FILEs and check them.
### 29-2. 获得`a.txt`的指纹
```
md5sum a.txt # 比如说输出"ba1f2511fc30423bdbb183fe33f3dd0f  a.txt"
```
### 29-3. 比较指纹
```
md5sum -c md5sum.db # 由于我们是直接把md5sum的输出放到md5sum.db中的,所以格式都是<指纹,文件>的形式,所以我们只要指定指纹库,md5sum就会自动检查每一行的文件是否和该行的指纹对应
```

### 29-4. 检测站点文件是否被篡改
只使用`md5sum`的话,是检测不到新文件的(因为`-c`只检查里面出现过的文件名,由于新建的文件不在指纹库里面,所以也不会被检查),所以还要自己判断文件数.
#### 29-4.1. 思路
1. 代码上线后,获得文件的指纹库
2. 记录正确的文件名
3. 每5分钟,对指纹库检查,如果有错就记录到错误日志中
4. 记录当前的文件名,与原来的文件名库比较,如果有错,就记录到错误日志中
5. 如果错误日志不为空,就发送邮件到管理人中
#### 29-4.2. 实战
##### 假设
+ 监听目录为`/tmp/www`
+ 指纹存放在`/tmp/www_check/md5sum.db`
+ 正确的文件库放在`/tmp/www_check/files.log`
+ 错误日志放在`/tmp/www_check/error.log`
+ 当前的文件库放在`/tmp/www_check/curr_files.log`
###### 代码上线后,先更新指纹库等
```
#! /bin/sh

site="/tmp/www"

find "$site" -type f | xargs md5sum >/tmp/www_check/md5sum.db
find "$site" -type f > /tmp/www_check/files.log
```
###### 定时检查
```
#! /bin/sh

site="/tmp/www" # the site to be checked
path="/tmp/www_check" # the directory to store info
error_log="$path"/"error.log"
md5sum_db="$path"/"md5sum.db"
files="$path"/"files.log"
curr_files="$path"/"curr_files.log"
tmp="$path"/"tmp"

while true
do
	
	# check md5sum
	md5sum -c "$md5sum_db" 2>"$error_log" | grep -v 'OK' >"$error_log" 2>&1

	# check files
	find "$site" -type f > "$curr_files"
	diff "$files" "$curr_files" >> "$error_log"

	sleep 300
done
```
## 30. dos2unix
如果脚本文件是在除了Linux以外的操作系统编辑,那么可能会出现结尾为`^M`等错误,这种时候需要格式化脚本
```
dos2unix xx.sh
```

## 31. sh
创建一个子进程来处理shell脚本文件
### option
1. `-x`:将执行的内容和输出显示出来,用来调试
+ 如果要调试代码块的话,就在前后分别加上`set -x`,`set +x`就可以了

## 32. arp
具体介绍
> https://coggle.it/diagram/WyYWb0Yeep9zuJ9R/t/protocol
### option
1. `-a`:查看arp缓存
2. `-d address`:删除某个ip地址的arp缓存

## 33. arping
### ARP广播从而获取新的地址解析
```
/sbin/arping -I eth0 -c 3 -s 10.0.0.162 10.0.0.253
/sbin/arping -U -I eth0 10.0.0.162
```

## 34. lsmod
Show the status of modules in the Linux Kernel
### 34-1. 检测Linux内核是否已经有ipvs
```
lsmod | grep ip_vs
```

## 35. modprobe
Add and remove modules from the Linux Kernel
### 35-1. 添加ip_vs到Linux内核
```
modprobe ip_vs
```

## 36. ipvsadm
ipvs的管理工具
### 36-1. option
1. `--add-service,-A`:add virtual service with options
2. `--delete-service,-D`:delete virtual service
3. `--clear,-C`:clear the whole table
4. `--add-server,-a`:add real server with options
5. `--delete-server,-d`:delete real server
6. `--list,-L|-l`:list the table
7. `--persistent,-p[timeout]`:persistent service
8. `--weight,-w`:capacity of real server
9. `--scheduler,-s scheduler`:one of rr|wrr|lc|wlc|lblc|lblcr|dh|sh|sed|nq, the default is wlc
10. `--set tcp tcpfin udp`:set connection timeout values
11. `--tcp-service,-t service-address`:service-address is host[:port]
12. `--real-server,-r server-address`:server-address is host (and port)
13. `--gatewaying,-g`:gatewaying (direct routing) (default)
### 36-2. 实战
#### 36-2-1. 删除real server
```
ipvsadm -d -t 172.19.28.82:80 -r 172.19.28.83:80
```
#### 36-2-2. 删除virtual server
```
ipvsadm -D -t 172.19.28.82:80
```
#### 36-2-3. 配置操作
> https://github.com/orris27/orris/blob/master/linux/lvs/installation.md

## 37. 磁盘和文件系统
### 37-1. dumpe2fs
dump ext2, ext3, ext4 filesystem infomation
#### 37-1-1. 查看`/dev/vda1`的文件系统信息
```
sudo dumpe2fs /dev/vda1
```

## 38. iptables
Administration tool for ipv4 and ipv6 packet filtering and NAT.
### 38-1. options
1. `-L`:列出指定链的所有规则(默认链为filter)
2. `-n`:数字格式输出IP和PORT.不设置该参数的话,`127.0.0.1`会显示成`localhost`,`0.0.0.0`会显示成`anywhere`等
3. `-t`:指定表的类型
4. `-V`:版本号
5. `-F, --flush [chains]`:清除所有规则(默认规则仍保留)
6. `-Z`:清零链的计数器
7. `-j`:如果规则匹配上了,要做什么处理(drop/return/accept)
8. `-A, --append chain rule-specification`:在某个链上添加规则
9. `-D, --delete chain rule-specification`:在某个链上删除规则
10. `-p, --protocol potocol`:检查什么协议的数据包(tcp,  udp,  udplite,  icmp,  icmpv6,esp, ah, sctp, mh)
### 38-2. 显示iptables的filter和NAT表的规则
```
sudo iptables -L -n # filter
sudo iptables -L -n -t nat # nat
```
我的阿里云的情况是
```
Chain INPUT (policy ACCEPT)
target     prot opt source               destination         
ACCEPT     all  --  0.0.0.0/0            0.0.0.0/0            state RELATED,ESTABLISHED
ACCEPT     icmp --  0.0.0.0/0            0.0.0.0/0           
ACCEPT     all  --  0.0.0.0/0            0.0.0.0/0           
ACCEPT     tcp  --  0.0.0.0/0            0.0.0.0/0            state NEW tcp dpt:22
REJECT     all  --  0.0.0.0/0            0.0.0.0/0            reject-with icmp-host-prohibited
DROP       tcp  --  0.0.0.0/0            0.0.0.0/0            tcp dpt:22
DROP       tcp  --  0.0.0.0/0            0.0.0.0/0            tcp dpt:22

Chain FORWARD (policy ACCEPT)
target     prot opt source               destination         
REJECT     all  --  0.0.0.0/0            0.0.0.0/0            reject-with icmp-host-prohibited

Chain OUTPUT (policy ACCEPT)
target     prot opt source               destination         
```
### 38-3. 启动iptables服务
1. 由于CentOS7的默认防火墙是firewalld,而不是iptables,所以要手动安装systemctl能管理的iptables版本
2. `systemctl`式启动服务
```
sudo yum install iptables-services -y
sudo systemctl start iptables
```
#### 38-3-1. 问题:启动不了iptables
##### 情况1
虚拟机的话,就是在SetUp界面处的firewall configuration里enabled防火墙
##### 情况2
没有加载内核模块.iptables是基于内核的包过滤.需要的包通过`lsmod | egrep "nat|filter|ipt"`可以看到.如果没有足够的模块的话,需要加载这些模块
```
modprobe ip_tables
modprobe iptable_filter
modprobe iptable_nat
modprobe ip_conntrack
modprobe ip_conntrack_ftp
modprobe ip_nat_ftp
modprobe ipt_state
```
### 38-4. 添加/删除规则
#### 38-4-1. 服务器不允许其他主机访问她的22端口
+ 给iptables的什么表的什么链添加/删除什么规则
+ 规则:根据什么采取什么处理
+ 处理:drop/return/accept
+ 拒绝服务=>drop
```
iptables -t filter -A INPUT -p tcp --dport 22 -j DROP # 如果是删除的话,-A=>-D
```
如果要删除该规则的话,直接`iptables --flush`就行了



## 0. 实战
### 0-1. 找到/etc/passwd下的shell出现次数
```
cat /etc/passwd | awk -F: '$7!=""{print $7}' | sort | uniq -c
```
