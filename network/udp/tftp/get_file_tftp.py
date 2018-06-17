'''
    This program is used to download {target_filename} from the {src_ip} to the {curr_ip} using TFTP

'''
import struct 
import socket
import sys

if len(sys.argv) != 2:
    print('-'*30)
    print('tips:')
    print('python xxx.py 10.189.xxx.xxx\t(notes:ip that you will download from)')
    print('-'*30)
    exit()
else:
    src_ip=sys.argv[1]

target_filename="tftp_format.png" # "15s" should be altered
dest_filename="dest.png" # the downloaded filename
curr_ip='10.189.190.100'
#src_ip='10.189.190.100'

s=socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

s.bind((curr_ip,2333))

send_request=struct.pack("!H15sb5sb",1,target_filename.encode('utf-8'),0,"octet".encode('utf-8'),0)


s.sendto(send_request,(src_ip,69))

with open(dest_filename,"wb") as f:

    while True:
        # get response from the server
        data_addr=s.recvfrom(1024)

        # get src_port and data(bytes)
        recv_addr=data_addr[1]
        data=data_addr[0]

        # get op
        (op,)=struct.unpack("!H",data[:2])
        # get block_num
        (block_num,)=struct.unpack("!H",data[2:4])
        
        if op == 3: # data
            f.write(data[4:])

            # send ACK
            ack=struct.pack("!HH",4,block_num)
            s.sendto(ack,recv_addr) # ! the port of it is not 69


            if len(data) < 516:
                print("finished!")
                break

        elif op == 5:
            print("ERROR:Block#%d"%(block_num))
            break
            
        #print("data",data)
        #print("op",op)
        #print("block_num",block_num)
        

s.close()
