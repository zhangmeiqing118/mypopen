#!/usr/bin/python
#########################################################################
# @File Name: test.py
# @Author: zhangmeiqing
# @mail: zhangmeiqing_2009@126.com
# @Created Time: Thu 08 Dec 2016 11:42:45 AM CST
#########################################################################
import getpass
import sys
import socket
import os
import struct
import select
import time
import thread
import telnetlib

#host_a = "10.3.56.222"
#passwd_a = "acce.com"
#host_b = "10.3.56.222"
#passwd_b = "acce.com"
#host_c = "10.3.56.222"
#passwd_c = "acce.com"
DEFAULT_PORT = 23
#g_device_list = ['10.3.56.222', '10.3.56.223', '10.3.56.224']
g_device_list = ['10.3.56.222']
g_device_passwd_list = ['acce.com', 'acce.com', 'acce.com']
g_device_link_status_list = {}
g_device_telnet_status = {}
g_device_telnet_list = {}
g_device_telnet_object = {}
g_dict_itf_stat_list = [{}, {}, {}]

#watch stp port list
g_stp_itf_list = ['GigabitEthernet1/0/1']

#display interface brief name type
g_itf_name_list = ["InLoop", "M-GE", "NULL", "REG", "Vlan", "GE", "XGE"]
g_dict_itf_stp_stat_list = [{}, {}, {}]

POLLING_TIME = 2       #2 sec
NORMAL_LINK_SLEEP_TIME = 5
LINK_DOWN_SLEEP_TIME = 3
#specail key
CMD_PROMPT = "<H3C>"
CMD_MORE = "---- More ----"
#define used command

display_interface_brief_cmd = "display interface brief"
display_stp_brief_cmd = "display stp brief"
display_vrrp_cmd = "display vrrp"

#ping macro
ICMP_ECHO_REQUEST = 8 # Platform specific
DEFAULT_TIMEOUT = 2
DEFAULT_COUNT = 4 
 
class Pinger(object):
    """ Pings to a host -- the Pythonic way"""
    
    def __init__(self, target_host, count=DEFAULT_COUNT, timeout=DEFAULT_TIMEOUT):
        self.target_host = target_host
        self.count = count
        self.timeout = timeout

    def do_checksum(self, source_string):
        """  Verify the packet integritity """
        sum = 0
        max_count = (len(source_string)/2)*2
        count = 0
        while count < max_count:
            val = ord(source_string[count + 1])*256 + ord(source_string[count])
            sum = sum + val
            sum = sum & 0xffffffff 
            count = count + 2
     
        if max_count<len(source_string):
            sum = sum + ord(source_string[len(source_string) - 1])
            sum = sum & 0xffffffff 
     
        sum = (sum >> 16)  +  (sum & 0xffff)
        sum = sum + (sum >> 16)
        answer = ~sum
        answer = answer & 0xffff
        answer = answer >> 8 | (answer << 8 & 0xff00)
        return answer


    def receive_ping(self, sock, ID, timeout):
        """
        Receive ping from the socket.
        """
        time_remaining = timeout
        while True:
            start_time = time.time()
            readable = select.select([sock], [], [], time_remaining)
            time_spent = (time.time() - start_time)
            if readable[0] == []: # Timeout
                return
     
            time_received = time.time()
            recv_packet, addr = sock.recvfrom(1024)
            icmp_header = recv_packet[20:28]
            type, code, checksum, packet_ID, sequence = struct.unpack(
                "bbHHh", icmp_header
            )
            if packet_ID == ID:
                bytes_In_double = struct.calcsize("d")
                time_sent = struct.unpack("d", recv_packet[28:28 + bytes_In_double])[0]
                return time_received - time_sent
     
            time_remaining = time_remaining - time_spent
            if time_remaining <= 0:
                return
    def send_ping(self, sock,  ID):
        """
        Send ping to the target host
        """
        target_addr  =  socket.gethostbyname(self.target_host)
     
        my_checksum = 0
     
        # Create a dummy heder with a 0 checksum.
        header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, my_checksum, ID, 1)
        bytes_In_double = struct.calcsize("d")
        data = (192 - bytes_In_double) * "Q"
        data = struct.pack("d", time.time()) + data
     
        # Get the checksum on the data and the dummy header.
        my_checksum = self.do_checksum(header + data)
        header = struct.pack(
            "bbHHh", ICMP_ECHO_REQUEST, 0, socket.htons(my_checksum), ID, 1
        )
        packet = header + data

    def send_ping(self, sock,  ID):
        """
        Send ping to the target host
        """
        target_addr  =  socket.gethostbyname(self.target_host)
     
        my_checksum = 0
     
        # Create a dummy heder with a 0 checksum.
        header = struct.pack("bbHHh", ICMP_ECHO_REQUEST, 0, my_checksum, ID, 1)
        bytes_In_double = struct.calcsize("d")
        data = (192 - bytes_In_double) * "Q"
        data = struct.pack("d", time.time()) + data
     
        # Get the checksum on the data and the dummy header.
        my_checksum = self.do_checksum(header + data)
        header = struct.pack(
            "bbHHh", ICMP_ECHO_REQUEST, 0, socket.htons(my_checksum), ID, 1
        )
        packet = header + data
        sock.sendto(packet, (target_addr, 1)) 


    def ping_once(self):
        """
        Returns the delay (in seconds) or none on timeout.
        """
        icmp = socket.getprotobyname("icmp")
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_RAW, icmp)
        except socket.error, (errno, msg):
            if errno == 1:
                # Not superuser, so operation not permitted
                msg +=  "ICMP messages can only be sent from root user processes"
                raise socket.error(msg)
        except Exception, e:
            print "Exception: %s" %(e)
    
        my_ID = os.getpid() & 0xFFFF
     
        self.send_ping(sock, my_ID)
        delay = self.receive_ping(sock, my_ID, self.timeout)
        sock.close()
        return delay 
        
    def ping(self):        
        """
        Run the ping process
        """
        for i in xrange(self.count):
            #print "Ping to %s..." % self.target_host,
            try:
                delay  =  self.ping_once()
            except socket.gaierror, e:
                #print "Ping failed. (socket error: '%s')" % e[1]
                break
     
            #if delay  ==  None:
            #    print "Ping failed. (timeout within %ssec.)" % self.timeout
            #else:
            if delay  !=  None:
                delay  =  delay * 1000
                #print "Get ping in %0.4fms" % delay
                return 1
        return 0



#pinger = Pinger(target_host=target_host)
#pinger.ping()

def ping_thread(device_list, link_status):
    pinger_list = {}
    nums = len(device_list)
    for idx in range(nums):
        pinger_list[idx] = Pinger(device_list[idx])

    while True:  
        for idx in range(nums):
            recode = pinger_list[idx].ping()
            if 1 == recode :
               link_status[device_list[idx]] = 1

        flag = 0;        
        for idx in range(nums):
            if link_status[device_list[idx]] == 0 :
                flag = 1
                break
        if 1 == flag :
            time.sleep(LINK_DOWN_SLEEP_TIME)
        else :
            time.sleep(NORMAL_LINK_SLEEP_TIME)
    return  


class Process(object):
    """ Pings to a host -- the Pythonic way"""
    
    def __init__(self, target_host, passwd, port=DEFAULT_PORT):
        self.target_host = target_host
        self.passwd = passwd
        self.port = port


    def telnet(self):
        try:
            tn = telnetlib.Telnet(self.target_host, self.port)
            print tn.read_until("Password: ")
            tn.write(self.passwd + "\n")
            print tn.read_until(CMD_PROMPT)
            return tn
        except socket.error, (errno, msg):
            if errno == 1:
                # Not superuser, so operation not permitted
                msg +=  "ICMP messages can only be sent from root user processes"
                raise socket.error(msg)
        except Exception, e:
            print "Exception: %s" %(e)
    

    def display_interface_brief(self, device_tn):
        try :
            result = ""
            device_tn.write(display_interface_brief_cmd + "\n")
            result += device_tn.read_until(CMD_MORE)
            device_tn.write(" ")
            result += device_tn.read_until(CMD_MORE)
            device_tn.write(" ")
            result += device_tn.read_until(CMD_PROMPT)
            return result
        except socket.error, (errno, msg):
            if errno == 1:
                # Not superuser, so operation not permitted
                msg +=  "ICMP messages can only be sent from root user processes"
                raise socket.error(msg)
        except Exception, e:
            print "Exception: %s" %(e)
    def display_stp_brief(self, device_tn):
        try :
            result = ""
            device_tn.write(display_stp_brief_cmd + "\n")
            result = device_tn.read_until(CMD_PROMPT)
            return result
        except socket.error, (errno, msg):
            if errno == 1:
                # Not superuser, so operation not permitted
                msg +=  "ICMP messages can only be sent from root user processes"
                raise socket.error(msg)
        except Exception, e:
            print "Exception: %s" %(e)

    def display_vrrp(self, device_tn):
        try:
            result = ""
            device_tn.write(display_vrrp_cmd + "\n")
            result = device_tn.read_until(CMD_PROMPT)
            return result
        except socket.error, (errno, msg):
            if errno == 1:
                # Not superuser, so operation not permitted
                msg +=  "ICMP messages can only be sent from root user processes"
                raise socket.error(msg)
        except Exception, e:
            print "Exception: %s" %(e)

def process_interface_status(result):
    #index = result.find(interface_brief_start)
    #print result
    tmp_list = result.split('\n')
    line = len(tmp_list)
    dict_stat = {}
    for i in range(line) :
        #print "line:", i, tmp_list[i],
        #print "line:", i
        #continue
        flag = 0;
        itf_name = ''
        index = 0
        itf_type = len(g_itf_name_list)
        tmp_list_len = len(tmp_list[i])
        for j in range(itf_type):
            #if -1 == tmp_list[i].find(g_itf_name_list[j], 0, len(g_itf_name_list[j]) + 1) :
            index = tmp_list[i].find(g_itf_name_list[j])
            if -1 != index :
                flag = 1
                break
        
        if 0 == flag :
            continue
        
        j = index;
        while j < tmp_list_len :
            if ' ' != tmp_list[i][j]:
                j = j + 1
            else :
                break
        itf_name = tmp_list[i][index:j]
        while j < tmp_list_len:
            if ' ' == tmp_list[i][j] :
                j = j + 1
            else :
                break

        itf_status =  tmp_list[i][j:j + 4]
        #print j, tmp_list[i][j:j + 4]
        #print tmp1[0],
        dict_stat[itf_name] = itf_status
        
        #print itf_name, itf_status
    
    return dict_stat


def process_interface_stp(result):
    #index = result.find(interface_brief_start)
    #print result
    tmp_list = result.split('\n')
    line = len(tmp_list)
    dict_stat = {}
    for i in range(line) :
        #print "line:", i, tmp_list[i],
        #print "line:", i
        #continue
        flag = 0
        index = 0
        itf_name = ''
        tmp_list_len = len(tmp_list[i])
        itf_type_num = len(g_stp_itf_list)

        for j in range(itf_type_num):
            index = tmp_list[i].find(g_stp_itf_list[j]) 
            if -1 != index :
                flag = 1
                itf_name = g_stp_itf_list[j]
                break
        
        if 0 == flag :
            continue
        
        
        #print "line ", i, len(tmp_list[i]),
        #print itf_name
        index += len(itf_name) + 1


        #remove space after interface name 
        j = index
        while j < tmp_list_len :
            if ' ' == tmp_list[i][j] :
                j = j + 1
            else :
                break

        #remove characters  after interface name    
        while j < tmp_list_len :
            if ' ' != tmp_list[i][j] :
                j = j + 1
            else :
                break

        #remove space after role name 
        while j < tmp_list_len :
            if ' ' == tmp_list[i][j] :
                j = j + 1
            else :
                break
        index = j

        #count characters  for stp stat
        while j < tmp_list_len :
            if ' ' != tmp_list[i][j] :
                j = j + 1
            else :
                break

        #print "list:", tmp_list[i]
        stp_stat = tmp_list[i][index:j]
        
        dict_stat[itf_name] = stp_stat
        
        #print itf_name, stp_stat
    
    return dict_stat

    

thread.start_new_thread(ping_thread, (g_device_list, g_device_link_status_list))  
 
nums = len(g_device_list)
for idx in range(nums):
    g_device_link_status_list[g_device_list[idx]] = 0 
    g_device_telnet_status[g_device_list[idx]] = 0
    #g_dict_itf_stat_list[idx] = {}
    

times_count = 0
while True :    
    num = len(g_device_list);
    for idx in range(num) :
        if 1 == g_device_link_status_list[g_device_list[idx]] and g_device_telnet_status[g_device_list[idx]] == 0 : 
            g_device_telnet_object[g_device_list[idx]] = Process(g_device_list[idx], g_device_passwd_list[idx])
            g_device_telnet_list[g_device_list[idx]] = g_device_telnet_object[g_device_list[idx]].telnet();
            if None == g_device_telnet_list[g_device_list[idx]] :
                print "telnet failed\n"
                continue
            
            g_device_telnet_status[g_device_list[idx]] = 1
            print "device ", g_device_list[idx], "telnet success"
            
            #init device interface status
            #get device interface status string
            result = g_device_telnet_object[g_device_list[idx]].display_interface_brief(g_device_telnet_list[g_device_list[idx]])
            if None == result :
                print "display interface brief error"
                g_device_telnet_list[g_device_list[idx]].close()
                g_device_telnet_status[g_device_list[idx]] = 0
                continue
            
            #print result
            #process device interface status string
            g_dict_itf_stat_list[idx] = process_interface_status(result)
            #print g_dict_itf_stat_list[idx]

            #init device interface stp stat
            #get device interface stp stat  string
            result = g_device_telnet_object[g_device_list[idx]].display_stp_brief(g_device_telnet_list[g_device_list[idx]])
            if None == result :
                print "display stp brief error"
                g_device_telnet_list[g_device_list[idx]].close()
                del g_dict_itf_stat_list[idx]
                g_device_telnet_status[g_device_list[idx]] = 0
                continue
            #process device interface status string
            g_dict_itf_stp_stat_list[idx] = process_interface_stp(result)

        if 0 == g_device_link_status_list[g_device_list[idx]] and g_device_telnet_status[g_device_list[idx]] == 1 : 
            g_device_telnet_list[g_device_list[idx]].close()
            #clear global stat list
            del g_dict_itf_stat_list[idx]
            del g_dict_itf_stp_stat_list[idx]
            print "device ", g_device_list[idx], "telnet closed"
    
    print "start process"
    time.sleep(POLLING_TIME)
    
    for idx in range(num) :
        if 1 == g_device_link_status_list[g_device_list[idx]] and g_device_telnet_status[g_device_list[idx]] == 1 :
            
            #get device interface status string
            result = g_device_telnet_object[g_device_list[idx]].display_interface_brief(g_device_telnet_list[g_device_list[idx]])
            if None == result :
                print "display interface brief error"
                g_device_telnet_list[g_device_list[idx]].close()
                g_device_telnet_status[g_device_list[idx]] = 0
                continue

            #process device interface status string
            curr_itf_stat_list = process_interface_status(result)
            items_num = len(curr_itf_stat_list);
            for itm_num in range(items_num) :
                item = curr_itf_stat_list.popitem()
                if g_dict_itf_stat_list[idx].has_key(item[0]) :
                    #print g_dict_itf_stat_list[idx].get(item[0])
                    if item[1] != g_dict_itf_stat_list[idx].get(item[0]):
                        print "port ", item[0], "changed from", g_dict_itf_stat_list[idx].get(item[0]), "to", item[1]
                        g_dict_itf_stat_list[idx][item[0]] = item[1]

           #get device interface stp stat string
            result = g_device_telnet_object[g_device_list[idx]].display_stp_brief(g_device_telnet_list[g_device_list[idx]])
            if None == result :
                print "display stp brief error"
                g_device_telnet_list[g_device_list[idx]].close()
                g_device_telnet_status[g_device_list[idx]] = 0
                continue

            #process device interface stp stat string
            curr_itf_stp_stat_list = process_interface_stp(result)
            items_num = len(curr_itf_stp_stat_list);
            for itm_num in range(items_num) :
                item = curr_itf_stp_stat_list.popitem()
                #print item[1]
                if g_dict_itf_stp_stat_list[idx].has_key(item[0]) :
                    #print g_dict_itf_stat_list[idx].get(item[0])
                    if item[1] != g_dict_itf_stp_stat_list[idx].get(item[0]):
                        print "port ", item[0], "changed from", g_dict_itf_stp_stat_list[idx].get(item[0]), "to", item[1]
                        g_dict_itf_stp_stat_list[idx][item[0]] = item[1]             
        
    print "process", times_count, "over"
    #print "over"
    times_count = times_count + 1

