'''
    Study for struct module
'''
import struct

# !:网络字节顺序解析 H:unsigned short; 5s:(abbr for:sssss) 5 chars (must be type of "bytes"); b:signed char
send_request=struct.pack("!H5sb5sb",1,"1.txt".encode('utf-8'),0,"octet".encode('utf-8'),0)


# get op
# the return value of unpack is a tuple
op=struct.unpack("!H",send_request[:2])
print(op)

# get filename
(filename,)=struct.unpack("!5s",send_request[2:7])
print(filename.decode('utf-8'))


