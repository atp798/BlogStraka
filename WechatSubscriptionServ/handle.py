# -*- coding: utf-8 -*-
# filename: main.py

import hashlib
import web
import reply
import receive
import time
import datetime
from user import User
from getweather import weather
import re
import json

class Handle(object):
    def __init__(self):
        self.strMenu = '回复"1"查看马道桥天气\n回复"2"查看今日伙食\n回复"3"查看今日班车\n'
        self.strMenu += '回复"4"维修报备\n回复"5"寻医问药\n回复"6"意见建议\n回复"7"设置姓名'
        
    def GET(self):
        try:
            data = web.input()
            if len(data) == 0:
                return "Website not found!"
            signature = data.signature
            timestamp = data.timestamp
            nonce = data.nonce
            echostr = data.echostr
            token = "------"
            list = [token, timestamp, nonce]
            list.sort()
            sha1 = hashlib.sha1()
            map(sha1.update, list)
            hashcode = sha1.hexdigest()
            print "handle/GET func: hashcode, signature: ", hashcode, signature
            if hashcode == signature:
                return echostr
            else:
                return ""
        except Exception, Argument:
            return Argument

    def POST(self):
        try:
            webData = web.data()
            print "Handle Post webdata is \n", webData   #后台打日志
            recMsg = receive.parse_xml(webData)
            if isinstance(recMsg, receive.Msg):
                if recMsg.MsgType == 'text':
                    toUser = recMsg.FromUserName
                    fromUser = recMsg.ToUserName
                    content = ""
                    if recMsg.Content.startswith("1"):
                        recContlow = recMsg.Content.lower()
                        wget = weather()
                        if recContlow.startswith("1tz") or recContlow.startswith("1北京"):
                            content = wget.GetWeather('d7_beijing')
                            self.WriteToLog("GW:"+User.GetUserName(recMsg.FromUserName)+":查询北京天气")
                        elif recContlow.startswith("1xa") or recContlow.startswith("1西安"):
                            content = wget.GetWeather('d7_xian')
                            self.WriteToLog("GW:"+User.GetUserName(recMsg.FromUserName)+":查询西安天气")
                        else:
                            with open("weather.txt", 'rt') as weatherinfo:
                                User.weatherdic = json.load(weatherinfo)
                            lastUpdateTime = time.mktime(time.strptime(User.weatherdic["update"], "%Y-%m-%d %H:%M:%S"))
                            timeNow = time.time() + 13 * 60 * 60
                            if timeNow - lastUpdateTime > 3600:
                                content = wget.GetWeather()
                            else:
                                content = User.GetWeather()
                            self.WriteToLog("GW:"+User.GetUserName(recMsg.FromUserName)+":查询马道天气")
                    elif recMsg.Content.startswith("2") or recMsg.Content.startswith("菜谱"):
                        content = self.GetCookBook()
                        self.WriteToLog("GC:"+User.GetUserName(recMsg.FromUserName)+":获取菜谱")
                    elif recMsg.Content.startswith("3") or recMsg.Content.startswith("班车"):
                        content = "该功能尚未开通，敬请期待"
                        self.WriteToLog("GB:" + User.GetUserName(recMsg.FromUserName) + ":查询班车")
                    elif recMsg.Content.startswith("4"):
                        content = self.RepairReport(recMsg)
                    elif recMsg.Content.startswith("5"):
                        content = self.MedicineReport(recMsg)
                    elif recMsg.Content.startswith("6"):
                        content = self.SuggestFeedback(recMsg)
                    elif recMsg.Content.startswith("7"):
                        content = self.SetName(recMsg)
                    elif recMsg.Content.startswith("wset:"):
                        content = User.SetWeather(recMsg.Content)
                        self.WriteToLog("WSET:"+User.GetUserName(recMsg.FromUserName)+":设置天气:"+ recMsg.Content[5:])
                    elif recMsg.Content.startswith("logget:"):
                        content = self.GetLogReport()
                    elif recMsg.Content.startswith("norecv:"):
                        print "rec"
                        content = self.SetLogNotReceive()
                    else:
                        content = self.strMenu
                    if content == "":
                        content = "公众号后台繁忙，请稍后再试"
                    print "reply is :\n" + content
                    replyMsg = reply.TextMsg(toUser,fromUser,content)
                    return replyMsg.send()
                elif recMsg.MsgType == 'event':
                    toUser = recMsg.FromUserName
                    fromUser = recMsg.ToUserName
                    content = ""
                    if recMsg.Event == "subscribe":
                        content = "感谢您的订阅，这里是----，竭诚为您服务\n\n" + self.strMenu
                        replyMsg = reply.TextMsg(toUser,fromUser,content)
                        return replyMsg.send()
                    else:
                        return ""
                else:
                    return ""
        except Exception, Argment:
            return Argment
        
    def WriteToLog(self,content):
        try:
            f=open(r"syslog.txt","a")
            f.write(time.strftime("(%Y-%m-%d %H:%M:%S)",time.localtime(time.time()+13*60*60)))
            f.write(content+"\n")
        except IOError as err:
            print ('File Error:'+str(err)) 
        finally:
            if f:
                f.close()
                
    def GetLogReport(self):
        lastlogtime = User.GetSysinfo("LastLogGetTime")
        timelast = time.strptime(lastlogtime,"%Y-%m-%d %H:%M:%S")
        strret = ""
        ctn = 0
        try:
            f=open(r"syslog.txt","r")
            loglist = f.readlines()
            for logi in loglist:
                timelogstr = logi[logi.index("(")+1:logi.index(")")-1]
                timelog = time.strptime(timelogstr,"%Y-%m-%d %H:%M:%S")
                if timelog>timelast:
                    strret += logi
                    ctn += 1
            strret = strret[0:-1]
            f.close()
            User.SetSysinfo("LastLogGetTimeBak",lastlogtime)
            User.SetSysinfo("LastLogGetTime",time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(time.time()+13*60*60)))
            if ctn>0:
                return strret
            else:
                return "没有更新的日志信息"
        except Exception as e:
            return "日志信息获取失败："+str(e)
        finally:
            if f:
                f.close()
    
    def SetLogNotReceive(self):
        if User.GetSysinfo("LastLogGetTime") == User.GetSysinfo("LastLogGetTimeBak"):
            return "日志未接受设置未执行：系统已确认接收"
        else :
            if User.SetSysinfo("LastLogGetTime",User.GetSysinfo("LastLogGetTimeBak"))=="":
                return "日志未接收设置失败"
            else:
                return "日志未接收设置成功"
    
    def GetCookBook(self):
        wcontent = "本周菜谱未上传"
        cbfilename = time.strftime(r"cookbook/%y-%W.txt",time.localtime())
        dayofweek = (int(time.strftime("%w",time.localtime(time.time())))+6)%7
        hourofday = int(time.strftime("%H",time.localtime(time.time())))
        if hourofday > 10:
            dayofweek = (dayofweek+1)%7
        hourofday = (hourofday+13)%24
        print cbfilename
        try:
            f = open(cbfilename,"r")
            wlist=f.readlines()
            startline = dayofweek * 13
            meal = 0
            strmeal = ""
            if hourofday <8:
                startline += 1
                strmeal = "今天的早饭"
            elif hourofday <13:
                startline += 5
                meal = 1
                strmeal = "今天的午饭"
            elif hourofday <19:
                startline += 9
                meal = 2
                strmeal = "今天的晚饭"
            else:
                startline += 14
                strmeal = "明天的早饭"
            wcontent = strmeal + "是:"
            wcontent += "\n主食：" + self.GetDishes(wlist[startline])
            wcontent += "\n副食：" + self.GetDishes(wlist[startline+1])
            wcontent += "\n汤、其他:" + self.GetDishes(wlist[startline+2])
            if meal==1: 
                wcontent += "\n水果:" + self.GetDishes(wlist[startline+3])
            if len(wlist)>91:
                wcontent += "\n本周菜谱由@" + wlist[91][wlist[91].find(":")+1:-1] + "上传"
            return wcontent 
        except Exception as e:
            return "本周菜谱未上传"
           
    def GetDishes(self,strline):
        strline = strline[strline.find(":")+1:-1]
        dlist = strline.split("\\")
        dstr = ""
        for dish in dlist:
            dstr += "\n  "+dish
        return dstr 
        
    def RepairReport(self,recMsg):
        strret = "该功能尚未开通，敬请期待"
        content = recMsg.Content.strip()
        if not User.TestUserExist(recMsg.FromUserName):
            strret = "请先回复7添加姓名"
        elif content[1:]=="":
            strret = '请回复4+报备维修的物品及位置，如"4宿舍209卫生间排气扇损坏"'
        else:
            self.WriteToLog("RR:"+User.GetUserName(recMsg.FromUserName)+":"+ content[1:])
            strret = "您的请求已记录，将转达至相关人员，谢谢"
        return strret

    def MedicineReport(self,recMsg):
        strret = "该功能尚未开通，敬请期待"
        content = recMsg.Content.strip()
        if not User.TestUserExist(recMsg.FromUserName):
            strret = "请先回复7添加姓名"
        elif content[1:]=="":
            strret = '请回复5+症状描述、所需的药品或想了解的问题，如"5脚扭伤需要云南白药喷雾剂"'
        else:
            self.WriteToLog("MR:"+User.GetUserName(recMsg.FromUserName)+":"+ content[1:])
            strret = "您的请求已记录，将转达至相关人员，谢谢"
        return strret

    def SuggestFeedback(self,recMsg):
        strret = "该功能尚未开通，敬请期待"
        content = recMsg.Content.strip()
        if content[1:]=="":
            strret = '请回复6+意见或建议，如"6今日班车功能早日能开通"'
        else:
            if User.TestUserExist(recMsg.FromUserName):
                self.WriteToLog("SF:"+User.GetUserName(recMsg.FromUserName)+":"+ content[1:])
            else:
                self.WriteToLog("SF:"+recMsg.FromUserName +":"+ content[1:])
            strret = "您反映的意见、建议已记录，将转达至开发人员，谢谢"
        return strret
        
    def SetName(self,recMsg):
        strret = ""
        content = recMsg.Content.strip()
        if content[1:]=="":
            strret = '请输入7+姓名,例如"7王大锤"\n1、请勿添加其他非空字符\n2、多次添加将做覆盖处理\n3、回复"77"将查看系统录入的姓名"'
        elif content[1]=="7":
            strret = User.GetUserName(recMsg.FromUserName)
        else :
            content = content[1:].decode('utf-8')
            pattern = re.compile(u'([\u4e00-\u9fa5]+)', re.S)
            name = re.findall(pattern,content)
            if name:
                strret = User.SetUserName(recMsg.FromUserName,name[0].encode('utf-8'))
            else:
                strret = "设置姓名失败，请重新输入"
        return strret
        
    



