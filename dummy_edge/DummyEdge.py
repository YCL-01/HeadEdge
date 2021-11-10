import os
import socket 
import random
import time
from multiprocessing import Process

def tcp_client_car(cam_id, serv_ip, serv_port):
    while True:    
        s = cam_id + " " + str(random.randint(0,4))
        print(s)
        #소켓 생성
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
        #연결
        sock.connect((serv_ip, serv_port)) 
        #전송
        sock.send(s.encode())
        #100ms 쉼
        time.sleep(0.01)

def tcp_client_ped(ped_id, serv_ip, serv_port):
    while True:    
        s = ped_id + " " + str(random.randint(0,4))
        print(s)
        #소켓 생성
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
        #연결
        sock.connect((serv_ip, serv_port)) 
        #전송
        sock.send(s.encode())
        #100ms 쉼
        time.sleep(0.01)

if __name__ == '__main__':

    serv_ip = "192.168.0.10"
    serv_port = 2555
    
    #프로세스 생성
    p1 = Process(target=tcp_client_car, args=('1', serv_ip, serv_port))
    p2 = Process(target=tcp_client_car, args=('2', serv_ip, serv_port))
    p3 = Process(target=tcp_client_ped, args=('3', serv_ip, serv_port))
    p4 = Process(target=tcp_client_ped, args=('4', serv_ip, serv_port))

    #프로세스 시작
    p1.start()
    p2.start()
    p3.start()
    p4.start()
    
    time.sleep(1)
    
    p1.join()
    p2.join()
    p3.join()
    p4.join()
