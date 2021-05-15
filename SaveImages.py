# -*- coding: utf-8 -*-

import os.path
import sys
import datetime
import shutil

#python2
#import urllib2
#from HTMLParser import HTMLParser

#python3
import urllib.request
from html.parser import HTMLParser

# How to Install Pillow
# sudo apt install libtiff5
# python -m pip install -U Pillow
from PIL import Image, ImageFilter


DEBUG = 0

def downloadImage(url, savename):
	if DEBUG: print("<downloadImage> url savename={}".format(url,savename))
	#img = urllib.urlopen(url)
	img = urllib.request.urlopen(url)
	localfile = open(savename, 'wb')
	localfile.write(img.read())
	img.close()
	localfile.close()

def rotateImage(filename):
	tmpfile = os.path.join(os.getcwd(), "__temp.jpeg")
	if DEBUG: print("tmpfile={}".format(tmpfile))
	shutil.copyfile(filename, tmpfile)
	im = Image.open(tmpfile)
	im_rotate = im.rotate(90, expand=True)
	im_rotate.save(filename)
	os.remove(tmpfile)
	

def get_url_root(url):
	if("http://" in url):
		url_delet_http = url.lstrip("http://")
		if("/" in url_delet_http):
			url_root = "http://" + url_delet_http[0:url_delet_http.find("/")]
			return url_root
	elif("https://" in url):
		url_delet_http = url.lstrip("https://")
		if("/" in url_delet_http):
			url_root = "http://" + url_delet_http[0:url_delet_http.find("/")]
			return url_root
	return 0

class imgParser(HTMLParser):

	def __init__(self):
		HTMLParser.__init__(self)

	def handle_starttag(self,tagname,attribute):
		if tagname.lower() == "img":
			for i in attribute:
				if i[0].lower() == "src":
					img_url=i[1]
					# 取得した写真のURLを集めたファイルの作成
					f = open("collection_url.txt","a")
					f.write("%s\t"%img_url)
					f.close()

if __name__ == "__main__":
	argvs = sys.argv
	argc = len(argvs)
	if argc == 1:
		print("{0:s} SaveFileName [id]".format(argvs[0]))
		sys.exit(1)

	savefile = argvs[1]
	id = "935"
	if (argc == 3):
		id = argvs[2]
	date_file = os.path.dirname(savefile) + "/date.gif"
	month_file = os.path.dirname(savefile) + "/month.gif"

	NowDate = datetime.datetime.now()
	#print("{:04d} {:02d} {:02d}".format(NowDate.year, NowDate.month, NowDate.day))
	#input_url = "http://www.365calendar.net/index?action=user_calendar_detail&calendar_id={0:s}&target={1:%Y%m%d}".format(id,NowDate)
	input_url = "http://www.365calendar.net/index?action=user_calendar_detail&calendar_id={:s}&target=2020{:02d}{:02d}".format(id, NowDate.month,NowDate.day)
	if DEBUG: print("input_url={}".format(input_url))
	serch_url = input_url
	#htmldata = urllib2.urlopen(serch_url)
	htmldata = urllib.request.urlopen(serch_url)

	date_url = "http://www.365calendar.net/lib_image/calendar/img_num{0:%-d}.gif".format(NowDate)
	if DEBUG: print("date_url={}".format(date_url))

	month_url = "http://www.365calendar.net/lib_image/calendar/img_num{0:%-m}.gif".format(NowDate)
	if DEBUG: print("month_url={}".format(month_url))

	parser = imgParser()
	data = htmldata.read()
	#print("type(data)={}".format(type(data)))
	data = data.decode()
	#print("type(data)={}".format(type(data)))
	#parser.feed(htmldata.read())
	parser.feed(data)

	parser.close()
	htmldata.close()

	# 生成したファイルの読み込み
	f = open("collection_url.txt","r")
	for row in f:
		row_url = row.split('\t')
		len_url = len(row_url)
	f.close()

	number_url = []

	for i in range(0,(len_url-1)):
		number_url.append(row_url[i])

	#print("len_url={}".format(len_url))
	rcode = 1
	for j in range(0,(len_url-1)):
		url = number_url[j]
		if not "user_photo.php" in url:
		   continue

		root_url = get_url_root(serch_url)
		if DEBUG: print("root_url={}".format(root_url))
		downloadImage(root_url + url, savefile)
		rotateImage(savefile)
		#downloadImage(date_url, date_file)
		#downloadImage(month_url, month_file)
		rcode = 0


	# ファイルの削除
	os.remove("collection_url.txt")
	sys.exit(rcode)