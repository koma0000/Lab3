# -*- coding: utf-8 -*-
#!/usr/bin/env python

import sys, codecs
import cgi
import html

from Client import *

sys.stdout = codecs.getwriter('utf-8')(sys.stdout.detach())

form = cgi.FieldStorage()

myClient = Client(form)
myClient.myID = int(form.getvalue('myID', 0))
if myClient.myID == 0:
    myClient.start()

action = form.getvalue('action')
mForm = None

if action == 'get':
    mylist = myClient.getAll()
    if mylist is None:
        mylist = 'No messages.'
elif action == 'post':
    mylist = ''
    mId = int(form.getfirst("input_id", 0))
    mData = form.getfirst("input_mess", "")
    mData = html.escape(mData)
    mForm = {'id': mId, 'data': mData}
    res = myClient.post(mId, mData)
else:
    mylist = ''

if mForm is not None:
    if res == True:
        pub = '''<font color="green">OK</font>'''
    else:
        pub = '''<font color="red">Something went wrong</font>'''
else:
    pub = ' '

#if mylist is None:
#    mylist = ''


pattern = "".join(["""<!DOCTYPE HTML><html><head><meta charset="utf-8"><title>CGI CLient</title></head><body><form action="/cgi-bin/form.py" name="form"><h1>My ID = """, str(myClient.myID), """ </h1><input type="number" name="input_id"><input type="text" name="input_mess"> <input type=hidden name=myID value=""",str(myClient.myID),""">
<input type=hidden name="action" value='post'>
<button type="submit">Отправить</button>  
<button onclick="document.forms.form.action.value='get';  document.forms.form.submit();">Загрузить</button>
</form>""", mylist, pub, """</body></html>"""])

# if mForm is not None:
#    if res == True:
#        pub = '''<font color="green">OK</font>'''
#    else:
#        pub = '''<font color="red">Something went wrong</font>'''
#else:
#    pub = ''

#if list is None:
#    list = ''

print('Content-type: text/html\n')
print(pattern)
#print(pattern.format(messages=list, results=pub))