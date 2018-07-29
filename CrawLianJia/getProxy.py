# coding=utf-8
#!/usr/bin/env python
# -*- coding:utf-8 -*-
__author__ = 'ATP'
import urllib
import urllib2
import re
import time
import threading
import socket
from bs4 import BeautifulSoup
import sys
import random
import getProxy
reload(sys)
sys.setdefaultencoding('utf-8')

BSparser = 'html.parser'
UserAgents=['Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.6) Gecko/20091201 Firefox/3.5.6',
            'Mozilla/5.0 (Windows NT 6.2) AppleWebKit/535.11 (KHTML, like Gecko) Chrome/17.0.963.12 Safari/535.11',
            'Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.2; Trident/6.0)',
            'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:34.0) Gecko/20100101 Firefox/34.0',
            'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/44.0.2403.89 Chrome/44.0.2403.89 Safari/537.36',

            'Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_8; en-us) AppleWebKit/534.50 (KHTML, like Gecko) Version/5.1 Safari/534.50',
            'Mozilla/5.0 (Windows; U; Windows NT 6.1; en-us) AppleWebKit/534.50 (KHTML, like Gecko) Version/5.1 Safari/534.50',
            'Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0',
            'Mozilla/5.0 (Macintosh; Intel Mac OS X 10.6; rv:2.0.1) Gecko/20100101 Firefox/4.0.1',
            'Mozilla/5.0 (Windows NT 6.1; rv:2.0.1) Gecko/20100101 Firefox/4.0.1',

            'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_7_0) AppleWebKit/535.11 (KHTML, like Gecko) Chrome/17.0.963.56 Safari/535.11',
            'Opera/9.80 (Macintosh; Intel Mac OS X 10.6.8; U; en) Presto/2.8.131 Version/11.11',
            'Opera/9.80 (Windows NT 6.1; U; en) Presto/2.8.131 Version/11.11',
            'Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko',
            'Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/44.0.2403.130 Safari/537.36']
UserAgents_dicitem=[{'User-Agent':'Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US; rv:1.9.1.6) Gecko/20091201 Firefox/3.5.6'},\
    {'User-Agent':'Mozilla/5.0 (Windows NT 6.2) AppleWebKit/535.11 (KHTML, like Gecko) Chrome/17.0.963.12 Safari/535.11'},\
    {'User-Agent':'Mozilla/5.0 (compatible; MSIE 10.0; Windows NT 6.2; Trident/6.0)'},\
    {'User-Agent':'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:34.0) Gecko/20100101 Firefox/34.0'},\
    {'User-Agent':'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/44.0.2403.89 Chrome/44.0.2403.89 Safari/537.36'},\

    {'User-Agent':'Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_8; en-us) AppleWebKit/534.50 (KHTML, like Gecko) Version/5.1 Safari/534.50'},\
    {'User-Agent':'Mozilla/5.0 (Windows; U; Windows NT 6.1; en-us) AppleWebKit/534.50 (KHTML, like Gecko) Version/5.1 Safari/534.50'},\
    {'User-Agent':'Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0'},\
    {'User-Agent':'Mozilla/5.0 (Macintosh; Intel Mac OS X 10.6; rv:2.0.1) Gecko/20100101 Firefox/4.0.1'},\
    {'User-Agent':'Mozilla/5.0 (Windows NT 6.1; rv:2.0.1) Gecko/20100101 Firefox/4.0.1'},\

    {'User-Agent':'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_7_0) AppleWebKit/535.11 (KHTML, like Gecko) Chrome/17.0.963.56 Safari/535.11'},\
    {'User-Agent':'Opera/9.80 (Macintosh; Intel Mac OS X 10.6.8; U; en) Presto/2.8.131 Version/11.11'},\
    {'User-Agent':'Opera/9.80 (Windows NT 6.1; U; en) Presto/2.8.131 Version/11.11'},
    {'User-Agent':'Mozilla/5.0 (Windows NT 6.3; WOW64; Trident/7.0; rv:11.0) like Gecko'},
    {'User-Agent':'Mozilla/5.0 (Windows NT 6.3; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/44.0.2403.130 Safari/537.36'}
    ]

#obtain proxy from proxy360.cn
class proxy_proxy360:
    #
    def __init__(self):
        self.region=['Brazil','China','America','Taiwan','Japan','Thailand','Vietnam','bahrein']
        self.root_url = 'http://www.proxy360.cn/Region/'
        self.qqproxy_url='http://www.proxy360.cn/QQ-Proxy'
        self.msnproxy_url = 'http://www.proxy360.cn/MSN-Proxy'

    def getUrlList(self,proxyType='normal',proxyLocation='China'):
        url = []
        if proxyType == 'normal' or proxyType == 'all':
            if proxyLocation == 'all':
                for ri in self.region:
                    url.append(self.root_url + ri)
            else:
                url.append(self.root_url + proxyLocation)
        if proxyType == 'qq' or proxyType == 'all':
            url.append(self.qqproxy_url)
        if proxyType == 'msn' or proxyType == 'all':
            url.append(self.msnproxy_url)
        return url

    def getProxy(self,url=[]):
        items = []
        try:
            for urlitem in url:
                print 'Get proxy from url: ' + urlitem
                #request = urllib2.Request(urlitem,headers=headers)
                request = urllib2.Request(urlitem)
                response = urllib2.urlopen(request)
                content = response.read().decode('utf-8')
                pattern = re.compile(r'<div.*?proxylistitem.*?>.*?>'
                                        r'.*?>\s*(.*?)\s*</span>'
                                        r'.*?>\s*(.*?)\s*</span>'
                                        r'.*?>\s*(.*?)\s*</span>'
                                        r'.*?>\s*(.*?)\s*</span>'
                                        r'.*?>\s*(.*?)\s*</span>'
                                        r'.*?>\s*(.*?)\s*</span>'
                                        r'.*?>\s*(.*?)\s*</span>'
                                        r'.*?>\s*(.*?)\s*</span>'
                                        r'.*?/div>.*?>.*?<div.*?title="(.*?)">', re.S)
                itemslist = re.findall(pattern,content)
                for itemsnew in itemslist:
                    itemproperty = {}
                    itemproperty['ip']=itemsnew[0]
                    itemproperty['port'] = itemsnew[1]
                    itemproperty['anony_degree'] = itemsnew[2]
                    itemproperty['location'] = itemsnew[3]
                    itemproperty['updata_time'] = itemsnew[4]
                    itemproperty['today_mark'] = itemsnew[5]
                    itemproperty['total_mark'] = itemsnew[6]
                    itemproperty['available_time'] = itemsnew[7]
                    itemproperty['speed'] = itemsnew[8]
                    items.append(itemproperty)
        except (urllib2.HTTPError, urllib2.URLError), e:
            print e
        except Exception,e:
            print e
        return items

#obtain proxy from kuaidaili.com
class proxy_kuaidaili:
    def __init__(self):
        self.root_latest = 'http://www.kuaidaili.com/proxylist/'
        self.root_free = 'http://www.kuaidaili.com/free/'
        self.freeType = ['latest','inha','intr','outha','outtr']

    def getUrlList(self,proxyType='latest',pageNum=10):
        url = []
        if proxyType == 'latest':
            for i in range(0,min(pageNum,10)):
                url.append(self.root_latest+str(i+1))
        elif proxyType in self.freeType:
            try:
                urlitem = self.root_free + proxyType
                request = urllib2.Request(urlitem)
                response = urllib2.urlopen(request)
                content = response.read().decode('utf-8')
                pattern = re.compile(u'<div id="listnav".*?<ul.*>(.*?)</a></li><li>\u9875</li></ul>',re.S)
                lastPage = re.findall(pattern,content)
                if lastPage:
                    for i in range(0,min(pageNum,int(lastPage[0]))):
                        url.append(urlitem+'/'+str(i+1))
            except (urllib2.HTTPError, urllib2.URLError), e:
                 print e
            except Exception,e:
                 print e
        else:
            print "Input error no such type of proxy:"+proxyType
        return url

    def getProxy(self,url=[]):
        proxy = []
        for urlitem in url:
            try:
                print "Get proxy from url: " + urlitem
                request = urllib2.Request(urlitem)
                response = urllib2.urlopen(request)
                content = response.read().decode('utf-8')
                pattern = re.compile(r'<thead.*?th>IP<.*?PORT.*?</thead>\s*<tbody>(.*?)</tbody>',re.S)
                proxyTable = re.findall(pattern,content)
                if proxyTable:
                    content=proxyTable[0]
                    pattern = re.compile(r'get/post')
                    bGET_POST = re.findall(pattern,content)
                    if bGET_POST:
                        pattern = re.compile(r'<tr>.*?<td.*?"IP">(.*?)</td>'
                                             r'.*?<td.*?>(.*?)</td>'
                                             r'.*?<td.*?>(.*?)</td>'
                                             r'.*?<td.*?>(.*?)</td>'
                                             r'.*?<td.*?>(.*?)</td>'
                                             r'.*?<td.*?>(.*?)</td>'
                                             r'.*?<td.*?>(.*?)</td>'
                                             r'.*?<td.*?>(.*?)</td>',re.S)
                        proxylistget = re.findall(pattern,content)
                        for proxyitem in proxylistget:
                            proxyitem_property = {}
                            proxyitem_property['ip'] = proxyitem[0]
                            proxyitem_property['port'] = proxyitem[1]
                            proxyitem_property['anony_degree'] = proxyitem[2]
                            proxyitem_property['proxy_type'] = proxyitem[3]
                            proxyitem_property['get_post_support'] = proxyitem[4]
                            proxyitem_property['location'] = proxyitem[5]
                            proxyitem_property['speed'] = proxyitem[6]
                            proxyitem_property['last_verify_time'] = proxyitem[7]
                            proxy.append(proxyitem_property)
                    else :
                        pattern = re.compile(r'<tr>.*?<td.*?"IP">(.*?)</td>'
                                             r'.*?<td.*?>(.*?)</td>'
                                             r'.*?<td.*?>(.*?)</td>'
                                             r'.*?<td.*?>(.*?)</td>'
                                             r'.*?<td.*?>(.*?)</td>'
                                             r'.*?<td.*?>(.*?)</td>'
                                             r'.*?<td.*?>(.*?)</td>', re.S)
                        proxylistget = re.findall(pattern, content)
                        for proxyitem in proxylistget:
                            proxyitem_property = {}
                            proxyitem_property['ip'] = proxyitem[0]
                            proxyitem_property['port'] = proxyitem[1]
                            proxyitem_property['anony_degree'] = proxyitem[2]
                            proxyitem_property['proxy_type'] = proxyitem[3]
                            proxyitem_property['location'] = proxyitem[4]
                            proxyitem_property['speed'] = proxyitem[5]
                            proxyitem_property['last_verify_time'] = proxyitem[6]
                            proxy.append(proxyitem_property)
            except (urllib2.HTTPError, urllib2.URLError), e:
                print e
            except Exception,e:
                print e
        return proxy

class proxy_xicidaili:
    def __init__(self):
        self.root_url = 'http://www.xicidaili.com/'
        self.proxy_type = ['nn','nt','wn','wt','qq']
        self.user_agent = UserAgents[14]

    def getUrlList(self,proxyType='nn',pageNum=10):
        url = []
        if proxyType == 'all':
            for item in self.proxy_type:
                url.extend(self.getUrlList(item,pageNum))
        elif proxyType in self.proxy_type:
            try:
                urlitem = self.root_url + proxyType
                headers = {'User-Agent': self.user_agent}
                request = urllib2.Request(urlitem,headers=headers)
                response = urllib2.urlopen(request)
                content = response.read().decode('utf-8')
                soup = BeautifulSoup(content,BSparser)
                atag = soup.find(class_='next_page')
                num = int(atag.find_previous_sibling().string)
                for i in range(0,min(num,pageNum)):
                    url.append(urlitem+'/'+str(i+1))
            except (urllib2.HTTPError, urllib2.URLError), e:
                 print e
            except Exception,e:
                 print e
        else:
            print "Input error no such type of proxy:" + proxyType
        return url

    def getProxy(self,url=[]):
        proxy = []
        for urlitem in url:
            try:
                print "get proxy from url: " + urlitem
                headers = {'User-Agent': self.user_agent}
                request = urllib2.Request(urlitem, headers=headers)
                response = urllib2.urlopen(request)
                content = response.read().decode('utf-8')
                soup = BeautifulSoup(content, BSparser)
                tabletag = soup.find(id='ip_list')
                thtag = tabletag.find('tr')
                trtags = thtag.find_next_siblings('tr')
                for trtag in trtags:
                    tdtags = trtag.find_all('td',recursive=False)
                    proxyitem_property = {}
                    if tdtags[0].img:
                        proxyitem_property['country']=tdtags[0].img['alt']
                    else:
                        proxyitem_property['country']=tdtags[0].string
                    proxyitem_property['ip'] = tdtags[1].string
                    proxyitem_property['port'] = tdtags[2].string
                    if tdtags[3].a :
                        proxyitem_property['location'] = tdtags[3].a.string
                        locatext = tdtags[3].a['href']
                        pattern = re.compile(r'/[\-0-9]/(.*?)')
                        loca = re.findall(pattern, locatext)
                        if loca:
                            proxyitem_property['location-pinyin'] = loca[0]
                    else:
                        proxyitem_property['location'] = tdtags[3].string
                    proxyitem_property['anony_degree'] = tdtags[4].string
                    proxyitem_property['proxy_type'] = tdtags[5].string
                    proxyitem_property['speed'] = tdtags[6].div['title']
                    proxyitem_property['connect_speed'] = tdtags[7].div['title']
                    proxyitem_property['alive_time'] = tdtags[8].string
                    proxyitem_property['last_verify_time'] = tdtags[9].string
                    proxy.append(proxyitem_property)
            except (urllib2.HTTPError, urllib2.URLError), e:
                print e
            except Exception,e:
                print e
        return proxy

#validate the access of target_url using the proxy in proxylist, if key_word found in response, return true 
def proxyValidate(target_url, key_word, proxylist,timeout=5):
    resultList = {}
    for proxy in proxylist:
        try:
            starttime = time.time()
            headers = {'User-Agent': UserAgents[random.randint(0, len(UserAgents) - 1)]}
            proxy_handler = urllib2.ProxyHandler({"http": proxy})
            opener = urllib2.build_opener(proxy_handler)
            urllib2.install_opener(opener)
            request = urllib2.Request(target_url, headers=headers)
            response = urllib2.urlopen(request,timeout=timeout)
            content = response.read().decode('utf-8')
            pattern = re.compile(unicode(key_word), re.S)
            keywordFind = re.findall(pattern, content)
            endtime = time.time()
            if keywordFind:
                proxyAvailable = 'true'
                print '   proxy: %s --%s  thread: %s   time %s'% (proxy,'success',threading.current_thread().name,endtime - starttime)
                plistlock.acquire()
                try:
                    successList[proxy]=endtime-starttime
                finally:
                    plistlock.release()
            else:
                proxyAvailable = 'false'
                print '   proxy: %s --%s  thread: %s   time %s' % (proxy,'failed',threading.current_thread().name,endtime - starttime)
                plistlock.acquire()
                try:
                    failList[proxy]=endtime-starttime
                finally:
                    plistlock.release()
        except (urllib2.HTTPError, urllib2.URLError), e:
            proxyAvailable = str(e)
            print '   proxy: %s -httperror-%s  thread: %s'% (proxy,e,threading.current_thread().name)
        except Exception, e:
            proxyAvailable = str(e)
            print '   proxy: %s -other-%s  thread: %s'% (proxy,e,threading.current_thread().name)
        if proxyAvailable != 'true' and 'false' != proxyAvailable:
            plistlock.acquire()
            try:
                errorList[proxy]=proxyAvailable
            finally:
                plistlock.release()
        resultList[str(proxy)] = proxyAvailable
    return resultList

#what if python2.7 support variable parameters, things will go easier...make it more universally
def multiThreadValidate(dealList,threadnum,func,args,joinImmediate = True):
    if len(dealList) == 0:
        return
    successList.clear()
    failList.clear()
    errorList.clear()
    listLen = len(dealList)
    threadlist = []
    apartNum = (len(dealList)-1)/threadnum+1
    if apartNum < 2:
        apartNum = 2
    ctn = apartNum -1
    while ctn < listLen:
        th = threading.Thread(target=func, args=(args[0],args[1],dealList[ctn-apartNum+1:ctn+1]),name=str(ctn / apartNum))
        th.start()
        threadlist.append(th)
        ctn = ctn + apartNum
    if listLen % apartNum != 0:
        th = threading.Thread(target=func, args=(args[0],args[1],dealList[ctn-apartNum+1:listLen]),name=str(ctn / apartNum))
        th.start()
        threadlist.append(th)
    if joinImmediate:
        for ti in threadlist:
            ti.join()
    return threadlist

successList = {}   #format: {'proxyip:port':'time of access',}
failList = {}
errorList = {}
plistlock = threading.RLock()

if __name__=="__main__":
    starttime = time.time()

    """
    spider = proxy_proxy360()
    #proxy_items = spider.getProxy('all','all')
    proxy_items = spider.getProxy('normal','Brazil')
    print len(proxy_items)

    spider = proxy_kuaidaili()
    url = spider.getUrlList('inha',100)
    proxy_list = spider.getProxy(url)
    print len(proxy_list)

    spider = proxy_xicidaili()
    url = spider.getUrlList('nn',10)
    proxy_items=spider.getProxy(url)
    """

    spider = proxy_kuaidaili()
    #url = spider.getUrlList()
    url = spider.getUrlList('outha',10)
    #url = spider.getUrlList(proxyType='wt',pageNum=2)
    proxy_items = spider.getProxy(url)
    #proxy_items = []
    print 'num of proxy to be test: '+ str(len(proxy_items))
    proxylist = []
    for proxyi in proxy_items:
        proxylist.append(proxyi['ip'] + ':' + proxyi['port'])

    socket.setdefaulttimeout(5)
    multiThreadValidate(proxylist,20,proxyValidate,('http://bj.lianjia.com/','了解链家网'.decode()))  

    print len(successList)
    for i in successList:
        print '%s : %s ' % (i,successList[i])
    #print len(failList)
    #print failList
    #print len(errorList)
    endtime = time.time()
    print 'total time cost: %s' % str(endtime-starttime)
