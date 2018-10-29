## 数据组织
1. String
```
/** The value is used for character storage. */
private final char value[];

/** Cache the hash code for the string */
private int hash; // Default to 0

/** use serialVersionUID from JDK 1.0.2 for interoperability */
private static final long serialVersionUID = -6849794470754667710L;
```
String的主要成员是字符数组常量value、整形变量hash和长整型的serialVersionUID。Java使用String存储字符串的时候，每个字符都是保存在value成员里面的，根据Java数组的length可以得到字符串的常量，从而获得字符串的边界。需要注意的是，String中的value是final定义的，也就是说value成员是常量。也就是说我们不能修改value的值。虽然我们平时使用的`+=`乍看之下是直接在字符数组的后面添加新的字符，但实际在String中的实现并不是简单的写在后面。

String的value成员使用字符数组初始化时的代码是`this.value = Arrays.copyOf(value, value.length);`，利用了Arrays.copyOf方法，将参数的所有字符都拷贝到value成员中，从而实现初始化。


2. StringBuffer

在StringBuffer中，如果我们直接查看StringBuffer类的实现，我们只能看到serialVersionUID的成员，并不能像String一样看到value成员。所以我们可以推理，StringBuffer的value成员应该是继承过来的。
```
private static final long serialVersionUID = 3388685877147921107L;
```
查看StringBuffer的继承关系`public final class StringBuffer extends AbstractStringBuilder implements Appendable, Serializable, CharSequence`，可以看到StringBuffer是继承于AbstractStringBuilder的。查看AbstractStringBuilder的代码。可以看到下面的代码实现。

INITIAL_CAPACITY 表示初始化时value数组默认的长度

value 类似于String的value，也是字符数组，只是没有final定义

count int类型，记录字符串的长度

shared 布尔类型
```
static final int INITIAL_CAPACITY = 16;
private char[] value;
private int count;
private boolean shared;
```
首先，StringBuffer的value也是字符数组，但是没有final定义，也就是说StringBuffer的value是可变的，我们可以修改value的值。并且StringBuffer里多出了INITIAL_CAPACITY和count等成员，前者说明了StringBuffer默认创建的是16字节长度的value数组，而String的话默认是空字符串或根据构造函数的参数来决定自己的字符串长度，而StringBuffer则不然，StringBuffer在构造函数的参数的字符长度少于16时仍然构件16长度的字符数组，并且根据参数的字符串的大小动态决定自己的value长度。count成员用来标记字符串的长度，因为我们不能通过数组长度来决定字符串的长度，value数组后面的部分可能是无意义的，所以我们需要一个count成员来标记字符串的长度。在StringBuffer实例增加或减少字符的时候，count和value都会改变。


3. StringBuilder

在StringBuilder中，如果我们直接查看StringBuilder类的实现，同样只能看到serialVersionUID的成员。所以我们可以推理，StringBuilder的value成员和StringBuffer一样是继承过来的。
```
private static final long serialVersionUID = 3388685877147921107L;
```
查看StringBuilder的继承关系`public final class StringBuffer extends AbstractStringBuilder implements Appendable, Serializable, CharSequence`，可以看到StringBuilder也是继承于AbstractStringBuilder的。具体的数据组织和StringBuffer一样，不赘述。


## 功能实现

### 1. String

String类的主要方法有length、isEmpty、getChars、startsWith等，这里介绍部分功能的实现。

1. 构造函数
    1. 默认构造函数：构造一个没有元素的字符数组表示空的字符串
    ```
    public String() {
        this.value = new char[0];
    }
    ```
    2. 基于字符数组构造：利用Arrays.copyOf拷贝字符数组的每个字节到value成员中
    ```
    public String(char value[]) {
        this.value = Arrays.copyOf(value, value.length);
    }
    ```
    
2. length：通过value成员作为字符数组时的长度来判断。换句话说，String中的value所有元素都是有效的，比如“apple”使用String存储的话，value一定就是刚好“apple”5个元素，并且不包含其他多余的元素。所以可以直接通过字符数组的长度来判断字符串的长度
```
public int length() {
    return value.length;
}
```

3. isEmpty：通过value成员是否为空数组来确定这个字符串是否为空
```
public boolean isEmpty() {
    return value.length == 0;
}
```


4. equals：String类型的equals函数要求比较严格。如果要返回true的话，必须参数是一个String对象，并且字符长度以及字符数组value完全相同才行，否则就不能成立。当然，如果对象是“hello”这样的表达形式的话，也是可以返回true的，因为可以类型转换到String。
```
public boolean equals(Object anObject) {
    if (this == anObject) {
        return true;
    }
    if (anObject instanceof String) {
        String anotherString = (String) anObject;
        int n = value.length;
        if (n == anotherString.value.length) {
            char v1[] = value;
            char v2[] = anotherString.value;
            int i = 0;
            while (n-- != 0) {
                if (v1[i] != v2[i])
                        return false;
                i++;
            }
            return true;
        }
    }
    return false;
}
```


5. contentEquals：比较字符串内容是否相等。equals要求不仅内容相同，而且类型必须是String类型，但是contentEquals不那么严格，主要比较字符创内容是否相等。比如我么有一个StringBuffer的内容，那么会将这个实例转换成CharSequence类，然后进一步调用深入的contentEquals方法，类似于equal的比较方法，逐个比较字符是否相等。在StringBuffer的情况下，由于StringBuffer是AbstractStringBuilder的派生类，所以会调用if里面的内容，如果内容都是相同的话，就会返回true
```
public boolean contentEquals(StringBuffer sb) {
    synchronized (sb) {
        return contentEquals((CharSequence) sb);
    }
}
public boolean contentEquals(CharSequence cs) {
    if (value.length != cs.length())
        return false;
    // Argument is a StringBuffer, StringBuilder
    if (cs instanceof AbstractStringBuilder) {
        char v1[] = value;
        char v2[] = ((AbstractStringBuilder) cs).getValue();
        int i = 0;
        int n = value.length;
        while (n-- != 0) {
            if (v1[i] != v2[i])
                return false;
            i++;
        }
        return true;
    }
    // Argument is a String
    if (cs.equals(this))
        return true;
    // Argument is a generic CharSequence
    char v1[] = value;
    int i = 0;
    int n = value.length;
    while (n-- != 0) {
        if (v1[i] != cs.charAt(i))
            return false;
        i++;
    }
    return true;
}
```


6. substring：返回字符串的子串。根据下面的代码可以看到，首先进行越界检查，然后直接返回新创建的String对象
```
public String substring(int beginIndex) {
    if (beginIndex < 0) {
        throw new StringIndexOutOfBoundsException(beginIndex);
    }
    int subLen = value.length - beginIndex;
    if (subLen < 0) {
        throw new StringIndexOutOfBoundsException(subLen);
    }
    return (beginIndex == 0) ? this : new String(value, beginIndex, subLen);
}

```


7. replace：替换字符串的某个字符为另一个字符。由于String类中的value成员是常量，不能直接修改。所以源代码对于String类的replace处理是创建value相同长度的新字符数组，然后将String对象的value值不断赋值进去，并且需要改变的地方进行改变，最后根据这个新字符数组重新构造String对象，并返回这个String对象。
```
public String replace(char oldChar, char newChar) {
    if (oldChar != newChar) {
        int len = value.length;
        int i = -1;
        char[] val = value; /* avoid getfield opcode */

        while (++i < len) {
            if (val[i] == oldChar) {
                break;
            }
        }
        if (i < len) {
            char buf[] = new char[len];
            for (int j = 0; j < i; j++) {
                buf[j] = val[j];
            }
            while (i < len) {
                char c = val[i];
                buf[i] = (c == oldChar) ? newChar : c;
                i++;
            }
            return new String(buf, true);
        }
    }
    return this;
}

```


8. valueOf：构造String对象。如果参数已经第一个对想了，就调用toString方法。如果是字符数组或者单个字符等，都是直接构调用new来进一步调用构造函数来创建新的构造函数，所以创建的字符串仍然是保留在常量池里，如果已经有新的常量了，就会直接使用这个常量。
```
public static String valueOf(Object obj) {
    return (obj == null) ? "null" : obj.toString();
}
public static String valueOf(char data[]) {
    return new String(data);
}

public static String valueOf(char data[], int offset, int count) {
    return new String(data, offset, count);
}
public static String valueOf(char c) {
    char data[] = {c};
    return new String(data, true);
}
//....
```

### 2. StringBuffer
1. 构造函数： 调用AbstractStringBuilder的构造函数进行构造，
```
public StringBuffer() {
}
public StringBuffer(int capacity) {
    super(capacity);
}
```
我们直接看父类的构造函数，可以看到默认构造函数是直接对value成员创建new的字符数组。而如果使用capacity作为参数进行构造函数的话，就不使用INITIAL_CAPACITY长度的字符数组，而是直接使用参数长度的字符数组。
```
AbstractStringBuilder() {
    value = new char[INITIAL_CAPACITY];
}
AbstractStringBuilder(int capacity) {
    if (capacity < 0) {
        throw new NegativeArraySizeException(Integer.toString(capacity));
    }
    value = new char[capacity];
}
```


2. append：给StringBuffer字符串添加新的内容。该方法有很多重载，这里只看Object和String的部分。它们都是通过append0来append进去的。首先判断参数是否为null，如果是的话，就调用appendNULL添加"null"到字符串里面，它的实现是通过动态增大字符串的长度（当然如果字符串的长度已经满足要求的话，就不会重新创建新的字符数组），然后直接对后面的4个字符修改，从而构成"null"。
```
public synchronized StringBuffer append(Object obj) {
    if (obj == null) {
        appendNull();
    } else {
        append0(obj.toString());
    }
    return this;
}

public synchronized StringBuffer append(String string) {
    append0(string);
    return this;
}
```
如果进一步调用append0的话，首先也是按需增大字符串的长度，然后调用`_getChars`来给value有效字符串后面赋予新的字符串。并且StringBuffer的append会直接返回对象，而不会创建新的对象。
```
final void append0(String string) {
    if (string == null) {
        appendNull();
        return;
    }
    int length = string.length();
    int newCount = count + length;
    if (newCount > value.length) {
        enlargeBuffer(newCount);
    }
    string._getChars(0, length, value, count);
    count = newCount;
}
```
需要注意的是，StringBuffer里的append通过synchronized定义，也就是说这个方法是不允许同时被执行的，从而保证线程安全。


### 3. StringBuilder
1. 构造函数：StringBuilder构造函数和StringBuffer的构造函数基本一样，不赘述

2. append：类似于StringBuffer的append函数，也是通过调用父类的append0来实现的，前面的介绍也已经有了。而与StringBuffer不一样的是，StringBuilder的append没有通过synchronized来申明，因此是线程不安全的
```
public StringBuilder append(String str) {
    append0(str);
    return this;
}
```




## 设计原因和影响
总结String、StringBuffer、StringBuilder的几大区别
1. String是不可变字符串，StringBuffer和StringBuilder是可变字符串

2. StringBuffer是线程安全的，StringBuilder是线程不安全的

三个不同类的设计根据各自的功能而不同，并且导致了不同的影响。

String的







## 使用场景
