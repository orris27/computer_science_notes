## 1. Parameter Expansion
`man bash`可以得到所有信息
### 1-1. 规则
orris="I am a good boy"
1. `${}`可以用来指定变量名的范围.如`${dir}_flag`指的是$dir这个变量
2. `5:5`指的是从第5个位置开始的前5个 
3. `/`或`.`都可以替换成其他字符,如`,`等
4. `${#xxx}`:获取xxx变量的字符长度
5. `${xxx:2}`:从第2字符开始取到最后,`${string:position}`
+ `${xxx:2:2}`:从第2字符开始取2个(`cut -c`也可以).`${string:position:length}`
6. `${string/substring/replace}`:对字符串里第一个匹配substring的内容替换为replace
+ `${string/#substring/replace}`:表示从左到右第一个前缀(substring后面有字符)是substring的字符串,把这个前缀进行修改
- `${orris/#boy/yes}`就不会替换,因为第一个boy后面没值
+ `${string/%substring/replace}`:表示从左到右第一个后缀(substring前面有字符)是substring的字符串,把这个前缀进行修改
- `${orris/%I/yes}`就不会替换,因为第一个I前面没值
- `${file/%jpg/png}`就能将后缀jpg替换为png了
7. `${var1:-string}`:不指定变量的话(变量没定义/变量为空字符串),就给默认值.如果`var1`这个变量有值,那么就返回`$var1`;如果没有值的话,就返回string本身.相当于`get(,default=)`
+ `${var1-string}`=`${var1:-string}`
- httpd的一个脚本里就有`pidfile=${PIDFILE-/var/run/httpd/httpd.pid}`
+ `${var1:=string}`:变量未定义/为空字符串,就给默认值并赋值给var1
```
file=/dir1/dir2/dir3/my.file.txt
${file#*/}：删掉第一个 / 及其左边的字符串：dir1/dir2/dir3/my.file.txt
${file##*/}：删掉最后一个 /  及其左边的字符串：my.file.txt
${file#*.}：删掉第一个 .  及其左边的字符串：file.txt
${file##*.}：删掉最后一个 .  及其左边的字符串：txt
${file%/*}：删掉最后一个  /  及其右边的字符串：/dir1/dir2/dir3
${file%%/*}：删掉第一个 /  及其右边的字符串：(空值)
${file%.*}：删掉最后一个  .  及其右边的字符串：/dir1/dir2/dir3/my.file
${file%%.*}：删掉第一个  .   及其右边的字符串：/dir1/dir2/dir3/my
${file:0:5}：提取最左边的 5 个字节：/dir1
${file:5:5}：提取第 5 个字节右边的连续5个字节：/dir2
```
#### shell中获取文件名也可以用basename命令.只需要文件路径一个参数就可以了
第二个参数可以跟它的后缀名,从而把后缀名也能剔除掉
```
basename include/stdio.h .h     -> "stdio"
```
#### 将当前目录下的所有文件的后缀名改成.bak
1. 遍历当前文件夹用\*.\*表示所有文件. 
2. 获取当前文件,用for循环 
3. 解析字符串尤其是文件夹,用${}的语法
```
#! /bin/sh

for i in *.*
do
	mv $i ${i%.*}."$1"
done
```
#### 例子
${}里面的变量不用$来获取
```
file=/dir1/dir2/dir3/my.file.txt
echo ${file#*/}
```

## 2. 函数
定义时不用写参数,参数用`$1`,`$2`,..的方式使用,返回用`return 0`等.调用时不用写括号.调用时参数用空格分开.
+ shell中函数的定义一定要写在调用前
#### 原理

```
#! /bin/sh

f() # function f() 也可以
{
	echo 'function starts'	
	echo "$0"
	echo "$1"
	echo "$2"
	echo "$3"
	echo 'function ends'
}

f a b cdeng
```
#### 模块化
和高级语言的思路一样,将函数统一写在一个脚本文件里,然后在主脚本文件里用`source xx.sh`来调用里面的函数
+ `source`和`.`可以将里面的局部变量和函数导出到当前环境
+ `/etc/functions`就是这样使用的
具体可看 
> https://github.com/orris27/orris/blob/master/linux/shell/command.md

#### 例子
###### 创建文件夹,如果存在就不创建
1. `:`表示空语句,相当于pass
2. if后面跟的是函数的返回值的话.函数返回0,对于if来说为真;函数返回1,对于if来说为假,执行else.所以和普通程序不一样.而且函数不用加`[]`
3. `mkdir "$DIR" > /dev/null 2>&1 `:0-stdin,1-stdout,2-stderr.第一个>把stdout转移到/dev/null,第二个>把stderr转移到1的位置,因为1在/dev/null,所以stderr也到了/dev/null
```
#! /bin/sh

is_dir()
{
	DIR="$1"
	if [ -d "$DIR" ]
	then
		return 0
	else
		return 1
	fi
}

for DIR in "$@"
do
	if is_dir "$DIR" 
	then :
	else
		echo "$DIR doesn't exists.Creating this directory..."
		mkdir "$DIR" > /dev/null 2>&1 
		if [ "$?" -ne 0 ]
		then
			echo "Cannot create the directory"
			exit 1
		fi
	fideng

done
exit 0
```

## 3. 循环
### 3-1. for
#### 使用
1. python写法
```
#! /bin/sh

for GIRL in saber serena
do
	echo "I like $GIRL"
done
```
2. C写法
```
#! /bin/sh

sum=0
for ((i=0;i<=100;i++))
do
	((sum=sum+i))
done
echo $sum
```
### 3-2. while
可以用来设计定时任务
+ 后台运行
+ 开机自启动(`/etc/rc.local`)
+ 分钟级的定时任务推荐`crontab`
#### 管道
根据上一次结果的输出,对每一行输出进行操作
##### 根据输出创建文件
```
echo -e "123\n456\n789" | while read line;do touch $line;done
```
#### 输错密码五次就退出
```
#! /bin/sh

echo 'Please enter the password'
read PWD1
COUNT=0
while [ "$PWD1" != 'saber' -a "$COUNT" -lt 5 ]
do
	echo 'Sorry, please try again'
	COUNT=$(($COUNT+1))
	read PWD1

done
```
#### 读取文件每一行处理
> https://github.com/orris27/orris/blob/master/linux/shell/command.md

### 3-3. until
比while就是先做,其他都一样
```
until <condition>
do
	<statements>
done
```
### 3-4. break, continue
`break`和`continue`可以用`break n`,`continue n`的形式,表示跳出几层,很少用




## 4. 双引号 vs 单引号
### 4-1. 双引号
包住变量,保证空变量展开时的安全.并且里面的值全部保留字面意思(除了$),也可以使用`${var}`
### 4-2. 单引号
只要是在单引号里的,任何含有特殊意义的词都只保留字面意思,如`\`和`\n`
```
echo '\nhello'
echo "\nhello"
DATE=$(date)
echo '$DATE'
echo "$DATE"
```

## 5. 注释
### 使用`#`进行单行注释
```
echo 1 2 # I am a comment
```

## 6. 条件判断
### 6-1. `if`, `then`, `elif`, `else`, `fi`
#### 如何判断上个shell语句为真(exit status为9)呢
`echo $?`可以用来查看exit status,如果为0,说明为真,否则为假
```
[ -z "$VAR" ]
echo $?
```
#### 判断数字范围
1. `read NUM`为等待用户输入,从而赋值给NUM
2. `read`后面的变量不要跟`$`
3. `read`的结束符是回车,所以也包括空格
4. 提示符可以用`-p`命令
5. `NUM`不要写成`SUM`
6. 这里的`exit 1`和`exit 0`只是看看用的.实际比如说请输入yes或no,结果用户输入别的字符串时,可以用这个在条件语句里返回1,表示错误的退出
7. `else`不用写`then`
8. 结束时一定要写上fi.因为shell没有大括号,也不按缩进来算
9. `if [ ];then`的写法也比较常见,这样执行的内容可以单独列出来
```
#! /bin/sh

read NUM
if [ "$NUM" -gt 5 ]
	then echo 'number > 5' 
elif [ "$NUM" -lt 3 ] # <=写成le
	then echo 'number < 3'
elif [ "$NUM" -eq 4 ]
	then echo 'number = 4'
else
	echo 'number = 5 or nunber = 3'
	exit 1
fi
exit 0
```
### 6-2. case 
#### 6-2-1. `case`, `in`, `)`, `;;`, `esac`
##### yes or no
从上往下读,所以*)不能放在yes..这些上面
```
echo 'is it morning or not'
read YES_OR_NO
case "$YES_OR_NO" in
yes|YES|y|Yes)
	echo 'Good Morning';;
[nN]*)
	echo 'Good Afternoon';;
*)
	echo 'Sorry'
	exit 1;;
esac
exit 0
```

## 7. 转义字符
去除本来具有特殊意义的字符的含义,转换为字面意义
### 7-1. `-`
#### 创建文件名为`-hello`的文件
1. `touch -- -hello`
2. `touch ./-hello`
3. ~~touch \-hello~~
4. ~~touch -hello~~
### 7-2. Shell中常见用法
```
touch "\$DATE"
touch \$\ \$
```

## 8. `&&`, `||`
### 8-1. `&&`
`a&&b`表示先执行a,如果a执行成功就执行b
#### 编译安装
```
make && make install
```
### 8-2. `||`
`a || b`表示先执行a,如果a执行失败就执行b
#### 报错
```
[ -f a.log ] || echo "not exist"
```

## 9. 条件测试
### 9-1. 命令
1. `test`
2. `[  ]`:中括号两边一定要有空格
3. `[[  ]]`:不太了解...
4. `(())`:可以用`((i<400))`判断内容是否小于400
### 9-2. 条件
`man test`可以查看
#### 9-2-1. 算术OP
`-eq`, `-ne`, `-lt`, `-le`, `-gt`, `-ge`或者用`(())`来判断
##### 钱一直减少,减到极限为止
```
#! /bin/sh

sum=1000
limit=50

while ((sum>limit))
do
    echo "current:$sum"
    ((sum=sum-50))
done

echo "no money"
```
#### 9-2-2. 普通OP
```
[ -d DIR ] # 如果DIR存在并且是一个目录,为真
[ -f FILE ] #　如果FILE存在并且为一个普通文件,为真
[ -x FILE ] # 如果FILE存在并且可执行,为真
[ -s FILE ] # 如果FILE存在并且大小不为0,为真
[ -z STRING ] # 如果STRING的字符串长度为0,为真(如果[ -z ]的话,那么也是真)
[ -n STRING ] # 如果STRING的字符串长度不为0,为真
[ STRING1 = STRING2 ] # 字符串相同,为真
[ STRING1 != STRING2 ] # 字符串不相同,为假
[ ARG1 OP ARG2 ] # ARG为只含有整数的字符串,op为操作符
# 逻辑运算
[ ! EXPR ] # 非
[ EXPR1 -a EXPR2 ] # 与
[ EXPR1 -o EXPR2 ] # 或
```
### 9-3. 返回结果
返回的exit status,为0表示正常退出,也就是说上面的条件测试结果为真
#### 检测上面的语句是否成立
```
echo $?
```
### 9-4. 应用
1. 检测Web,MySQL是否正常运行
+ 端口是否存在
+ 进程中是否有(注意如果脚本文件名包含过滤的字符,那么grep后也会有脚本的进程)
+ 模拟用户登录(最真实)
- shell
- python/java
+ curl/wget(Web)


## 10.变量
### 10-1. 环境变量
#### 10-1-1. 定义
等号不要有空格
```
export dinner=apple
```

#### 10-1-2. 定义环境变量的文件
#### /etc/bashrc
会执行`/etc/profile.d`下的所有脚本文件

##### /etc/profile
设置对**所有用户**生效的环境变量的脚本文件.会执行`/etc/profile.d`下的所有脚本文件.当用户登录时,它就会被执行

##### ~/.bashrc
设置**对应用户**的环境变量的shell脚本的脚本文件.该用户登录时,仅仅执行一次
+ Anaconda就备份了一个原始的~/.bashrc文件,在新的~/.bashrc里面为PATH变量添加了自己的位置

##### ~/.bash_profile
`~/.bash_profile`会先自动调用`~/.bashrc`,`~/.bash_profile`多定义环境变量,而`~/.bashrc`主要定义alias


#### 10-1-3. 特殊环境变量
##### 1. LANG
Linux系统的默认字符集.我们的是en_US.utf-8.如果调成了中文的,一些命令什么的都会变成中文,不好
+ shell中输入`locale`可以看到语言,如果改变LANG变量,这些都会变化

##### 2. PATH
只有在PATH变量里出现过的目录下的程序,才能在shell里直接使用,PATH是用冒号分隔的.如果要执行某个命令,要么放到环境变量,要么指明其全路径.如果将chattr这样的程序改名字,那么对应地要使用该功能就要用新的名字
+ 只要是可执行文件,如sh文件,就能够作为bash的命令来使用
###### 添加某个命令
1. 对所有用户生效
```
echo 'export PATH=$PATH:/home/orris/dir1' >> /etc/profile # 也可以是`~/.bashrc` 将PATH环境变量写入到profile文件里,注意写进去的是$PATH这个原本的字符串,因为/etc/profile本身是个脚本文件,所以他会执行这段命令(我们输入的是命令)
source /etc/profile # 执行该文件
```
2. 只对当前用户生效
```
echo 'export PATH=$PATH:/home/orris/tmp2' >> ~/.bashrc # 重启后PATH也会改变
```

##### 3. PWD, OLDPWD, HOME
这些环境变量都会传递给python程序的open,shell的cd等等

##### 4. HISTCONTROL
###### 设置隐藏记录
如果在命令前敲空格,就会隐藏其记录
```
HISTCONTROL=ignorespace
echo 123
 echo 12321
```
#### 10-1-3. 查看环境变量
1. `env`
2. `printenv`
3. 查看dinner是不是环境变量
```
env | grep dinner
env | grep 'dinner'
```
#### 10-1-4. 删除
```
unset dinner
```

### 10-2. 局部变量
作用域在子进程(一个shell脚本)里的变量
#### 10-2-1. 定义
字符串!所以dinner=123后,如果$dinner+3,结果为"123+3"
1. `dinner="apple"`
2. `dinner=apple`
#### 10-2-2. 转换成环境变量
1. `export dinner`
2. `declare -x dinner`:不推荐

#### 10-2-3. 查看局部变量
```
set | grep dinner
```
#### 10-2-4. 删除
```
unset dinner
```

### 10-3. 特殊变量
真正的参数其实是在"./1.sh"后面的字符串.因此参数列表, 参数个数, shift都是针对$1, $2, ....
1. `$0`  相当于C语言main函数的argv[0],获取执行我这个当前脚本的文件名和路径
2. `$1`、`$2`,...,`${10}`,`${11}`,...    这些称为位置参数（Positional Parameter），相当于C语言main函数的argv[1]、argv[2]...
+ 数字的话只认第一个,所以如果$1是"abc"的话,那么$10是"abc0",只有${10}才是第10个参数
3. `$#`  命令行参数个数.相当于C语言main函数的argc-1，注意这里的#后面不表示注释
4. `$@`  表示参数列表"$1" "$2" ...，例如可以用在for循环中的in后面
5. `$*`  表示参数列表作为整体的一个字符串,即"$1 $2 ... $n".和`$@`的区别在for循环里只在加双引号时才会显示出来
6. `$?`  上一条命令的Exit Status
7. `$$`  当前进程号.可以用来输出到`pid`文件中


### 10-5. 大小写
没有严格标准.不过可以环境变量大写,而局部变量小写

## 11. 数学计算
> https://github.com/orris27/orris/blob/master/linux/shell/command.md

## 12. bash内建命令
bash内部定义了的程序/命令.
+ which不到
- pwd,echo都有两个版本,即`echo`和`/bin/echo`,前者为bash内建命令,而后者为外部命令
- 如果which到了,说明有外部命令版本,但也可能存在bash内建命令的版本
- shell会优先找bash内建命令,找不到才会去找外部命令
+ 然后man的话会走到bash里的变量,如`echo`,`eval`,`exec`,`read`,`set`等
+ bash内建变量/命令效率高于外部命令,比如用`${#xx}`就优于`wc -L`
- 外部命令通常功能强大,需要单独创建一个子进程,而内部命令则不需要
### 查看某个命令是不是内建命令
```
type nc
type pwd
type cd
```

## 12. 通配符
1. `*` 
2. `?` 
3. `[]`
### 例子
`[abcdt]?.sh` => `t1.sh`

## 13. 后台
如果脚本执行时间很长,那么可以考虑后台运行
> https://github.com/orris27/orris/blob/master/linux/shell/command.md

## 14. 随机数
### 14-1. RANDOM
Each time this parameter is referenced, a random integer between 0 and 32767 is  generated.(见`man bash`)
+ 使用`echo $RANDOM`就可以了
#### 生成随机的0为字符串
先生成随机数,然后用hash算法加密,最后随便从加密后的字符串里取8位就可以了
+ `md5sum`可以做hash计算
```
echo $RANDOM | md5sum | cut -c 1-8
```

### 14-2. openssl
```
openssl rand -base64 8
```

### 14-3. date
```
date +%s%N
```

### 14-4. urandom
```
head /dev/urandom | cksum
```
### 14-5. uuid
```
cat /proc/sys/kernel/random/uuid
```

### 14-6. mkpasswd
```
# sudo yum install expect -y
mkpasswd -l 8 # 8是位数
```

## 15. 数组
### 15-1. 定义
1. 写法1
```
array=(orris 100 hello)
```
2. 写法2
```
array=(
 orris
 100
 hello
)
```
### 15-2. 获取
#### 15-2-1. 整体
```
echo ${array[*]}
echo ${array[@]}
```

#### 15-2-2. 元素
```
echo ${array[2]}
```

#### 15-2-3. 长度
```
echo ${#array[*]}
echo ${#array[@]}
```

### 15-3. 增加
#### 15-3-1. 元素
```
array[3]=new
```

### 15-4. 删除
#### 15-4-1. 元素
```
unset array[2]
```
#### 15-4-2. 整体
```
unset array
```
### 15-5. 遍历
```
total=`echo ${#array[*]}` 
for i in `seq 0 $((total-1))`
do 
	echo ${array[$i]}
done
```
### 15-6. 应用
1. 在数组定义里执行命令,如`array=($(ls))`可以获取当前目录下的所有文件名
