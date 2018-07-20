# -*- coding: utf-8 -*-
# filename: main.py
import web
from handle import Handle
from user import User
import sys
reload(sys)
sys.setdefaultencoding('utf8')   

urls = (
    '/wx', 'Handle',
)

if __name__ == '__main__':
    User.__init__()
    app = web.application(urls, globals())
    app.run()
