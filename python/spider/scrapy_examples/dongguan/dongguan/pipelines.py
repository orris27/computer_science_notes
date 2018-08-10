# -*- coding: utf-8 -*-

# Define your item pipelines here
#
# Don't forget to add your pipeline to the ITEM_PIPELINES setting
# See: https://doc.scrapy.org/en/latest/topics/item-pipeline.html
import codecs
import json
class DongguanPipeline(object):
    def __init__(self):
        self.file=codecs.open('dongguan.json','w',encoding='utf-8')
    def process_item(self,item,spider):
        j=json.dumps(dict(item),ensure_ascii=False)+'\n'
        self.file.write(j)
        return item
    def close_spider(self,spider):
        self.file.close()
        
