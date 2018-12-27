#!/usr/bin/env python2.7
# -*-coding:utf-8 -*

import os, os.path, re, string, json

def getfunction(syscall):
      args=""
      num=1
      for argument in syscall['ARGS']:
            if num>1:
                  args=args+", "
            args=args+argument['TYPE']+" "+argument['NAME']
            num=num+1
      if args=="":
            args="void"
      return str(syscall['RETURN'])+" "+str(syscall['NAME'])+"("+args+")"

def getfunctioninternal(syscall):
      args=""
      argsname=["ebx","esi","edi","ebp"]
      num=1
      for argument in syscall['ARGS']:
            if num>1:
                  args=args+", "
            args=args+"dump->"+argsname[num-1]
            num=num+1
      if syscall.has_key('DUMP'):
            if num>1:
                  args=args+", "
            args=args+"dump"
      return str(syscall['INTERNALNAME'])+"("+args+")"

def get_duplicates(sorted_list):
    duplicates = []
    last = sorted_list[0]
    for x in sorted_list[1:]:
        if x == last:
            duplicates.append(x)
        last = x
    return set(duplicates)

path = "./"
files = os.listdir(path)
pattern = r'\/\* SYSCALL.*END-SYSCALL \*\/'
output_file = "syscalls.txt"
if os.path.exists(output_file):
    os.remove(output_file)
fo = open(output_file, "a+")
fo.write("[")
print "*** Collecte des SYSCALL"
for root, dirs, files in os.walk(path):
    for name in files:
        if name.endswith((".c")):
            if root=="./templates":
                 continue
            input_file = os.path.join(root, name)
            with open(input_file, "r") as fi :
                 content = fi.read()
                 results = re.findall(pattern, content, re.MULTILINE| re.DOTALL)
                 for result in results:
                       print("Fichier :"+os.path.join(root, name))
                       new=string.replace(string.replace(result,"/* SYSCALL ",""),"END-SYSCALL */","")
                       if fo.tell()>2:
                           new=","+new;
                       fo.write(new+"\r\n")
fo.write("]")
fo.seek(0)
alljson=json.load(fo);
print alljson
fo.close()
dup = get_duplicates([syscall['ID'] for syscall in alljson]);
if len(dup)>0:
     print "ERREUR : deux fonctions possedent le meme numero de SYSCALL"
     for error in dup:
           for syscall in alljson:
                  if syscall['ID']==error:
                       print "librairie :"+syscall['LIBRARY']+" fonction: "+syscall['INTERNALNAME']+" id:"+str(syscall['ID'])
     exit()
print "*** Mise en place de la documentation"
os.popen('cp ./templates/API.md ./API.md')
alllibs=set([syscall['LIBRARY'] for syscall in alljson])
text=""
for lib in alllibs:
      text=text+"""
###  """+lib.upper()+"""

All fonctions in the \""""+lib+"""\" library.

"""
      for syscall in alljson:
           if syscall['LIBRARY']==lib:
                  textargs=""
                  num=1
                  for argument in syscall['ARGS']:
                        textargs=textargs+"\n* * argument "+str(num)+" : **"+argument['TYPE']+" "+argument['NAME']+"** *"+argument['DESCRIPTION']+"*"
                        num=num+1
                  dump="no"
                  if syscall.has_key('DUMP'):
                        dump="yes"
                  text=text+"""------

`"""+getfunction(syscall)+""";`

*Description:"""+str(syscall['DESCRIPTION'])+"""*

* syscall id : **"""+str(syscall['ID'])+"""**
* arguments : **"""+str(len(syscall['ARGS']))+"""**"""+textargs+"""
* results : **"""+str(syscall['RETURN'])+"""**
* dump of register cpu: **"""+dump+"""**

"""
fo = open("./API.md", "r+")
content = fo.read()
content=content.replace("/* FOR INSERTING */",text)
##print content
fo.seek(0)
fo.write(content)
fo.close()
print "*** Mise en place des libraires (HEADERS)"
for lib in alllibs:
     text=""
     print "Librairie :"+lib
     libname="./programs/include/"+lib+".h"
     os.popen('cp ./templates/lib.h '+libname)
     for syscall in alljson:
           if syscall['LIBRARY']==lib:
                 text=text+getfunction(syscall)+";\n"
     fo = open(libname, "r+")
     content = fo.read()
     content=content.replace("/* FOR INSERTING */",text)
     ##print content
     fo.seek(0)
     fo.write(content)
     fo.close()
print "*** Mise en place des libraires (SOURCES)"
for lib in alllibs:
     text=""
     print "Librairie :"+lib
     libname="./programs/lib/"+lib+".c"
     os.popen('cp ./templates/lib.c '+libname)
     for syscall in alljson:
           if syscall['LIBRARY']==lib:
                 numargs=len(syscall['ARGS'])
                 textargs="syscall"+str(numargs)+"("+str(syscall['ID'])
                 for i in range(0,numargs):
                       if numargs>0:
                            textargs=textargs+","
                       textargs=textargs+"(u32) "+syscall['ARGS'][i]['NAME']
                 if syscall['RETURN']!='void':
                       textargs="return "+textargs+");"
                 else:
                       textargs=textargs+");\n	return;"
                 text=text+getfunction(syscall)+"""
{
	"""+textargs+"""
}

"""
     fo = open(libname, "r+")
     content = fo.read()
     content=content.replace("/* FOR INSERTING */",text)
     ##print content
     fo.seek(0)
     fo.write(content)
     fo.close()
print "*** Mise en place du fichier noyau SYSCALL"
text=""
libname="./lib/syscall.c"
os.popen('cp ./templates/syscall.c '+libname)
for syscall in alljson:
     print "ID :"+str(syscall['ID'])
     call=""
     if syscall['RETURN']!='void':
           call="dump->eax=(u32) "
     call=call+getfunctioninternal(syscall)
     text=text+"""     		case """+str(syscall['ID'])+""":
			"""+call+""";
			break;
"""
fo = open(libname, "r+")
content = fo.read()
content=content.replace("/* FOR INSERTING */",text)
##print content
fo.seek(0)
fo.write(content)
