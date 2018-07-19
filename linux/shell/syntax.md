## 1. `${}`,`##`,`%%`
### 1-1. 规则
1. `${}`可以用来指定变量名的范围.如`${dir}_flag`指的是$dir这个变量
2. `5:5`指的是从第5个位置开始的前5个 
3. `/`或`.`都可以替换成其他字符,如`,`等
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
定义时不用写参数.调用时不用写括号.调用时参数用空格分开
#### 原理
```
#! /bin/sh

f()
{
	echo 'function starts'	
	echo "$0"
	echo "$1"
	echo "$2"
	echo "$3"
	echo 'function ends'
}

f a b c
```
#### 创建文件夹,如果存在就不创建
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
	fi

done
exit 0
```

## 3. 循环
### 3-1. for
```
#! /bin/sh

for GIRL in saber serena
do
	echo "I like $GIRL"
done
```
### 3-2. while
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
