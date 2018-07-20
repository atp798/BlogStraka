# -*- coding: utf-8 -*-
# filename: user.py
import json
import time

class User(object):
    userdic = {}
    sysdic = {}
    weatherdic = {}
            
    @staticmethod
    def __init__():
        with open("user.txt", 'rt') as userinfo:
            User.userdic = json.load(userinfo)
        with open("sysinfo.txt", 'rt') as sysinfo:
            User.sysdic = json.load(sysinfo)
        with open("weather.txt", 'rt') as weatherinfo:
            User.weatherdic = json.load(weatherinfo)
    
    @staticmethod
    def SetUserName(openid,name):
        ret = ""
        if User.userdic.has_key(openid):
            User.userdic[openid]=name
            ret = "名字更改成功"
        else:
            User.userdic[openid]=name
            ret = "名字添加成功"
        with open("user.txt", 'wt') as userinfo:
            userinfo.write(json.dumps(User.userdic))
        return ret
        
    @staticmethod        
    def GetUserName(openid):
        ret = ""
        if User.userdic.has_key(openid):
            return User.userdic[openid]
        else:
            return openid
    
    @staticmethod
    def TestUserExist(openid):
        if User.userdic.has_key(openid):
            return True
        else:
            return False
    
    @staticmethod
    def GetWeather():
        wcontent = "数据未采集"
        setok = False
        try:
            wcontent = "今日--天气:\n"
            wcontent += "温度: %s ℃\n" % User.weatherdic["temperature"]
            wcontent += "湿度：%s %%\n" % User.weatherdic["humidity"]
            wcontent += "大气压强：%s kPa\n" % User.weatherdic["pressure"]
            wcontent += "PM2.5: %s μg/㎥\n" % User.weatherdic["PM2.5"]
            wcontent += "PM10: %s μg/㎥\n" % User.weatherdic["PM10"]
            #wcontent += "PM0.3~2.5: %s μg/㎥\n" % User.weatherdic["PM0.3~2.5"]
            #wcontent += "PM2.5~10: %s μg/㎥\n" % User.weatherdic["PM2.5~10"]
            wcontent += "数据更新于 %s " % User.weatherdic["update"]
            setok = True
        finally:
            if not setok:
                wcontent = "数据未采集"
            return wcontent

    @staticmethod
    def SetWeather(recTxt):
        ret = "天气数据设置失败"
        content = recTxt[5:]
        contlist = content.split(";")
        if len(contlist)==5:
            User.weatherdic["temperature"] = contlist[0]
            User.weatherdic["humidity"] =contlist[1]
            User.weatherdic["pressure"] =contlist[2]
            User.weatherdic["PM2.5"] =contlist[3]
            User.weatherdic["PM10"] =contlist[4]
            #User.weatherdic["PM0.3~2.5"] =contlist[5]
            #User.weatherdic["PM2.5~10"] =contlist[6]
            User.weatherdic["update"] = time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(time.time()+13*60*60))
            with open("weather.txt", 'wt') as weatherinfo:
                weatherinfo.write(json.dumps(User.weatherdic))
                ret = "天气数据设置成功"
        return ret
            
    @staticmethod        
    def GetSysinfo(key):
        if User.sysdic.has_key(key):
            return User.sysdic[key]
        else:
            return ""
            
    @staticmethod
    def SetSysinfo(key,value):
        ret = ""
        User.sysdic[key] = value
        with open("sysinfo.txt",'wt') as sysinfo:
            sysinfo.write(json.dumps(User.sysdic))
            ret = "系统信息设置成功"
        return ret
    