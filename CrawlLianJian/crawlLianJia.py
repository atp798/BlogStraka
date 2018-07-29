# coding=utf-8
#!/usr/bin/env python
# -*- coding:utf-8 -*-
__author__ = 'ATP'
import urllib
import urllib2
import re
import json
import sqlite3
import thread
import threading
import time
import random
import datetime
from bs4 import BeautifulSoup
import getProxy
from getProxy import *
import sys
reload(sys)
sys.setdefaultencoding('utf8')

BSparser = 'html.parser'

#class wrap the operation of sqlite database for better support of multiple threads
class SQLiteWraper(object):
    def __init__(self,conn_arg,command='',*args,**kwargs):  
        self.lock = threading.RLock()
        self.path = conn_arg
        if command!='':
            conn=self.get_conn()
            cu=conn.cursor()
            cu.execute(command)

    def get_conn(self):  
        conn = sqlite3.connect(self.path)#,check_same_thread=False)  
        conn.text_factory=str
        return conn   

    def conn_close(self,conn=None):  
        conn.close()  

    def conn_trans(func):
        def connection(self,*args,**kwargs):  
            self.lock.acquire()  
            conn = self.get_conn()  
            kwargs['conn'] = conn  
            rs = func(self,*args,**kwargs)  
            self.conn_close(conn)  
            self.lock.release()  
            return rs  
        return connection

    @conn_trans
    def execute(self, command, conn=None):
        cu = conn.cursor()
        try:
            cu.execute(command)
            conn.commit()
        except sqlite3.IntegrityError, e:
            print e
            return -1
        except Exception, e:
            print e
            return -2
        return 0

    @conn_trans
    def fetchall(self,command="select count(*)",conn=None):
        cu=conn.cursor()
        lists=[]
        try:
            cu.execute(command)
            lists=cu.fetchall()
        except Exception,e:
            print e
        return lists

class crawlLianJia:
    def __init__(self):
        self.__indebug = False
        self.crawlock = threading.RLock()
        self.proxylock = threading.RLock()
        
        self.city_urlhead = 'bj'
        self.root_url='http://'+self.city_urlhead+'.lianjia.com/'
        self.rent_urlword='zufang'
        self.comm_urlword = 'xiaoqu'
        self.housestat_urlword = 'housestat'
        self.mainRegionDic={}
        self.regionDic={}
        self.regionPageNumDic={}
        self.regionUnrepeat = []
        self.regionToMain = {}
        self.proxyNeedUpdateNum = 10 #too large of this num leads to frequently update, some unknown problem occur
        self.proxyValidateNum = 10  #too many threads will lead to some unknown problems
        self.errorListFileName = 'errorList'
        
        #edit by multi threads
        self.allPages = []
        self.errorPageUrlList=[]
        self.errorRoomList = []
        self.errorCommList = []
        self.proxyList = []
        self.proxyForceoutList = []

        self.sqlitedb = SQLiteWraper('lianjia.db')

    def threadUpdateProxy(self):
        spiderKuai = proxy_kuaidaili()
        urlkuai = spiderKuai.getUrlList('latest',10)
        urlkuai.extend(spiderKuai.getUrlList('inha', 5))
        urlkuai.extend(spiderKuai.getUrlList('intr', 5))
        urlkuai.extend(spiderKuai.getUrlList('outha', 5))
        urlkuai.extend(spiderKuai.getUrlList('outtr', 5))
        spiderXici = proxy_xicidaili()
        urlxici = spiderXici.getUrlList('nn', pageNum=1)
        urlxici.extend(spiderXici.getUrlList('nt', pageNum=1))
        urlxici.extend(spiderXici.getUrlList('wn', pageNum=1))
        urlxici.extend(spiderXici.getUrlList('wt', pageNum=1))
        tmpCnt = 0
        while True:
            #print '-Proxy update thread is running '
            try:
                self.proxylock.acquire()
                try:
                    proxyListOld = self.proxyList
                finally:
                    self.proxylock.release()
                tmpCnt = tmpCnt % 6
                #if 0 == tmpCnt:
                print '--Num of proxy remain in list: '+ str(len(proxyListOld))
                if len(proxyListOld) < self.proxyNeedUpdateNum:
                    proxylist = []
                    proxy_items = spiderKuai.getProxy(urlkuai)
                    proxy_items.extend(spiderXici.getProxy(urlxici))
                    for proxyi in proxy_items:
                        proxylist.append(proxyi['ip'] + ':' + proxyi['port'])
                    #proxylist.extend(proxyListOld)
                    multiThreadValidate(proxylist, self.proxyValidateNum, proxyValidate,
                                      ( 'http://bj.lianjia.com/', '了解链家网'.decode()))
                    print 'Num of newly get proxys: ' + str(len(successList))
                    self.proxylock.acquire()
                    try:
                        for i in successList:
                            if i not in self.proxyForceoutList and i not in self.proxyList:
                                self.proxyList.append(i)
                    finally:
                        self.proxylock.release()
                time.sleep(10)
                tmpCnt = tmpCnt +1
            except Exception,e:
                print 'Some errors occured in proxylist update thread: '+ e.message

    def getContentbyProxy(self,url,succPattern='',failPattern='',succCheckFirst= False,tryMax = 10,timeout = 10):
        getSucc = False  #match the succPattern
        getFail = False  #match the failPattern, use to judge whether the ip has been blocked
        tryCtn = 0
        content = ''
        while tryCtn < tryMax and not getSucc:
            getSucc = False
            getFail = False
            content = ''
            proxy = ''
            needWait = True
            while needWait:
                self.proxylock.acquire()
                try:
                    if len(self.proxyList) > 0:
                        proxy = self.proxyList[random.randint(0, len(self.proxyList) - 1)]
                        needWait = False
                    #proxy = '94.177.241.137:3128'
                finally:
                    self.proxylock.release()
                if needWait:
                    #print '--Wait the proxy list'
                    time.sleep(10)
            try:
                headers = UserAgents_dicitem[random.randint(0,len(UserAgents_dicitem)-1)]
                proxy_handler = urllib2.ProxyHandler({"http": proxy})
                opener = urllib2.build_opener(proxy_handler)
                urllib2.install_opener(opener)
                request = urllib2.Request(url, headers=headers)
                response = urllib2.urlopen(request,timeout=timeout)
                content = response.read().decode('utf-8')
                if not succCheckFirst:
                    if not ''== failPattern and re.findall(failPattern,content):
                        getFail = True
                    elif re.findall(succPattern,content):
                        getSucc = True
                else:
                    if re.findall(succPattern,content):
                        getSucc = True
                    elif not '' == failPattern  and re.findall(succPattern,content):
                        getFail = True
                if getFail:
                    self.proxylock.acquire()
                    try:
                        self.proxyForceoutList.append(proxy)
                        if proxy in self.proxyList:
                            self.proxyList.remove(proxy)
                    finally:
                        self.proxylock.release()
            except (urllib2.HTTPError, urllib2.URLError), e:
                #print '* Network error: ' + e.message
                self.proxylock.acquire()
                try:
                    if proxy in self.proxyList:
                        self.proxyList.remove(proxy)
                finally:
                    self.proxylock.release()
            except Exception, e:
                #print '* Warning * getContent error: ' + e.message
                self.proxylock.acquire()
                try:
                    if proxy in self.proxyList:
                        self.proxyList.remove(proxy)
                finally:
                    self.proxylock.release()
            tryCtn = tryCtn+1
        return getSucc,content

    #craw with single thread, about 1,000 accesses to the lianjia website will be ip blocked
    def crawAllRentInfo(self):
        for subRegion in self.regionUnrepeat:
            pageUrlList = self.crawRentInfo_certainRegion(subRegion)
            for pageurl in pageUrlList:
                self.crawRentInfo_certainPage(pageurl)

    #craw with multiple threads
    def crawAllRentInfo_multi(self,threadnum):
        #self.multiThreadCrawList(threadnum,self.regionUnrepeat,self.crawRentInfo_Regions,threadNamePrefix='-region-')
        #self.updatePageList()
        self.getPageListFromDB()
        self.allPages
        print 'Num of total pages: '+str(len(self.allPages))
        self.multiThreadCrawList(threadnum,self.allPages,self.crawRentInfo_Pages)

    #python 2.7 does not support variable parameters very well, pity
    def multiThreadCrawList(self,threadnum,deallist,func,threadNamePrefix='',joinImmediate = True,*argspass,**kwargs):
        totalPageNum = len(deallist)
        threadlist = []
        apartNum = (totalPageNum - 1) / threadnum + 1
        if apartNum<2:
            apartNum = 2
        ctn = apartNum - 1
        while ctn < totalPageNum:
            th = threading.Thread(target=func,
                                  args=(deallist[ctn - apartNum + 1:ctn + 1],),
                                  name=threadNamePrefix + str(ctn / apartNum))
            th.start()
            threadlist.append(th)
            ctn = ctn + apartNum
        if len(deallist) % apartNum != 0:
            th = threading.Thread(target=func,
                                  args=(deallist[ctn - apartNum + 1:totalPageNum],),
                                  name=threadNamePrefix +  str(ctn / apartNum))
            th.start()
            threadlist.append(th)
        if joinImmediate:
            for ti in threadlist:
                ti.join()
        return threadlist

    #get regions from website than craw by regions because the website only presents 100 pages in response
    def getRegion(self):
        try:
            url = self.root_url+self.rent_urlword
            request = urllib2.Request(url)
            response = urllib2.urlopen(request)
            content = response.read().decode('utf-8')
            pattern = re.compile(u'>\u533a\u57df.*?\u4e0d\u9650</a>(.*?)</dd>', re.S)
            regionText = re.findall(pattern, content)
            if regionText:
                pattern = re.compile(r'<a.*?/'+self.rent_urlword+r'/(.*?)/".*?>(.*?)</a>', re.S)
                mainRegionList=re.findall(pattern,regionText[0])
                for mainRegion in mainRegionList:
                    self.mainRegionDic[mainRegion[0]]=mainRegion[1]
                    print "Get subRegion list of : "+ self.mainRegionDic[mainRegion[0]] + " : " + mainRegion[0]
                    url=self.root_url+self.rent_urlword+"/"+mainRegion[0]
                    request = urllib2.Request(url)
                    response = urllib2.urlopen(request)
                    content = response.read().decode('utf-8')
                    pattern = re.compile(r'sub-option-list">.*?class="on">.*?</a>(.*?)</div>', re.S)
                    subRegionText = re.findall(pattern,content)
                    if subRegionText:
                        pattern = re.compile(r'<a.*?/'+self.rent_urlword+r'/(.*?)/">(.*?)</a>', re.S)
                        subRegionList = re.findall(pattern,subRegionText[0])
                        subListTmp = {}
                        #subListItem = {}
                        for subRegion in subRegionList:
                            #subListItem[subRegion[0]]=subRegion[1]
                            #subListTmp.append(subListItem)
                            subListTmp[subRegion[0]]=subRegion[1]
                        self.regionDic[mainRegion[0]]=subListTmp
            self.updateRegionInfo()
        except (urllib2.HTTPError, urllib2.URLError), e:
            print '* Network error: '+ e.message
        except Exception,e:
            print '* Warning * GetRegion error: '+ e.message

    #save regions and bizcircles(subRegions) crawed to database, because the regions only need to craw once
    def updateRegionInfo(self):
        infoDicList = []
        for itemkey,itemvalue in self.mainRegionDic.iteritems():
            infoDic = {}
            infoDic['typetxt'] = 'mainRegion'
            infoDic['keytxt'] = itemkey
            infoDic['valuetxt'] = itemvalue
            infoDicList.append(infoDic)
        self.insertDicListInfo('globalTag',infoDicList)
        infoDicList = []
        sub2mainDicList = []
        for mainRegion in self.regionDic:
            subRegionDic = dict(self.regionDic[mainRegion])
            for subRegion in subRegionDic:
                if subRegion not in self.regionUnrepeat:
                    self.regionUnrepeat.append(subRegion)
                    self.regionToMain[subRegion] = mainRegion
                    infoDic = {}
                    infoDic['typetxt'] = 'subRegion'
                    infoDic['keytxt'] = subRegion
                    infoDic['valuetxt'] = subRegionDic[subRegion]
                    infoDicList.append(infoDic)
                    smDic = {}
                    smDic['typetxt'] = 'subofMain'
                    smDic['keytxt'] = subRegion
                    smDic['valuetxt'] = mainRegion
                    sub2mainDicList.append(smDic)
        self.insertDicListInfo('globalTag',infoDicList)
        self.insertDicListInfo('globalTag',sub2mainDicList)

    def getRegionFromDB(self):
        self.regionUnrepeat = []
        strsql = 'select * from globalTag where typetxt=\'subRegion\''
        sqlist = self.sqlitedb.fetchall(strsql)
        for item in sqlist:
            self.regionUnrepeat.append(item[2])
        print 'Get regions from database: '+ str(len(self.regionUnrepeat))

    #craw info of the given regions
    def crawRentInfo_Regions(self,subRegionList):
        print 'Thread started : ' + threading.current_thread().name
        for regioni in subRegionList:
            self.crawlock.acquire()
            try:
                self.allPages.extend(self.crawRentInfo_certainRegion(regioni))
            finally:
                self.crawlock.release()

    #return a list of pages to be crawed in the region
    def crawRentInfo_certainRegion(self,region):
        pageUrlList = []
        try:
            print 'Get page list of region: ' + region
            url = self.root_url+self.rent_urlword+'/'+ region
            request = urllib2.Request(url)
            response = urllib2.urlopen(request)
            content = response.read().decode('utf-8')
            pattern = re.compile(r'{"totalPage":([0-9]*?),', re.S)
            pageNumText = re.findall(pattern, content)
            if pageNumText and int(pageNumText[0])>1:
                self.regionPageNumDic[region]=int(pageNumText[0])
                for i in range(self.regionPageNumDic[region]):
                    if 0 == i:
                        pageUrlList.append(url)
                    else:
                        urlpage=self.root_url+self.rent_urlword+"/"+region+"/pg"+str(i+1)
                        pageUrlList.append(urlpage)
            else:
                pageUrlList.append(url)
        except (urllib2.HTTPError, urllib2.URLError), e:
            print '* Network error: ' + e.message
        except Exception, e:
            print '* Warning * crawRegion error: ' + e.message
        return pageUrlList

    #save all pages need be crawed to database
    def updatePageList(self):
        infoDicList = []
        for item in self.allPages:
            infoDic = {}
            infoDic['typetxt'] = 'pageUrl'
            infoDic['valuetxt'] = item
            infoDicList.append(infoDic)
        self.insertDicListInfo('globalTag',infoDicList)

    def getPageListFromDB(self):
        self.allPages = []
        strsql = 'select * from globalTag where typetxt=\'pageUrl\''
        sqlist = self.sqlitedb.fetchall(strsql)
        for item in sqlist:
            self.allPages.append(item[3])
        print 'Num of urls get from database: '+ str(len(self.allPages))

    def crawRentInfo_Pages(self,pageList=[]):
        #print 'Thread started : ' + threading.current_thread().name
        for pagei in pageList:
            self.crawRentInfo_certainPage(pagei)

    def crawRentInfo_certainPage(self,urlpage):
        print "Deal with Page : "+ urlpage
        try:
            if self.__indebug:
                request = urllib2.Request(urlpage)
                response = urllib2.urlopen(request)
                content = response.read().decode('utf-8')
            else:
                getSucc,content = self.getContentbyProxy(urlpage,re.compile(u'>(\u4e86\u89e3\u94fe\u5bb6\u7f51)<'))
                if not getSucc:
                    raise UserWarning('Url open error...')
            pattern = re.compile(r'(<li data-index=.*?</div></li>)', re.S)
            houseInfoList = re.findall(pattern, content)
            for houseListText in houseInfoList:
                houseInfoDic={}
                pattern = re.compile(r'data-id="([A-Z0-9]+?)">'
                                    r'.*?<h2><a.*?>(.*?)</a><'
                                    #u'(<div class="ziroomTag zufang_ziroom" data-index="0">\u81ea\u5982</div>){0,1}'
                                    #u'<div class="info-poanel">.*?title="(.*?)">'
                                    r'.*?<div class="col-1">.*?xiaoqu/(.*?)/".*?region">(.*?)&nbsp.*?meters">([.0-9]*?).*?class="con">.*?/zufang/(.*?)/">.*?</span>.*?<span>/</span>(.*?)<'
                                    r'.*?class="view-label left">(.*?)</div>.*?price-pre">([.0-9]+?)\s',re.S)
                houseItemText = re.findall(pattern,houseListText)
                if houseItemText:
                    houseID =houseItemText[0][0]
                    houseInfoDic['HID'] = houseID
                    houseInfoDic['area'] = houseItemText[0][4]
                    datestring = time.strptime(houseItemText[0][8], u'%Y.%m.%d')
                    houseInfoDic['refreshDate'] = time.strftime('%Y-%m-%d', datestring)
                    commID = houseItemText[0][2]
                    commName = houseItemText[0][3]
                    seeUpdate = False
                    roomCrawed = self.dealRentRoomCrawed(houseID)
                    commCrawed = self.dealRentCommunityCrawed(commID)
                    #if not roomCrawed or not commCrawed:   #execute only if error of database, usually, ram it
                    jsonDic = self.crawRentInfo_json(houseID,commID)
                    if not roomCrawed:
                        print '--Get info of room id: %s' % houseInfoDic['HID']
                        houseInfoDic['name'] = houseItemText[0][1]
                        houseInfoDic['CID'] = commID
                        tagInfoText = houseItemText[0][7]
                        tagInfoDic = self.crawRentInfo_pageTagInfo(tagInfoText)
                        houseInfoDic=dict(houseInfoDic,**tagInfoDic)
                        tagInfoDic,payDicList,dscptDicList,seeDicList,errorMark = self.crawRentInfo_certainRoom(houseID)
                        houseInfoDic = dict(houseInfoDic,**tagInfoDic)
                        self.insertInfo('rentInfo',houseInfoDic)
                        if not errorMark:
                            self.insertDicListInfo('rentPay',payDicList)
                            self.insertDicListInfo('houseTag',dscptDicList)
                            if len(seeDicList)>0:
                                seeUpdate = True
                                self.insertDicListInfo('seeHouse', seeDicList)
                    #else:
                    #    self.updateInfo('rentInfo','HID',houseInfoDic)  #execute only if error of database, usually, ram it
                    if not commCrawed:
                        commInfoDic = {}
                        commInfoDic['buildStyle']=houseItemText[0][6]
                        commInfoDic['bizcircle'] = houseItemText[0][5]
                        commInfoDic['CID'] = commID
                        commInfoDic['name'] = commName
                        print '  Get info of community id: %s' % commID
                        commErrorMark,commDicTmp = self.crawRentInfo_community(commID)
                        commInfoDic = dict(commInfoDic,**commDicTmp)
                        self.insertInfo('communityInfo',commInfoDic)
                    if not jsonDic.has_key('code') or 0 == jsonDic['code']:
                        continue
                    else:
                        if not seeUpdate and jsonDic['data'].has_key('seeRecord'):
                            if int(jsonDic['data']['seeRecord']['totalCnt']) != 0:
                                seeRecordList = jsonDic['data']['seeRecord']['list']
                                for item in seeRecordList:
                                    item['agentId'] = str(item['agentId'])
                                    item['agentName'] = jsonDic['data']['agent'][item['agentId']]['agentName']
                                    item['HID'] = houseID
                                self.insertDicListInfo('seeHouse', seeRecordList)
                        if jsonDic['data'].has_key('resblockSold'):
                            houseList = jsonDic['data']['resblockSold']
                            for roomitem in houseList:
                                roomInfoDic = {}
                                roomInfoDic['isRemove'] = 'True'
                                roomInfoDic['signDate'] = roomitem['transDate']
                                roomInfoDic['signPrice'] = roomitem['price']
                                roomInfoDic['decoration'] = roomitem['decoration']
                                roomInfoDic['HID'] = (re.findall(re.compile(r'zufang\/(.*?).html',re.S),roomitem['viewUrl']))[0]
                                if not self.dealRentRoomCrawed(roomInfoDic['HID']):
                                    roomInfoDic['CID'] = commID
                                    print '--Get info of dealed room id: %s' % roomInfoDic['HID']
                                    roomDicTmp,roomPayDicList,roomDscptDicList,seeDicList,roomErrorMark = self.crawRentInfo_certainRoom(roomInfoDic['HID'],isRemoved=True)
                                    roomInfoDic = dict(roomInfoDic,**roomDicTmp)
                                    self.insertDicListInfo('rentPay',roomPayDicList)
                                    self.insertDicListInfo('houseTag',roomDscptDicList)
                                    self.insertInfo('rentInfo',roomInfoDic)
                                    self.insertDicListInfo('seeHouse', seeDicList)
                else:
                    raise UserWarning('Content uncomplete or format error, can not find text match rules')
        #finally:
        except Exception, e:
           self.crawlock.acquire()
           try:
               self.errorPageUrlList.append(urlpage)
           finally:
               self.crawlock.release()
           print '* Warning * crawPage error: ' +  str(e.message)
         #   print '-debug-'

    def crawRentInfo_pageTagInfo(self,tagText):
        tagDic={}
        pattern_fang05 = re.compile(r'class="fang05-ex"><span>(.*?)<', re.S)
        pattern_subway = re.compile(r'class="fang-subway-ex"><span>(.*?)<', re.S)
        pattern_haskey = re.compile(r'class="haskey-ex"><span>(.*?)<', re.S)
        pattern_decoration = re.compile(r'class="decoration-ex"><span>(.*?)<', re.S)
        pattern_heating = re.compile(r'class="heating-ex"><span>(.*?)<', re.S)
        pattern_unique = re.compile(r'class="unique-ex"><span>(.*?)<', re.S)
        pattern_balcony = re.compile(r'class="indenpendentBalcony-ex"><span>(.*?)<', re.S)
        pattern_bathroom = re.compile(r'class="privateBathroom-ex"><span>(.*?)<', re.S)
        resultText = re.findall(pattern_fang05,tagText)
        if resultText:
            tagDic['schoolNear']=resultText[0]
        resultText = re.findall(pattern_haskey, tagText)
        if resultText:
            tagDic['haskey'] = resultText[0]
        resultText = re.findall(pattern_decoration, tagText)
        if resultText:
            tagDic['decoration'] = resultText[0]
        resultText = re.findall(pattern_unique, tagText)
        if resultText:
            tagDic['uniquex'] = resultText[0]
        resultText = re.findall(pattern_balcony, tagText)
        if resultText:
            tagDic['balcony'] = resultText[0]
        resultText = re.findall(pattern_bathroom, tagText)
        if resultText:
            tagDic['bathroom'] = resultText[0]
        resultText = re.findall(pattern_subway, tagText)
        if resultText:
            tagDic['fromSubway'] = resultText[0]
        resultText = re.findall(pattern_heating, tagText)
        if resultText:
            tagDic['heating'] = resultText[0]
        return tagDic

    def crawRentInfo_certainRoom(self,hid,isRemoved=False):
        urlroom = self.root_url + self.rent_urlword + '/' + str(hid) + '.html'
        #print '    Room url: ' + urlroom
        tagDic = {}
        payDicList = []
        dscptDicList = []
        seeDicList = []
        errorMark = False
        try:
            if self.__indebug:
                request = urllib2.Request(urlroom)
                response = urllib2.urlopen(request)
                content = response.read().decode('utf-8')
            else:
                getSucc,content = self.getContentbyProxy(urlroom, re.compile(u'>(\u4e86\u89e3\u94fe\u5bb6\u7f51)<'))
                if not getSucc:
                    raise UserWarning('Url open error...')

            soup = BeautifulSoup(content, BSparser)

            removetxt = re.findall(re.compile(u'(price isRemove)',re.S),content)
            if isRemoved :
                tagDic['isRemove'] = 'True'
            elif removetxt:
                tagDic['isRemove'] = 'True'
            else:
                tagDic['isRemove'] = 'False'

            zirutxt = re.findall(re.compile(u'(\u81ea\u5982)',re.S),content)
            if not zirutxt:
                tagDic['ziru'] = False

            zfroomtag = soup.find(class_='zf-room')
            if zfroomtag:
                tipstag = soup.find('span',{'class':'tips decoration'})
                if tipstag:
                    tagDic['decoration'] = tipstag.string
                tipstag = soup.find('span', {'class': 'tips occupation'})
                if tipstag:
                    tagDic['roomStatus'] = tipstag.string
                tipstag = soup.find('span', {'class': 'tips independentBalcony'})
                if tipstag:
                    tagDic['balcony'] = tipstag.string
                tipstag = soup.find('span', {'class': 'tips privateBathroom'})
                if tipstag:
                    tagDic['bathroom'] = tipstag.string

                zfcontenttag = soup.find(class_='content zf-content')
                if zfcontenttag:
                    pricetag = zfcontenttag.find(class_='price')
                    rentmonthly = pricetag.find(class_='total')
                    tagDic['rentMonthly']=str(rentmonthly.string)

                ptags = zfroomtag.find_all('p')
                #tagfind = re.findall(re.compile(ur'</i>([.0-9]+?)\u5e73\u7c73.*?</p>',re.S),str(ptags[0]).decode('utf8'))
                tagfind = re.findall(re.compile(ur'</i>(.*?)</p>', re.S),str(ptags[0]).decode('utf8'))
                if tagfind:
                    tagDic['area'] = tagfind[0]
                tagDic['layout'] = (re.findall(re.compile(r'</i>(.*?)</p>',re.S),str(ptags[1])))[0]
                tagDic['floor'] = (re.findall(re.compile(r'</i>(.*?)</p>',re.S),str(ptags[2])))[0]
                tagDic['direction'] =(re.findall(re.compile(r'</i>(.*?)</p>',re.S),str(ptags[3])))[0]
                tagDic['fromSubway'] = (re.findall(re.compile(r'</i>(.*?)</p>',re.S),str(ptags[4])))[0]
                communityID = (re.findall(re.compile(r'xiaoqu/(.*?)/',re.S),str(ptags[5])))[0]
                communityName = (re.findall(re.compile(r'xiaoqu.*?>(.*?)</a>',re.S), str(ptags[5])))[0]
                #self.communityDic[communityID] = communityName
                #tagDic['community'] = communityID
                #commDic['bizcircle'] = (re.findall(re.compile(r'</a>.*?<a.*?zufang/(.*?)/',re.S),str(ptags[6])))[0]
                afterpubdate = re.findall(re.compile(u'</i>([\-0-9]+).*?</p>',re.S),str(ptags[7]).decode('utf8'))
                pubdate =  datetime.datetime.now() - datetime.timedelta(days=int(afterpubdate[0]))
                #pubdate_format = time.strftime('%Y-%m-%d', int(time.mktime(pubdate.timetuple())))
                pubdate_format = pubdate.strftime('%Y-%m-%d')
                tagDic['pubdate'] = pubdate_format

                titletag = soup.find('h1',{'class':'main'})
                if titletag:
                    tagDic['name'] = titletag.string

                zrtag = soup.find(class_='imgContainer')
                if zrtag.label:
                    if u'自如' == zrtag.label.string :
                        tagDic['ziru'] = 'True'
                    else:
                        tagDic['ziru'] = 'False'
                else:
                    tagDic['ziru'] = 'False'

                introtag = soup.find(class_='introContent')
                introcontent = introtag.find(class_='content')
                introli = introcontent.ul.find_all('li')
                rentway = re.findall(re.compile(u'</span>(.*?)</li>',re.S),str(introli[0]))[0]
                tagDic['rentWay'] = rentway.strip()
                if introli[1].a:# room of ziru
                    paymethod = re.findall(re.compile(r'pay">(.*?)</a>', re.S), str(introli[1]))[0]
                else:
                    paymethod = re.findall(re.compile(ur'</span>(.*?)</li>',re.S),str(introli[1]))[0]
                tagDic['payMethod'] = paymethod.strip()
                if len(introli) > 1:
                    roomstatus = re.findall(re.compile(r'</span>(.*?)</li>',re.S), str(introli[2]))[0]
                    tagDic['roomStatus'] = roomstatus.strip()
                if len(introli)>2:
                    heating = re.findall(re.compile(r'</span>(.*?)</li>',re.S), str(introli[3]))[0]
                    tagDic['heating'] = heating.strip()
                introfeature = introtag.find(class_='featureContent')
                if introfeature.ul:
                    introlitext = introfeature.ul.find_all('li')
                    for introli in introlitext:
                        label = re.findall(re.compile(r'"label">(.*?)</span>',re.S),str(introli))
                        text = re.findall(re.compile(r'"text">(.*?)</span>',re.S),str(introli))
                        dscptDicList.append({'HID':hid,'keytxt':label[0],'valuetxt':text[0]})

                paytag = soup.find(class_='pay')
                if paytag:
                    payliList = paytag.ul.find_all('li')
                    payDicItem = {}
                    for payli in payliList:
                        if payli.has_attr('class') and 'first' == payli['class']:
                            continue
                        payDicItem['payMethod'] = (payli.find(class_='way')).string
                        payDicItem['price'] = (payli.find(class_='rent')).string
                        payDicItem['deposit'] = (payli.find(class_='deposit')).string
                        payDicItem['serviceCharge'] = (payli.find(class_='server')).string
                        payDicItem['HID'] = hid
                        payDicList.append(payDicItem)
                if isRemoved:
                    jsonDic = self.crawRentInfo_json(hid, communityID)
                    if jsonDic.has_key('code') and jsonDic['code'] != 0:
                        if jsonDic['data'].has_key('seeRecord'):
                            if int(jsonDic['data']['seeRecord']['totalCnt']) != 0:
                                seeDicList = jsonDic['data']['seeRecord']['list']
                                for item in seeDicList:
                                    item['agentId'] = str(item['agentId'])
                                    item['agentName'] = jsonDic['data']['agent'][item['agentId']]['agentName']
                                    item['HID'] = hid
            else:
                labeltag = soup.find('div',{'class':'view-label'})
                if labeltag:
                    tagDicTmp = self.crawRentInfo_pageTagInfo(str(labeltag))
                    tagDic = dict(tagDic,**tagDicTmp)

                titletag = soup.find('h2', {'class': 'title-box left'})
                if titletag:
                    tagDic['name'] = titletag.string

                emtexttag = soup.find('span',{'class':'em-text'})
                if emtexttag:
                    tagDic['rentMonthly'] = emtexttag.strong.string
                    tagDic['area'] = (re.findall(re.compile(r'/\s*(.*?)㎡'),str(emtexttag.i)))[0]
                infodivtag = soup.find_all('div',{'class':'desc-text clear'})
                if infodivtag:
                    dltag = infodivtag[0].find_all('dl',recursive=False)
                    tagDic['layout'] = dltag[1].dd.string
                    tagDic['direction'] = dltag[2].dd.string
                    tagDic['floor'] = dltag[3].dd.string
                    #(re.findall(re.compile(r'<a.*?zufang/(.*?)/">.*?<a'), str(dltag[4].dd.span)))[0]
                    #commDic['bizcircle'] = (re.findall(re.compile(r'/a>.*?<a.*?zufang/(.*?)/">.*?'), str(dltag[4].dd.span)))[0]
                    datestring = time.strptime(dltag[5].dd.string.strip(),u'%Y\u5e74%m\u6708%d\u65e5')
                    tagDic['pubdate'] = time.strftime('%Y-%m-%d',datestring)
                seelitag = soup.find('li',{'class':'waap-li'})
                if seelitag:
                    seeultag = seelitag.find_all('ul',{'class':'top-dl'})
                    for seetagitem in seeultag:
                        seeDic = {}
                        seeDic['HID']=hid
                        seeDic['seeTime'] = seetagitem.find('li',{'class':'timer visit-date'}).string
                        agenttag = seetagitem.find('p', {'class': 'info-name'})
                        seeDic['agentName'] = re.findall(re.compile(r'info-name">(.*?)<a',re.S),str(seetagitem))[0]
                        seeDic['agentId'] = agenttag.a['data-ucid']
                        seeDic['seeCnt'] = seetagitem.find('li',{'class':'visit-number'}).string
                        seeDicList.append(seeDic)
        #finally:
        except Exception,e:
            self.crawlock.acquire()
            try:
                self.errorRoomList.append(hid)
            finally:
                self.crawlock.release()
            print '* Warning * crawRoom error: '+ e.message + ' url: ' + urlroom
            errorMark = True
        #print '    taginfo: '+ str(tagDic)
        #print '    pay: ' + str(payDicList)
        #print '    community: ' + str(commDic)
        #print '    dscpt: ' + str(dscptDicList)
        #print '    see record: ' + str(seeDicList)
        return tagDic,payDicList,dscptDicList,seeDicList,errorMark

    def crawRentInfo_community(self,cid):
        tagDic = {}
        errorMark = False
        tagDic['CID'] = str(cid)
        try:
            url = self.root_url+ self.comm_urlword +'/' + str(cid)
            #print '    Community url: ' + url
            if self.__indebug:
                request = urllib2.Request(url)
                response = urllib2.urlopen(request)
                content = response.read().decode('utf-8')
            else:
                getSucc, content = self.getContentbyProxy(url, re.compile(u'>(\u4e86\u89e3\u94fe\u5bb6\u7f51)<'))
                if not getSucc:
                    raise UserWarning('Url open error...')
            location = re.findall(re.compile(r'resblockPosition:\'(.*?)\',',re.S),content)
            if location:
                tagDic['location'] = location[0]
            soup = BeautifulSoup(content, BSparser)
            infotags = soup.find_all('span',{'class':'xiaoquInfoContent'})
            tagDic['buildTime'] = infotags[0].string
            tagDic['buildStyle'] = infotags[1].string
            tagDic['propCost'] = infotags[2].string #re.findall(re.compile(r'([0-9.]+)',re.S),str(infotags[2].string))
            tagDic['propCompany'] = infotags[3].string
            tagDic['landAgent'] = infotags[4].string
            tagDic['brief'] = infotags[5].string
            tagDic['buildNum'] = re.findall(re.compile(r'([0-9.]+)',re.S),str(infotags[6].string))[0]
            tagDic['houseNum'] = re.findall(re.compile(r'([0-9.]+)',re.S),str(infotags[7].string))[0]
        except Exception,e:
            self.crawlock.acquire()
            try:
                self.errorCommList.append(cid)
            finally:
                self.crawlock.release()
            print '* Warning * crawComm error: '+ e.message + ' url: ' + url
            errorMark = True
        #print '    commInfo: ' + str(tagDic)
        return errorMark,tagDic

    def crawRentInfo_json(self,HID,CID):
        infodic = {}
        try:
            url = self.root_url+ self.rent_urlword +'/' + self.housestat_urlword + '?hid=' + str(HID)+'&rid=' + str(CID)
            #print '    Json url: ' + url
            if self.__indebug:
                request = urllib2.Request(url)
                response = urllib2.urlopen(request)
                content = response.read().decode('utf-8')
            else:
                getSucc, content = self.getContentbyProxy(url, re.compile(u'(code)'))
                if not getSucc:
                    raise UserWarning('Url open error...')
            infodic = json.loads(content)
        except Exception,e:
            print '* Warning* getJson error: '+ e.message + ' url: '+  url
        return infodic

    #return true of false to indicate whether a room is in database(has been crawed)
    def dealRentRoomCrawed(self,hid):
        result = self.sqlitedb.fetchall('select * from rentInfo where HID=\''+ str(hid) + '\'')
        if  len(result)>0:
            return True
        else:
            return False

    def dealRentCommunityCrawed(self,cid):
        result = self.sqlitedb.fetchall('select * from communityInfo where CID=\'' + str(cid) + '\'')
        if len(result) > 0:
            return True
        else:
            return False

    #insert a new record into a certain table of database
    def insertInfo(self,table='',infoDic={}):
        sqlstr = ''
        try:
            if 0 == len(infoDic):
                return
            strtmp = reduce(lambda x,y:x + ',' + y ,infoDic.keys())
            strtmp2 = reduce(lambda x,y:x + '\',\''+ y ,map(lambda x:str(x),infoDic.values()))
            #strtmp2 = reduce(lambda x,y:x + '\',\''+ y ,str(infoDic.values()))
            strtmp2 = '\''+ strtmp2 + '\''
            sqlstr = 'insert into %s (%s) values (%s)' % (table,strtmp,strtmp2)
            self.sqlitedb.execute(sqlstr)
        except Exception,e:
            print '* Warning * insertInfo error: '+ e.message
            print infoDic
            print sqlstr

    #update the infomation of certain record
    def updateInfo(self,table,primaryKey,infoDic={}):
        sqlstr = ''
        try:
            if 0 == len(infoDic) or not infoDic.has_key(primaryKey):
                return
            strtmp = ''
            primaryValue = ''
            for keyitem,valueitem in infoDic.iteritems():
                if keyitem == primaryKey:
                    primaryValue = valueitem
                else:
                    strtmp = '%s,%s=\'%s\'' % (strtmp,keyitem,valueitem)
            strtmp = strtmp[1:len(strtmp)]
            sqlstr = 'update %s set %s where %s=\'%s\'' % (table,strtmp,primaryKey,primaryValue)
            self.sqlitedb.execute(sqlstr)
        except Exception,e:
            print '* Warning * updateInfo error: '+ e.message
            print infoDic
            print sqlstr

    #insert some key-value pairs to the table
    def insertDicListInfo(self,table='',infoDicList=[]):
        sqlstr = ''
        try:
            if 0 == len(infoDicList) :
                return
            for infoDic in infoDicList:
                strtmp = reduce(lambda x,y:x + ',' + y ,infoDic.keys())
                strtmp2 = reduce(lambda x,y:x + '\',\''+ y ,map(lambda x:str(x),infoDic.values()))
                strtmp2 = '\''+ strtmp2 + '\''
                sqlstr = 'insert into %s (%s) values (%s)' % (table,strtmp,strtmp2)
                self.sqlitedb.execute(sqlstr)
        except Exception,e:
            print '* Warning * insertInfo error: '+ e.message
            print infoDicList
            print sqlstr

    def loadErrorListFromFile(self,filename = 'errorlist'):
        with open(filename,'r') as fileError:
            jsonError = json.load(fileError)
            if jsonError.has_key('pages'):
                self.errorPageUrlList = jsonError['pages']
            if jsonError.has_key('rooms'):
                self.errorRoomList = jsonError['rooms']
            if jsonError.has_key('communities'):
                self.errorCommList = jsonError['communities']

    def dumpErrorListToFile(self,filename = 'errorlist'):
        dicError = {}
        dicError['pages'] = self.errorPageUrlList
        dicError['rooms'] = self.errorRoomList
        dicError['communities'] = self.errorCommList
        with open(filename, 'w') as fileError:
            fileError.write(json.dumps(dicError))

    def recrawError_Comms(self,commList):
        for item in commList:
            errorMark, commInfoDic = self.crawRentInfo_community(item)
            if not errorMark:
                if self.dealRentCommunityCrawed(item):
                    self.updateInfo('communityInfo', 'CID', commInfoDic)
                else:
                    self.insertInfo('communityInfo', commInfoDic)
            else:
                print '--* Recraw community info error: ' + item

    def recrawError_Rooms(self,roomList):
        for roomID in roomList:
            roomInfoDic, payDicList,dscptDicList, seeDicList, errorMark = self.crawRentInfo_certainRoom(roomID)
            if self.dealRentRoomCrawed(roomID):
                self.updateInfo('rentInfo', 'HID', roomInfoDic)
            else:
                self.insertInfo('rentInfo', roomInfoDic)
            self.insertDicListInfo('rentPay', payDicList)
            self.insertDicListInfo('houseTag', dscptDicList)
            if len(seeDicList) > 0:
                self.insertDicListInfo('seeHouse', seeDicList)

    def recrawErrorList_multi(self,threadnum=10):
        print 'Total error page num : ' + str(len(self.errorPageUrlList))
        print 'Total error community num : ' + str(len(self.errorCommList))
        print 'Total error room num : ' + str(len(self.errorRoomList))
        errorComms = self.errorCommList
        self.errorCommList = []
        self.multiThreadCrawList(threadnum,errorComms,self.recrawError_Comms)
        errorRooms = self.errorRoomList
        self.errorRoomList = []
        self.multiThreadCrawList(threadnum, errorRooms, self.recrawError_Rooms)
        errorPages = self.errorPageUrlList
        self.errorPageUrlList = []
        self.multiThreadCrawList(threadnum,errorPages,self.crawRentInfo_Pages)

    def recrawErrorList(self):
        print 'Total error page num : '+ str(len(self.errorPageUrlList))
        #print self.errorPageUrlList
        print 'Total error community num : '+ str(len(self.errorCommList))
        #print self.errorCommList
        print 'Total error room num : '+ str(len(self.errorRoomList))
        #print self.errorRoomList
        errorPages = self.errorPageUrlList
        self.errorPageUrlList = []
        for item in errorPages:
            self.crawRentInfo_certainPage(item)
        errorComms = self.errorCommList
        self.errorCommList = []
        self.recrawError_Comms(errorComms)
        errorRooms = self.errorRoomList
        self.errorRoomList = []
        self.recrawError_Rooms(errorRooms)

def createDB():
    command = ""
    # db_lianjia = SQLiteWraper('lianjia.db')
    command = "CREATE TABLE if not exists rentInfo (HID TEXT PRIMARY KEY UNIQUE, name TEXT, rentMonthly TEXT, area TEXT, floor TEXT, fromSubway TEXT, CID TEXT, pubdate TEXT, refreshDate TEXT, layout TEXT, direction TEXT, " \
              "rentWay TEXT, payMethod TEXT, heating TEXT, schoolNear TEXT, uniquex TEXT, haskey TEXT, decoration TEXT, balcony TEXT, bathroom TEXT, ziru TEXT,roomStatus TEXT, " \
              "isRemove TEXT, signDate TEXT, signPrice TEXT )"
    db_lianjia = SQLiteWraper('lianjia.db', command)

    command = 'create table if not exists houseTag (TID INTEGER PRIMARY KEY AUTOINCREMENT, HID TEXT, keytxt TEXT, valuetxt TEXT)'
    db_lianjia.execute(command)
    command = 'create table if not exists rentPay (PID INTEGER PRIMARY KEY AUTOINCREMENT, HID TEXT, payMethod TEXT, price TEXT,deposit TEXT, serviceCharge TEXT)'
    db_lianjia.execute(command)
    command = 'create table if not exists seeHouse (SID INTEGER PRIMARY KEY AUTOINCREMENT, HID TEXT, seeTime TEXT, agentId TEXT, agentName TEXT, seeCnt TEXT)'
    db_lianjia.execute(command)
    command = "create table if not exists communityInfo (CID TEXT primary key UNIQUE, name TEXT, buildTime TEXT, buildStyle TEXT,propCost TEXT, propCompany TEXT, landAgent TEXT, brief TEXT, buildNum TEXT, houseNum TEXT, location TEXT, bizcircle TEXT )"
    db_lianjia.execute(command)
    command = "create table if not exists globalTag (GID INTEGER PRIMARY KEY AUTOINCREMENT, typetxt TEXT, keytxt TEXT, valuetxt TEXT)"
    db_lianjia.execute(command)

if __name__=="__main__":
    starttime = time.time()
    createDB()
    spider = crawlLianJia()
        #spider.getRegion()
    th = threading.Thread(target=spider.threadUpdateProxy, name='updateProxy')
        #thread.start_new_thread(spider.threadUpdateProxy)
    th.start()
        # print len(spider.regionUnrepeat)
    spider.getRegionFromDB()
        #spider.crawAllRentInfo()
    spider.crawAllRentInfo_multi(10)
        #spider.crawRentInfo_certainRegion('andingmen')
        #th.start()
        #spider.crawRentInfo_certainPage('http://bj.lianjia.com/zufang/huairou/')
    #spider.crawRentInfo_certainRoom('101100493419')
        #spider.crawRentInfo_dealroom('101100407741','1111027382512')
        #spider.crawRentInfo_community('1113173051370523')
    spider.dumpErrorListToFile('errorlist')
    #spider.loadErrorListFromFile()
    spider.recrawErrorList_multi()
    spider.dumpErrorListToFile('errorlist-2')
    spider.recrawErrorList_multi()
    endtime = time.time()
    print 'Total time cost: '+  str(endtime-starttime)
    exit()
