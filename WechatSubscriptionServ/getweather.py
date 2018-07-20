#    coding=utf-8
#!/usr/bin/env    python
#    -*-    coding:utf-8    -*-
__author__ = 'ATP'
import urllib
import urllib2
import re
import sys
reload(sys)
sys.setdefaultencoding('utf8')

class weather:
    def __init__(self):
        self.root_url = 'http://www.22086.com/'

    def GetWeather(self,wurl='d7_sanhe'):
        crawurl = self.root_url+wurl+'.html' 
        strret = ""
        try:
            request  = urllib2.Request(crawurl)
            response = urllib2.urlopen(request)
            content  = response.read().decode('utf-8')
            pattern  = re.compile(ur'\u53d1\u5e03\u7684(.*?)\u4eca\u65e5',re.S)
            place = re.findall(pattern,content)
            if wurl != 'd7_sanhe':
                strret = place[0]+"实时天气：\n"
            else:
                strret = "--实时天气：\n"
            pattern = re.compile(r'hours36">'
                                r'<li>(.*?)</li><li>(.*?)</li><li>(.*?)</li><li>(.*?)</li>'
                                r'<li>(.*?)</li><li>(.*?)</li><li>(.*?)</li><li>(.*?)</li>'
                                r'<li>(.*?)</li><li>(.*?)</li><li>(.*?)</li><li>(.*?)</li>'
                                r'<li>(.*?)</li><li>(.*?)</li><li>(.*?)</li><li>(.*?)</li>'
                                r'</div>',re.S)
            itemlist = re.findall(pattern,content)
            for i in range(0,15):
                strret += itemlist[0][i]+'\n'
            updatetime = itemlist[0][15]
            pattern = re.compile(r'(hours36kq">)',re.S)
            something = re.findall(pattern,content)
            if something:
                    pattern = re.compile(r'hours36kq">'
                                        r'<li>(.*?)</li><li>(.*?)</li><li>(.*?)</li><li>(.*?)</li>'
                                        r'<li>(.*?)</li><li>(.*?)</li><li>(.*?)</li><li>(.*?)</li>'
                                        r'</div>',re.S)
                    itemlist = re.findall(pattern,content)
                    for item in itemlist[0]:
                            strret    +=    item + '\n'        
            strret += updatetime
        except Exception,e:
                print    e
        return strret                        