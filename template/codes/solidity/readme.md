## Solidity
默认都使用`Current version:0.4.8+commit.60cc1668.Emscripten.clang`
### HelloWorld
```
pragma solidity ^0.4.0;

contract HelloWorld {
    event Print(string out);
    function HelloWorld1() {
        Print("Hello World!");
    }
}
```

### uint
```
pragma solidity ^0.4.0;

contract IntExample {
    uint value;

    function add(uint x, uint y) returns (uint) {
        value = x + y;
        return value;
    }


    function divide() returns (uint) {
        uint x = 3;
        uint y = 2;
        value = x / y;
        return value;
    }
}
```
### account
```
pragma solidity ^0.4.0;

contract AddressExample {
    bool isSuccess;

    function sendEth(address account, uint amount) returns (bool) {
        if (account.balance >= amount) {
            isSuccess = account.send(amount);
        } else {
            isSuccess = false;
        }
        return isSuccess;
    }
}

```


### string
string表示动态长度的UTF-8编码的字符串数据.bytes表示动态长度的字节数据
```
pragma solidity ^0.4.0;

contract StringExample {
    string name;

    function stringTest() {
        name = "Jack";
    }
}

```


### array

```
pragma solidity ^0.4.0;

contract StringExample {
    uint[] uintArray;
    bytes b;
    
    function arrayPush() returns (uint){
        uint[3] memory a = [uint(1), 2, 3];
        uintArray = a;
        
        return uintArray.push(4);
    }
    
    function bytesPush() returns (uint) {
    
        b = new bytes(3);
        return b.push(4);
    }
}

```

### mapping
```
pragma solidity ^0.4.0;

contract MappingExample {
    mapping(address => uint) public balances;
    uint balance;

    function arrayPush(uint amount) returns (uint){
        balances[msg.sender] = amount;

        balance = balances[msg.sender];
        return balance;
    }   
        
}       

```

### type
默认存储位置(关键词可覆盖): 
1. memory: 函数参数, 返回的参数
2. storage: 全局状态变量, 局部变量
```
pragma solidity ^0.4.0;

contract TypeExample {
    uint uintValue;
    bool boolValuel;
    address addressValue;
    bytes32 bytes32Value;
    uint[] arrayValue;
    string stringValue;
    enum Direction{Left, Right} // no semicolon
    mapping(address => uint) mappingValue;
}   

```
### function
1. 调用内部函数(相同合约内)
```
pragma solidity ^0.4.0;

contract FunctionExample {
    function g(uint a) returns(uint) {
        return f(a);
    }
    function f(uint a) returns(uint) {
        return a;
    }
}

```
2. 调用外部函数(不同合约)
```
pragma solidity ^0.4.0;

contract User {
    function age() returns(uint) {
        return 25; 
    }
}


contract FunctionExample{
    User user;

    function FunctionExample() {
        address addr = new User();
        user = User(addr);
    }
    function callUser() {

        user.age();
    }

}

```
3. 指定参数名字调用
```
pragma solidity ^0.4.0;

contract FunctionExample{
    function add(uint first, uint second) returns (uint){
        return first + second;
    }   
    function callUser() returns (uint){
    
        return add({second:2, first: 1});
    }   
    
}

```
#### 函数可见性
定义在参数列表和返回关键字中间
```
function func(uint a) private returns (uint b) {return a + 1;}
```
1. external: `this.f()`调用
2. public: 访问权限最大
3. internal: 当前合约内或继承的合约内可以使用
4. private: 只有当前合约内可以使用

#### 常函数
1. 不会消耗gas
2. 不会改变区块链状态
```
pragma solidity ^0.4.0;

contract FunctionExample{
    uint a;
    
    function getA() constant returns (uint){
        return a;
    }
    
}

```

### event
```
pragma solidity ^0.4.0;

contract EventExample{
    event SendBalance(
        address indexed _from,
        bytes32 indexed _id,
        uint _value
    );  
    function sendBalance(bytes32 _id) {
        SendBalance(msg.sender, _id, msg.value);
    }
}
```

### special variables
#### 货币单位:wei(默认), finney, ether
使用: 作为数字的后缀,可以互相比较
```
pragma solidity ^0.4.0;

contract EtherExample{
    function isEqual() returns (bool) {
        if (2 ether == 2000 finney) {
            return true;
        }
        return false;
    }   
}   

```
#### 时间单位: now, days
```
pragma solidity ^0.4.0;

contract TimeExample{
    function f(uint start, uint daysAfter){
        if (now >= start + daysAfter * 1 days) {
            //...
        }
    }   
}   

```
#### 区块全局属性
block.xxx

msg.xxx

now(uint)

tx.xx





