import socket
import threading
import time
from user import User
    
def tcplink(sock,addr):
    strlog = 'Accept new connection from %s:%s...' % addr
    writeToLog(strlog)
    sock.send('begin')
    try:
        while True:
            data = sock.recv(1024)
            time.sleep(1)
            if data:
                strlog = "recv from %s:%s :" % addr
                strlog += data
                writeToLog(strlog)
            if data.startswith("wset:"):
                ret = User.SetWeather(data)
                print ret
            elif data == 'exit' or not data:
                break
            sock.send('Recved')
        sock.close()
        strlog =  'Connection from %s:%s closed.' % addr
        writeToLog(strlog)
    except Exception as e:
        print 'Error occur ' + str(e)


def writeToLog(content):
    try:
        f = open(r"weatherlog.txt", "a")
        f.write(time.strftime("(%Y-%m-%d %H:%M:%S)", time.localtime(time.time() + 13 * 60 * 60)))
        f.write(content + "\n")
    except IOError as err:
        print ('File Error:' + str(err))
    finally:
        if f:
            f.close()

if __name__ == '__main__':
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('123.123.123.123', 9999))  
    s.listen(5)
    print 'Waiting for connection'
    while True:
        sock, addr = s.accept()
        t = threading.Thread(target=tcplink, args=(sock, addr))
        t.start()